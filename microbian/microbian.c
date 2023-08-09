/* microbian.c */
/* Copyright (c) 2018 J. M. Spivey */

#include "microbian.h"
#include "lib.h"
#include "hardware.h"
#include <string.h>

#ifdef MICROBIAN_DEBUG
void debug_sched(int pid);
#define DEBUG_SCHED(pid) debug_sched(pid)
#else
#define DEBUG_SCHED(pid)
#endif


/* PROCESS DESCRIPTORS */

/* Each process has a descriptor, allocated when the process is
created.  The next field in the descriptor allows each process to be
linked into at most one queue -- either the queue of ready processes
at some priority level, or the queue of senders waiting to deliver a
message to a particular receiver process. */

typedef struct _proc *proc;

struct _proc {
    int pid;                  /* Process ID (equal to index) */
    char name[16];            /* Name for debugging */
    unsigned state;           /* SENDING, RECEIVING, etc. */
    unsigned *sp;             /* Saved stack pointer */
    void *stack;              /* Stack area */
    unsigned stksize;         /* Stack size (bytes) */
    int priority;             /* Priority: 0 is highest */
    
    proc waiting;             /* Processes waiting to send */
    int pending;              /* Whether HARDWARE message pending */
    int filter;               /* Message type accepted by receive */
    message *msgbuf;          /* Pointer to message buffer */
    proc next;                /* Next process in ready or send queue */
};

/* Possible state values */
#define DEAD 0
#define ACTIVE 1
#define SENDING 2
#define RECEIVING 3
#define SENDREC 4
#define IDLING 5


/* STORAGE ALLOCATION */

/* Stack space for processes is allocated from the low end of the
space between the static data and the main stack.  Process
descriptors are allocated from the opposite end of the space; this
is deliberate to reduce the likelihood that a process overrunning
its stack space will destroy its own descriptor or that of its
neighbour. */

extern unsigned char __stack_limit[], __end[];

static unsigned char *hbot = __end;
static unsigned char *htop = __stack_limit;

#define ROUNDUP(x, n)  (((x)+(n)-1) & ~((n)-1))

/* sbrk -- allocate space at the bottom of the heap */
static void *sbrk(int inc)
{
    hbot = (unsigned char *) ROUNDUP((unsigned) hbot, 8);
    inc = ROUNDUP(inc, 8);

    if (inc > htop - hbot)
        panic("Microbian is out of memory");
    void *result = hbot;
    hbot += inc;
    return result;
}

/* new_proc -- allocate a process descriptor from the top of the heap */
static proc new_proc(void)
{
    if (htop - hbot < sizeof(struct _proc))
        panic("No space for process");
    htop -= sizeof(struct _proc);
    return (proc) htop;
}


/* PROCESS TABLE */

#define NPROCS 32

static proc os_ptable[NPROCS];
static unsigned os_nprocs = 0;

static proc os_current;
static proc idle_proc;

#define BLANK 0xdeadbeef        /* Filler for initial stack */

static void kprintf_setup(void);
static void kprintf_internal(char *fmt, ...);

/* pad -- pad string with spaces to a specified width */
static void pad(char *buf, int width)
{
    int w = strlen(buf);
    if (w < width) {
        memset(buf+w, ' ', width-w);
        buf[width] = '\0';
    }
}

/* state_name -- printable names for each process state */
static const char *state_name[] = {
    "[DEAD]   ",
    "[ACTIVE] ",
    "[SEND]   ",
    "[RECEIVE]",
    "[SENDREC]",
    "[IDLE]   "
};

/* microbian_dump -- display process states */
static void microbian_dump(void)
{
    char buf[16];

    kprintf_setup();
    kprintf_internal("\r\nPROCESS DUMP\r\n");

    /* Our version of printf is a bit feeble, so the following is
       more painful than it should be. */

    for (int pid = 0; pid < os_nprocs; pid++) {
        proc p = os_ptable[pid];

        /* Find free space on process stack */
        unsigned *z = (unsigned *) p->stack;
        while (*z == BLANK) z++;
        unsigned free = (char *) z - (char *) p->stack;

        sprintf(buf, "%u/%u", p->stksize-free, p->stksize);
        pad(buf, 9);
        kprintf_internal("%s%d: %s %x stk=%s %s\r\n",
                         (pid < 10 ? " " : ""), pid,
                         state_name[p->state], (unsigned) p->stack,
                         buf, p->name);
    }
}


/* PROCESS QUEUES */

/* os_readyq -- one queue for each priority */
typedef struct _queue *queue;

static struct _queue {
    proc head, tail;
} os_readyq[NPRIO];

/* make_ready -- add process to end of the ready queue for its priority */
static inline void make_ready(proc p)
{
    int prio = p->priority;
    if (prio == P_IDLE) return;

    p->state = ACTIVE;
    p->next = NULL;

    queue q = &os_readyq[prio];
    if (q->head == NULL)
        q->head = p;
    else
        q->tail->next = p;
    q->tail = p;
}

/* choose_proc -- the current process is blocked: pick a new one */
static inline void choose_proc(void)
{
    for (int p = 0; p < NPRIO; p++) {
        queue q = &os_readyq[p];
        if (q->head != NULL) {
            os_current = q->head;
            q->head = os_current->next;
            DEBUG_SCHED(os_current->pid);
            return;
        }
    }
    
    os_current = idle_proc;
    DEBUG_SCHED(0);
}


/* SEND AND RECEIVE */

/* These versions of send and receive are invoked indirectly from user
processes via the system calls send() and receive(). */

/* accept -- test if a process is waiting for a message of given type */
static inline int accept(proc pdest, int type)
{
    return (pdest->state == RECEIVING
            && (pdest->filter == ANY || pdest->filter == type));
}

/* deliver -- copy a message and make the destination ready */
static inline void deliver(proc pdest, proc psrc)
{
    if (pdest->msgbuf) {
        *(pdest->msgbuf) = *(psrc->msgbuf);
        pdest->msgbuf->sender = psrc->pid;
    }
    make_ready(pdest);
}

/* queue_send -- add current process to a receiver's queue */
static inline void queue_send(proc pdest)
{
    os_current->next = NULL;
    if (pdest->waiting == NULL)
        pdest->waiting = os_current;
    else {
        proc r = pdest->waiting;
        while (r->next != NULL)
            r = r->next;
        r->next = os_current;
    }
}

/* find_sender -- search process queue for acceptable sender */
static proc find_sender(proc pdst, int type)
{
    proc psrc, prev = NULL;
        
    for (psrc = pdst->waiting; psrc != NULL; psrc = psrc->next) {
        if (type == ANY || psrc->msgbuf->type == type) {
            if (prev == NULL)
                pdst->waiting = psrc->next;
            else
                prev->next = psrc->next;

            return psrc;
        }

        prev = psrc;
    }

    return NULL;
}

/* await_reply -- wait for reply after sendrec */
static void await_reply(proc pdst)
{
    proc psrc = find_sender(pdst, REPLY);
    if (psrc != NULL) {
        /* Unlikely but not impossible: a REPLY message is already waiting.
           It can't come from the receiver of the original request. */
        deliver(pdst, psrc);
        make_ready(psrc);
    } else {
        pdst->state = RECEIVING;
        pdst->filter = REPLY;
    }
}

static inline proc find_dest(int dest)
{
    proc pdest;

    if (dest < 0 || dest >= os_nprocs)
        panic("Sending to a non-existent process %d", dest);

    pdest = os_ptable[dest];

    if (pdest->state == DEAD)
        panic("Sending to a dead process %s", pdest->name);

    return pdest;
}

/* mini_send -- send a message */
static void mini_send(int dest, message *msg)
{
    proc pdest = find_dest(dest);

    os_current->msgbuf = msg;

    if (accept(pdest, msg->type)) {
        /* Receiver is waiting: deliver the message and run receiver */
        deliver(pdest, os_current);
        make_ready(os_current);
    } else {
        /* Sender must wait by joining the receiver's queue */
        os_current->state = SENDING;
        queue_send(pdest);
    }

    choose_proc();
}

/* mini_receive -- receive a message */
static void mini_receive(int type, message *msg)
{
    /* First see if an interrupt is pending */
    if (os_current->pending && (type == ANY || type == INTERRUPT)) {
        os_current->pending = 0;
        if (msg) {
            msg->sender = HARDWARE;
            msg->type = INTERRUPT;
        }
        return;
    }

    os_current->msgbuf = msg;

    /* Now see if a sender is waiting */
    if (type != INTERRUPT) {
        proc psrc = find_sender(os_current, type);

        if (psrc != NULL) {
            deliver(os_current, psrc);

            switch (psrc->state) {
            case SENDING:
                make_ready(psrc);
                break;

            case SENDREC:
                await_reply(psrc);
                break;

            default:
                panic("Bad state in receive()");
            }

            choose_proc();
            return;
        }
    }

    /* No luck: we must wait. */
    os_current->state = RECEIVING;
    os_current->filter = type;
    choose_proc();
}    

/* mini_sendrec -- send a message and wait for reply */
static void mini_sendrec(int dest, message *msg)
{
    proc pdest = find_dest(dest);

    if (msg->type == REPLY)
        panic("sendrec may not be used to send REPLY message");

    os_current->msgbuf = msg;

    if (accept(pdest, msg->type)) {
        /* Send the message and wait for a reply */
        deliver(pdest, os_current);
        await_reply(os_current);
    } else {
        /* Join receiver's queue */
        os_current->state = SENDREC;
        queue_send(pdest);
    }

    choose_proc();
}


/* INTERRUPT HANDLING */

/* Interrupts send an INTERRUPT message (from HARDWARE) to a
registered handler process.  The default beheviour is to disable the
relevant IRQ in the interrupt handler, so that it can be re-enabled in
the handler once it has reacted to the interrupt.  We only deal with
the genuine interrupts >= 0, not the 16 exceptions that are < 0 this way. */

/* os_handler -- pid of handler process for each interrupt */
static int os_handler[N_INTERRUPTS];

/* connect -- connect the current process to an IRQ */
void connect(int irq)
{
    if (irq < 0) panic("Can't connect to CPU exceptions");
    os_current->priority = P_HANDLER;
    os_handler[irq] = os_current->pid;
}

/* priority -- set process priority */
void priority(int p)
{
    if (p < 0 || p > P_LOW) panic("Bad priority %d\n", p);
    os_current->priority = p;
}

/* interrupt -- send interrupt message */
void interrupt(int dest)
{
    proc pdest = os_ptable[dest];
    message *msg = pdest->msgbuf;

    if (accept(pdest, INTERRUPT)) {
        /* Receiver is waiting for an interrupt */
        if (msg) {
            msg->sender = HARDWARE;
            msg->type = INTERRUPT;
        }
        make_ready(pdest);
        if (os_current->priority > P_HANDLER) {
            /* Preempt lower-priority process */
            reschedule();
        }
    } else {
        /* Let's hope it's not urgent! */
        pdest->pending = 1;
    }
}

/* All interrupts are handled by this common handler, which disables
the interrupt temporarily, then sends or queues a message to the
registered handler task.  Normally the handler task will deal with the
cause of the interrupt, then re-enable it. */

/* default_handler -- handler for most interrupts */
void default_handler(void)
{
    int irq = active_irq(), task;
    if (irq < 0 || (task = os_handler[irq]) == 0)
        panic("Unexpected interrupt %d", irq);
    disable_irq(irq);
    interrupt(task);
}

static const char *exc_name[] = {
    "*zero*", "Reset", "NMI", "HardFault",
    "MemManage", "BusFault", "UsageFault"
};

/* hardfault_handler -- substitutes for the definition in startup.c */
void hardfault_handler(void)
{
    /* On nRF52, other exceptions come here too */
    int fault = active_irq() + 16;
    panic("Unexpected fault %s", exc_name[fault]);
}


/* INITIALISATION */

/* create_proc -- allocate and initialise process descriptor */
static proc create_proc(char *name, unsigned stksize)
{
    int pid;
    proc p;
    unsigned char *stack;
    unsigned *sp;

    if (os_nprocs >= NPROCS)
        panic("Too many processes");

    /* Allocate descriptor and stack space */
    pid = os_nprocs++;
    p = os_ptable[pid] = new_proc();
    stack = sbrk(stksize);
    sp = (unsigned *) &stack[stksize];

    /* Blank out the stack space to help detect overflow */
    for (unsigned *p = (unsigned *) stack; p < sp; p++) *p = BLANK;

    /* Fill in fields of the descriptor */
    p->pid = pid;
    strncpy(p->name, name, 15);
    p->name[15] = '\0';
    p->sp = sp;
    p->stack = stack;
    p->stksize = stksize;
    p->state = ACTIVE;
    p->priority = P_LOW;
    p->waiting = 0;
    p->pending = 0;
    p->filter = ANY;
    p->msgbuf = NULL;
    p->next = NULL;

    return p;
}

#define MAGIC 0xfffffffd        /* Magic value for exception return */
#define INIT_PSR 0x01000000     /* Thumb bit is set */

/* These match the frame layout in mpx.s, and the hardware */
#define ERV_SAVE 0 /* Offset for magic return value */
#define R0_SAVE 9
#define R1_SAVE 10
#define R2_SAVE 11
#define LR_SAVE 14
#define PC_SAVE 15
#define PSR_SAVE 16
#define FRAME_WORDS 17

#define roundup(x, n) (((x) + ((n)-1)) & ~((n)-1))

/* start -- initialise process to run later */
int start(char *name, void (*body)(int), int arg, int stksize)
{
    proc p = create_proc(name, roundup(stksize, 8));

    if (os_current != NULL)
        panic("start() called after scheduler startup");

    /* Fake an exception frame */
    unsigned *sp = p->sp - FRAME_WORDS;
    memset(sp, 0, 4*FRAME_WORDS);
    sp[PSR_SAVE] = INIT_PSR;
    sp[PC_SAVE] = (unsigned) body & ~0x1; /* Activate the process body */
    sp[LR_SAVE] = (unsigned) exit; /* Make it return to exit() */
    sp[R0_SAVE] = (unsigned) arg;  /* Pass the supplied argument in R0 */
    sp[ERV_SAVE] = MAGIC;
    p->sp = sp;

    make_ready(p);
    return p->pid;
}

/* set_stack -- enter thread mode with specified stack (see mpx.s) */
void set_stack(unsigned *sp);

/* init -- main program, creates application processes */
void init(void);

#define IDLE_STACK 128

/* idle_task -- body of the idle process */
void idle_task(void)
{
    /* Idle only runs again when there's nothing to do. */
    while (1) pause();
}

/* __start -- start the operating system */
void __start(void)
{
    /* Create idle task as process 0 */
    idle_proc = create_proc("IDLE", IDLE_STACK);
    idle_proc->state = IDLING;
    idle_proc->priority = P_IDLE;

    /* Call the application's setup function */
    init();

    /* The main program morphs into the idle process. */
    os_current = idle_proc;
    DEBUG_SCHED(0);
    set_stack(os_current->sp);
    yield();

    idle_task();
}


/* SYSTEM CALL INTERFACE */

/* System call numbers */
#define SYS_YIELD 0
#define SYS_SEND 1
#define SYS_RECEIVE 2
#define SYS_SENDREC 3
#define SYS_EXIT 4
#define SYS_DUMP 5

/* System calls retrieve their arguments from the exception frame that
was saved by the SVC instruction on entry to the operating system.  We
can't rely on the arguments still being in r0, r1, etc., because an
interrupt may have intervened and trashed these registers. */

#define arg(i, t) ((t) psp[R0_SAVE+(i)])

/* system_call -- entry from system call traps */
unsigned *system_call(unsigned *psp)
{
    short *pc = (short *) psp[PC_SAVE]; /* Program counter */
    int op = pc[-1] & 0xff;      /* Syscall number from SVC instruction */

    /* Save sp of the current process */
    os_current->sp = psp;

    /* Check for stack overflow */
    if (* (unsigned *) os_current->stack != BLANK)
        panic("Stack overflow");

    switch (op) {
    case SYS_YIELD:
        make_ready(os_current);
        choose_proc();
        break;

    case SYS_SEND:
        mini_send(arg(0, int), arg(1, message *));
        break;

    case SYS_RECEIVE:
        mini_receive(arg(0, int), arg(1, message *));
        break;

    case SYS_SENDREC:
        mini_sendrec(arg(0, int), arg(1, message *));
        break;

    case SYS_EXIT:
        os_current->state = DEAD;
        choose_proc();
        break;

    case SYS_DUMP:
        /* Invoking microbian_dump as a system call means that its own
           stack space is taken from the system stack rather than the
           stack of the current process. */
        microbian_dump();
        break;

    default:
        panic("Unknown syscall %d", op);
    }

    /* Return sp for next process to run */
    return os_current->sp;
}

/* cxt_switch -- context switch following interrupt */
unsigned *cxt_switch(unsigned *psp)
{
    os_current->sp = psp;
    make_ready(os_current);
    choose_proc();
    return os_current->sp;
}


/* SYSTEM CALL STUBS */

/* Assembly-language stubs for each system call are in mpx-???.s: they
depend on finding their arguments in the correct registers, and not
optimised away as van happen with recent versions of GCC.  Below are a
few convenience routines for sending messages with no attached data,
or with a single integer or pointer. */

void send_msg(int dest, int type)
{
    message m;
    m.type = type;
    send(dest, &m);
}

void send_int(int dest, int type, int val)
{
    message m;
    m.type = type;
    m.int1 = val;
    send(dest, &m);
}

void send_ptr(int dest, int type, void *ptr)
{
    message m;
    m.type = type;
    m.ptr1 = ptr;
    send(dest, &m);
}


/* DEBUG PRINTING */

/* The routines here work by reconfiguring the UART, disabling
interrupts and polling: they should be used only for debugging. */

#define TX USB_TX
#define RX USB_RX

/* kprintf_setup -- set up UART connection to host */
static void kprintf_setup(void)
{
    /* Delay so any UART activity can cease */
    delay_loop(2000);

    /* Set up pins to maintain signal levels while UART disabled */
    gpio_dir(USB_RX, GPIO_DIR_Input);
    gpio_dir(USB_TX, GPIO_DIR_Output);
    gpio_out(USB_TX, 1);

    /* Reconfigure the UART just to be sure */
    UART_ENABLE = UART_ENABLE_Disabled;
    UART_BAUDRATE = UART_BAUDRATE_9600; /* 9600 baud */
    UART_CONFIG = FIELD(UART_CONFIG_PARITY, UART_PARITY_None);
                                        /* format 8N1 */
    UART_PSELTXD = TX;                  /* choose pins */
    UART_PSELRXD = RX;
    UART_ENABLE = UART_ENABLE_Enabled;
    UART_TXDRDY = 0;
    UART_STARTTX = 1;
}

/* kputc -- send output character */
static void kputc(char ch)
{
    UART_TXD = ch;
    while (! UART_TXDRDY) { }
    UART_TXDRDY = 0;
}


/* kprintf_internal -- internal version of kprintf */
static void kprintf_internal(char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    do_print(kputc, fmt, va);
    va_end(va);
}

/* kprintf -- printf variant for debugging (disables interrupts) */
void kprintf(char *fmt, ...)
{
    va_list va;
    unsigned prev = get_primask();
    intr_disable();
    kprintf_setup();

    va_start(va, fmt);
    do_print(kputc, fmt, va);
    va_end(va);

    set_primask(prev);
    /* Caller gets a UART interrupt if enabled. */
}

/* panic -- the unusual has happened.  Did you think it impossible? */
void panic(char *fmt, ...)
{
    va_list va;
    intr_disable();
    kprintf_setup();     

    kprintf_internal("\r\nPanic: ");
    va_start(va, fmt);
    do_print(kputc, fmt, va);
    va_end(va);
    if (os_current != NULL)
         kprintf_internal(" in process %s", os_current->name);
    kprintf_internal("\r\n");

    spin();
}

/* badmesg -- default case for switches on message type */
void badmesg(int type)
{
    panic("Bad message type %d", type);
}
