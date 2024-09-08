import pandas as pd
import matplotlib.pyplot as plt

def plot_serial_data(filename):
    # Initialize a dictionary to store the data
    data_dict = {}
    data_dict['time'] = []

    # Read the data from the file
    with open(filename, 'r') as file:
        for i, line in enumerate(file):
            values = line.strip().split(' ')
            if len(values) % 2 != 0:
                print(f"Skipping malformed line {i+1}: {line.strip()}")
                continue
            for j in range(0, len(values), 2):
                try:
                    label = values[j]
                    number = float(values[j+1])
                    if label not in data_dict:
                        data_dict[label] = []
                    data_dict[label].append(number)
                except (IndexError, ValueError) as e:
                    print(f"Error processing line {i+1}: {line.strip()} - {e}")
                    continue
            data_dict['time'].append(i * 10e-6)  # 10 microseconds interval

    # Convert the dictionary to a DataFrame
    data = pd.DataFrame(data_dict)

    # Plot the data
    plt.figure(figsize=(10, 6))
    for column in data.columns:
        if column != 'time':
            plt.plot(data['time'], data[column], label=column)

    plt.xlabel('Time (s)')
    plt.ylabel('Values')
    plt.title('Serial Data Plot')
    plt.legend()
    plt.grid(True)
    plt.show()

if __name__ == "__main__":
    filename = 'serial_data.txt'
    plot_serial_data(filename)