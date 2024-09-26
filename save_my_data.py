import serial
import time

def save_serial_data(port, baudrate, filename, duration):
    ser = serial.Serial(port, baudrate)
    start_time = time.time()
    with open(filename, 'w') as file:
        while time.time() - start_time < duration:
            if ser.in_waiting:
                data = ser.readline().decode('utf-8').strip()
                file.write(data + '\n')
    ser.close()

if __name__ == "__main__":
    port = '/dev/ttyUSB0'  # Change this to your serial port
    baudrate = 250000  # Change this to your baud rate
    filename = 'serial_data.txt'
    duration = 10  # Duration to capture data in seconds

    save_serial_data(port, baudrate, filename, duration)
    print("Done!")