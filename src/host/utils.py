import curses
import shlex
from os import listdir

class Command:
    def __init__(self, name, help, args = {}):
        self.name = name
        self.help = help
        self.args = args

def split_input(input_command):
    args = shlex.split(input_command)
    decomposed = {'cmd': args[0]}
    prev_arg = ''
    for i, v in enumerate(args[1:]):
        # If the index of the current thing being parsed is even it indicates it's the argument name, 
        # odd therefore indicates it's the argument value
        if i%2 == 0:
            prev_arg = v.strip('-')
        elif i%2 == 1:
            decomposed[prev_arg] = v

    # Should only occur for help commands where there is no value for the argument
    if prev_arg not in decomposed:
        if decomposed['cmd'] == "help" or decomposed['cmd'] == "run":
            decomposed['fun'] = prev_arg

    return decomposed

def increment_filename(name):
    # list current directory then count name matches
    count = 0
    files = listdir()

    name, extension = name.split('.')

    for filename in files:
        if name in filename:
            count = count + 1

    return name + ("" if count == 0 else "_%s" % str(count)) + ".%s" % extension