import sys,os
import curses
import time

filename = sys.argv[1]

print("----------------------------------------------------------------------------")
print("                               Thermocouple Readings                        ")
print("----------------------------------------------------------------------------")
print("")
count_no_data = 0
count_threshold = 50

while True:
    in_data = []
    with open(filename, 'r+') as f:
        lines = f.readlines()
        os.remove(filename)
        if len(lines) > 0:
            in_raw = lines[-1].strip('\n')

            if "disconnected" in in_raw:
                # If the device disconnects, so should we
                exit(0)

            in_data = in_raw.split(',')

        else:
            if count_no_data > count_threshold:
                print("No data recieved in 10 seconds")
                exit(1)
            else:
                time.sleep(0.2)
                count_no_data += 1

    if 'CJT' in in_data and 'TH' in in_data:
        to_print = "Cold Junction Temp (celsius): %4.3f; Thermocouple Temp (Kelvin): %4.3f" % (float(in_data[1]), float(in_data[3]))
        print(to_print)