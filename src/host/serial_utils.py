import sys
import glob
import serial
from threading import Thread, Event
import time
from queue import Queue

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
            with serial.Serial(port = port, \
                                baudrate = 115200,\
                                write_timeout = 1, \
                                timeout = 1.5) as s:

                s.write(b'sst36vuw')
                response = str(s.readline()).strip("b'").strip("\\r\\n")

                if response == "active":
                    s.close()
                    return port

        except Exception as e:
            print(e)
            pass

    raise DisconnectedError("Simulator not found, please check connections and try again")


"""
    Class for managing serial interactions. Should be accessed on main thread
"""            
class serial_manager():
    def __init__(self):
        self.out_q = Queue()
        self.in_q = Queue()
        self.ser = None
        self.st = None
        self.stop = Event()

    def __enter__(self):
        self.in_q = Queue()
        self.out_q = Queue()
        if not self.ser:
            self.ser = self.__connect(None, lambda err: print(err))

        if self.ser:
            self.st = Thread(name = "Serial-Thread", \
                            daemon = True, \
                            target = self.ser_thread, \
                            args = (self.ser, self.in_q, self.out_q, self.stop))
            self.st.start()

        return self

    def __exit__(self, exc_type, exc_value, traceback):
        # Nothing to do here. We want to maintain queue status
        self.stop.set()
        self.st.join()

        self.stop = Event()
        
    def __connect(self, ser, on_err):
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
                                timeout = 2)

        return ser

    def ser_thread(self, ser, in_q, out_q, stop):
        while not stop.is_set():
            if not out_q.empty():
                ser.write(out_q.get())

            in_q.put(str(ser.readline()).strip("b'").strip("\\r\\n"))

    def output(self, string):
        if not self.st.is_alive():
            __enter__()

        self.out_q.put(string.encode())

    def read_in(self):
        if not self.st.is_alive():
            __enter__()

        return self.in_q.get()