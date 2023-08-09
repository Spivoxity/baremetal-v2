# houdini.py
# Copyright (c) 2021 J. M. Spivey

# Manipulate gnome-style configuration files.  Similar to crudini, but
# obeying the different conventions about spaces that are used in
# Gnome.  Python has a config file parsing library used by crudini,
# but it just won't do for this task because of minor syntax
# variations.
#
# Invoking
#
#     houdini set file.conf section option value
#
# sets an option in file.conf, creating it if necessary.  All comments
# in an existing file are lost, apart from a preamble that is preserved.
#
# Invoking
#
#     houdini get file.conf section option
#
# prints an option setting to standard output, or prints nothing if
# the file doesn't exist or the option is not set.
#
# Invoking
#
#     houdini del file.conf section option
#
# deletes an option.  Deleting a non-existent option is allowed.
#
# Key-value pairs before the first named section are treated as
# belonging to a section named with the empty string.

import sys
import re

# A global database of config file entries.  If sec exists, then dbase[sec]
# is itself a dictionary d with d[key] = value.
dbase = dict()
# The usual dict objects preserve the order keys were added, which is
# helpful for us.

# Preamble before first section
preamble = []

def setopt(section, key, value):
    """Set an option, creating the section if needed."""
    global dbase
    if not (section in dbase): dbase[section] = dict()
    dbase[section][key] = value

def delopt(section, key):
    """Delete an option."""
    global dbase
    if section in dbase and key in dbase[section]:
        del dbase[section][key]

def rematch(re, line):
    """Match a regexp and save the result."""
    global mdata
    mdata = re.match(line)
    return mdata

def regroup(n):
    """Retrieve group from previous match."""
    global mdata
    return mdata.group(n)

def parse(fp):
    """Parse a config file and add its content to the database."""
    global preamble

    # These are rather strict, but adequate for the task
    sechdr = re.compile(r"^\[([^]]+)\]$")
    kvline = re.compile(r"^([^=]+)=(.*)$")
    comment = re.compile(r"^[ \t]*(#.*)?$")
    
    cursec = ""
    started = False

    for line in fp:
        if rematch(sechdr, line):
            # A [section] line.
            started = True
            cursec = regroup(1)
        elif rematch(kvline, line):
            # A key=value line.  No spaces around key, but
            # spaces ARE significant before and after value
            started = True
            key, value = regroup(1), regroup(2)
            setopt(cursec, key, value)
        elif rematch(comment, line):
            # A blank or comment line.  Preserve comments at top of file.
            if not started: preamble.append(line)
        else:
            print("Syntax error:", line, file=sys.stderr)
            exit(1)

def save(fp):
    """Save the database on a file."""
    global dbase, preamble
    started = False
    for line in preamble: fp.write(line)

    if "" in dbase and dbase[""]: 
        started = True
        for k, v in dbase[""].items(): fp.write(f"{k}={v}\n")

    for s, d in dbase.items():
        if s == "" or not d: continue
        if started: fp.write("\n")
        started = True
        fp.write(f"[{s}]\n")
        for k, v in d.items(): fp.write(f"{k}={v}\n")

args = sys.argv[1:]

if not ((len(args) == 4 and args[0] == "get")
        or (len(args) == 5 and args[0] == "set")
        or (len(args) == 4 and args[0] == "del")):
    print("Usage: houdini (get|set) file.conf section option [value]",
          file=sys.stderr); exit(1)

cmd, fname, section, option = args[0], args[1], args[2], args[3]
if args[0] == "set": value = args[4]

try:
    with open(fname) as fp: parse(fp)
except FileNotFoundError:
    # Non-existent file is treated as empty
    pass

if cmd == "get":
    # houdini get file.conf section option
    if (section in dbase) and (option in dbase[section]):
        print(dbase[section][option])

if cmd == "set":
    # houdini set file.conf section option value
    setopt(section, option, value)
    with open(fname, 'w') as fp: save(fp)

if cmd == "del":
    # houdine del file.conf section option
    delopt(section, option)
    with open(fname, 'w') as fp: save(fp)
