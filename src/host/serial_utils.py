import sys
import glob
import serial

class DisconnectedError(Exception):
    pass

def serial_ports():
    """ Lists serial port names

        :raises EnvironmentError:
            On unsupported or unknown platforms
        :turns:
            A list of the serial ports available on the system
    """
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this excludes your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result


def solar_serial_port():
    """ Finds connected solar simulator device

        :raises DisconnectedError:
            When device is not connected
        :turns:
            The solar simulator port
    """
    ports = serial_ports()

    for port in ports:
        try:
            s = serial.Serial(port, \
                                baudrate = 115200,\
                                write_timeout = 1, \
                                timeout = 1)

            s.write("sst36vuw".encode())
            response = s.readline().strip("b'").strip("\\r\\n")

            if response == "active":
                return port

        except:
            pass

    raise DisconnectedError("Simulator not found, please check connections and try again")


