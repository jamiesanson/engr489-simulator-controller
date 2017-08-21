import sys
import glob
import serial
import threading

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
                s.close()
                return port

        except:
            pass

    raise DisconnectedError("Simulator not found, please check connections and try again")



def connect(ser, on_err):
    """ Trys to connect to serial instance

        :turns:
            If controller is connected
    """

    if not ser:
        # Check for simulator, if not here then exit at this point
        try:
            solar_port = solar_serial_port()
        except (EnvironmentError, DisconnectedError) as e:
            on_err(str(e))
            return None

        ser = serial.Serial(solar_port, \
                            baudrate = 115200,\
                            write_timeout = 1, \
                            timeout = 1)
                        
    try:
        ser.open()
        if ser.read():
            ser.close()
            return ser

        ser.close()
    except:
        on_err("Port was found but connection failed")
        return None

"""
    Class for handling serial threading
"""
class serial_thread(threading.Thread):
    def __init__(self, serial, name = "Serial"):
        threading.Thread.__init__(self)
        self.name = name
        self.serial = serial
    
    def run(self):
        while True:
            println("Reading serial")
            delay(1000)