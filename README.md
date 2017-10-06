# ENGR489: Solar Simulator
This repository contains code relating to the operation of a solar simulator, constructed for my ENGR489 project. Included in this is code to run a Teensy 3.6 Microprocessor board as a controller of the system, as well as a measurement device. This communicates with the host computer over serial, which is handles through the command line using Python for input handling, threading, and filesystem operations.
As well as this, MATLAB code is included to compute statistics regarding the simulator specifications in line with the ASTM E927 Solar Simulator Standard.

## Teensy 3.6 Code
Code to automate the collection of data from a temperature controlled solar simulator setup. This code is to be run on a Teensy 3.6, and is able to be remotely controlled through serial.

### Libraries used
* Adafruit MAX31855K Library (https://github.com/adafruit/Adafruit-MAX31855-library)
* X113647 Stepper Motor Library (https://github.com/tardate/X113647Stepper)

## Host Computer Code 
### Note, this is now handled by the Solar JavaFX project
Code to handle the communication of information between the host computer and the controller device. This code runs on the command line through the use of the `curses` package, and is able to be bundled into a single executable file through the use of `PyInstaller` on version of Python < 3.6.

A number of libraries are used, and are required dependencies to run the python script. These aren't needed when running the all-in-one executable. All libraries can be acquired using `pip install XXX`, with the exception being curses, which isn't officially supported on Windows due to the fact that the default command line interface `cmd` doesn't support redirection. Unofficial versions of this can be found and used in `powershell`.

### Libraries used
* PySerial
* Curses (Unofficially found here: http://www.lfd.uci.edu/~gohlke/pythonlibs/#curses)
* Shlex, threading, os, sys (built in modules)

## MATLAB Code
This code exists for classification of the solar simulator, given inputs of spatial uniformity. This is to be extended to handle calculation of temporal instability
and spectral match.
