import curses

class Command:
    def __init__(self, name, help, args = ["No arguments"]):
        self.name = name
        self.help = help
        self.args = args

def split_input(input_command):
    cmd = input_command.split(' ')
    decomposed = {'cmd': cmd[0]}
    
    prev_arg = ''
    for i, v in enumerate(cmd[1:]):
        # If the index of the current thing being parsed is even it indicates it's the argument name, 
        # odd therefore indicates it's the argument value
        if i%2 == 0:
            prev_arg = v.strip('-')
        elif i%2 == 1:
            decomposed[prev_arg] = v

    # Should only occur for help commands where there is no value for the argument
    if prev_arg not in decomposed:
        if decomposed['cmd'] == "help":
            decomposed['fun'] = prev_arg

    return decomposed
