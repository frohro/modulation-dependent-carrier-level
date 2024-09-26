import serial
import matplotlib.pyplot as plt

# Function to open the serial port
def open_serial_port():
    ports = ['/dev/ttyUSB0', '/dev/ttyUSB1']
    for port in ports:
        try:
            ser = serial.Serial(port, 9600)
            print(f"Connected to {port}")
            return ser
        except serial.SerialException:
            print(f"Failed to connect to {port}")
    raise serial.SerialException("Could not open any serial port")

# Open the serial port
ser = open_serial_port()

# Read the first line to get the labels
labels_line = ser.readline().decode('utf-8').strip()
labels = labels_line.split(',')

# Ask the user for the number of data points to read
# N = int(input("Enter the number of data points to read: "))
N = 250

# Initialize lists to store the data
data_counter = []
received_word1 = []
received_word2 = []
received_word3 = []

# Read up to N lines of data
for _ in range(N):
    line = ser.readline().decode('utf-8').strip()
    if line:
        try:
            values = list(map(int, line.split(',')))
            data_counter.append(values[0])
            received_word1.append(values[1])
            if len(values) > 2:
                received_word2.append(values[2])
            if len(values) > 3:
                received_word3.append(values[3])
        except ValueError:
            print(f"Invalid line: {line}")

# Close the serial port
ser.close()

# Plot the data
plt.figure(figsize=(10, 5))
plt.plot(data_counter, received_word1, marker='o', linestyle='-', label=labels[1])
if received_word2:
    plt.plot(data_counter, received_word2, marker='o', linestyle='-', label=labels[2])
if received_word3:
    plt.plot(data_counter, received_word3, marker='o', linestyle='-', label=labels[3])
plt.xlabel('Data Counter')
plt.ylabel('Received Values')
plt.title('The uint16_t Values You Wanted to Plot')
plt.legend()
plt.grid(True)
plt.show()