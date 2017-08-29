import serial
import curses
import sys
import os
import time
from curses.ascii import NL as newline
from curses.ascii import BS as backspace
from serial_manager import serial_manager
from serial_utils import DisconnectedError
from utils import Command, split_input, increment_filename

# Initialising the curses session and overriding print
screen = curses.initscr()
screen.keypad(True)

def printc(string):
    screen.addstr(string)

def println(string = ""):
    screen.addstr(string + "\n")
    
def printArgs(args):
    if len(args) > 0:
        println("\n\tArguments:")

    for arg_name, arg_desc in args.items():
        if len(arg_name) > 0:
            println("\t%s - %s" % (arg_name, arg_desc))

# -------------------------------------------------
# Region command functions
# -------------------------------------------------
def exit():
    curses.endwin()
    os.system('cls' if os.name == 'nt' else 'clear')
    sys.exit(1)

def test():
    global serial_manager
    with serial_manager as sm:
        sm.output("test")
        in_str = sm.read_in()
        while in_str != "done":
            println(in_str)     
            screen.refresh()
            in_str = sm.read_in()   

def help(name):
    for command in commands:
        if command.name == name:
            println(command.help)
            printArgs(command.args)
            return
    
        if len(name) == 0 and command.name == "help":
            println(command.help)
            return
        
    println("Unknown function: %s" % name)

def run(args):
    global serial_manager
    with serial_manager as sm:
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
        sm.output("temp:%s\n" % targets)
        sm.output("t_thresh:%s\n" % thresh)

   
    start_thread(filename = "run.csv", run_cmd = "run")

def stop():
    global serial_manager
    if (serial_manager.worker_alive()):
        println("Stopping worker thread")
        serial_manager.stop_worker()
    
def run_sweep():
    start_thread(filename = "sweep.csv", run_cmd = "runsweep")

# Function for starting the worker thread in a specific state
def start_thread(filename, run_cmd):
    global serial_manager
    run_filename = increment_filename(filename)

    println("Staring worker thread, outputing to " + run_filename)
    try:
        serial_manager.start_worker(run_filename)
    except DisconnectedError as e:
        println(str(e))
        return

    serial_manager.output(run_cmd)
    println("Thread started")

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
    elif name == "stop":
        stop()
    elif name == "test":
        test()
    elif name == "runsweep":
        run_sweep()
    else:
        println("Unknown command: %s, try the `help` command" % name)


# -------------------------------------------------
# Region simulator control code
# -------------------------------------------------
println()
println('-------------------------------------------------')
println('ENGR489 Solar Simulator Controller - Jamie Sanson')
println('-------------------------------------------------')
println()

# Defining commands available to the user. Format: Command(name, help text, dict of arguments)
commands = [Command("help", "Run as `help <command>` for more information about that command"),\
            Command("test", "Tests the solar simulator"),\
            Command("run", "Run as `run -<arg> <arg_value> to run measurements", {"targets": "Expects comma-separated numbers",\
                                                                                  "threshold" : "Temperature target threshold for taking measurements, defaults to 1.5 degrees Celsius"}),\
            Command("stop", "Stops operation of controller"),\
            Command("exit", "Stops operation of controller and exits program"),\
            Command("log",  "Shows the past transmission"),\
            Command("runsweep", "Runs a single sweep of measurements. Useful for testing")]

solar_port = ""

ser = None
serial_manager = serial_manager()
        
println()
println("Available commands:")
for command in commands:
    println(command.name + " - " + command.help)
   
println()

printc(">>> ")

# -------------------------------------
# Begin terminal interface
# -------------------------------------
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
        printc(">>> ")
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

    
        