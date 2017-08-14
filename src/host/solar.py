import serial
import curses
import sys
import os
from curses.ascii import NL as newline
from curses.ascii import BS as backspace
from serial_utils import solar_serial_port, DisconnectedError
from utils import Command, split_input

# Initialising the curses session and overriding print
screen = curses.initscr()
screen.keypad(True)

def print(string):
    screen.addstr(string)

def println(string = ""):
    screen.addstr(string + "\n")

println()
println('-------------------------------------------------')
println('ENGR489 Solar Simulator Controller - Jamie Sanson')
println('-------------------------------------------------')
println()

commands = [Command("help", "Run as `help <command>` for more information about that command"),\
            Command("test", "Tests the solar simulator"),\
            Command("run", "Run as `run -<arg> <arg_value> to run measurements", ["`targets`: Expects comma-separated numbers",\
                                                                                  "`threshold`: Temperature target threshold for taking measurements, defaults to 1.5 degrees Celsius"]),\
            Command("stop", "Stops operation of controller"),\
            Command("exit", "Stops operation of controller and exits program"),\
            Command("log", "")]

solar_port = ""


# TODO: Uncomment for proper testing
#
## Check for simulator, if not here then exit at this point
#try:
#    solar_port = solar_serial_port()
#except (EnvironmentError, DisconnectedError) as e:
#    print(e)
#    exit()
#
#sim_ser = serial.Serial(solar_port, \
#                        baudrate = 115200,\
#                        write_timeout = 1, \
#                        timeout = 1)



println("Solar cell found on port %s. Connected." % solar_port)
println()
println("Available commands:")
for command in commands:
    println(command.name + " - " + command.help)

println()

def exit():
    curses.endwin()
    os.system('cls' if os.name == 'nt' else 'clear')
    sys.exit(1)

def help(name):
    for command in commands:
        if command.name == name:
            println(command.help)
            return
    
        if len(name) == 0 and command.name == "help":
            println(command.help)
            return
        
    println("Unknown function: %s" % name)

def run(args):
    targets = ""
    thresh = ""

    if 'targets' in args:
        targets = args['targets']
    else:
        println("Missing temperature targets argument, try run -targets 5,10,30 -threshold 2")
        return

    if 'threshold' in args:
        thresh = args['threshold']
    else:
        println("Missing temperature threshold argument, try run -targets 5,10,30 -threshold 2")
        return

    println("Begining simulator controller for targets: %s and temperature threshold: %s" % (targets, thresh))

# This function does the heavy lifting, getting the parsed input and invoking the appropriate functions
def run_command(command):
    c = split_input(command)
    name = c['cmd']

    if name == "exit":
        exit()
    elif name == "help":
        help(c['fun'])
    elif name == "run":
        run(c)
    else:
        println("Unknown command: %s, try the `help` command" % name)

print(">>> ")

# --------------------------------  
# Begin terminal interface
# --------------------------------  
in_cmd = ""

# For command history
prev_commands = []
curr_command_index = -1

while True:
    in_char = screen.getch()

    if in_char == newline:
        # If the user enters a new line, the command should be run. This command running is done in a blocking manner
        run_command(in_cmd)
        prev_commands.append(in_cmd)
        in_cmd = ""
        curr_command_index = -1
        print(">>> ")
    elif in_char == backspace:
        # Manually implementing backspace. 
        # Remove a character from command if backspace, during command, 
        # or move character back to where it was if command empty
        if len(in_cmd) > 0:
            in_cmd = in_cmd[0:len(in_cmd)-1]
            screen.delch()
            screen.refresh()
        else:
            (y,x) = screen.getyx()
            screen.move(y, x+1)
    elif in_char == curses.KEY_UP:
        if len(prev_commands) > 0 and curr_command_index + 1 < len(prev_commands):
            # Get current yx for rewriting screen
            (y,_) = screen.getyx()
            curr_command_index += 1
            ind = len(prev_commands) - (curr_command_index+1)
            screen.deleteln()
            screen.addstr(y,0, ">>> " + prev_commands[ind])
            in_cmd = prev_commands[ind]
    elif in_char == curses.KEY_DOWN:
        if len(prev_commands) > 0 and curr_command_index > -1:
            # Get current yx for rewriting screen
            (y,_) = screen.getyx()
            curr_command_index -= 1
            ind = len(prev_commands) - (curr_command_index+1)
            if curr_command_index == -1:
                screen.deleteln()
                screen.addstr(y,0, ">>> ")
                in_cmd = ""
            else:
                screen.deleteln()
                screen.addstr(y,0, ">>> " + prev_commands[ind])
                in_cmd = prev_commands[ind]
    else:
        in_cmd = in_cmd + str(chr(in_char))

    
        