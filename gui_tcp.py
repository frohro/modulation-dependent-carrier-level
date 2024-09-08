# To do: Make the messages sent back and forth to the Teensy via TCP/IP shorter.
# You can make a table here with each message described carefully and then a brief 
# few characters for the message.

import socket
import tkinter as tk
from tkinter import messagebox

# Define the server address and port
server_ip = '10.7.1.12'  # Change to your Teensy's IP address
server_port = 5100

def send_CPOD():
    try:
        # Create a TCP socket
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            # Connect to the server
            s.connect((server_ip, server_port))

            # Get the integer from the input field
            integer_to_send = entry.get()
            if integer_to_send.isdigit():
                print(f"Sending integer: {integer_to_send}")
                s.sendall(f"setsamplesPerDecay:{integer_to_send}\n".encode())  # Send integer followed by a newline

                # Receive the response from the Teensy
                response6 = s.recv(1024)
                messagebox.showinfo("Received", f"Received integer from Teensy: {response6.decode()}")
            else:
                messagebox.showwarning("Input Error", "Please enter a valid integer.")
    except Exception as e:
        messagebox.showerror("Connection Error", f"Could not connect to the server: {e}")

def get_CPOD():
    try:
        # Create a TCP socket
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            # Connect to the server
            s.connect((server_ip, server_port))

            # Request the current integer value
            s.sendall("getsamplesPerDecay\n".encode())  # Send request for current integer

            # Receive the response from the Teensy
            response5 = s.recv(1024)
            current_value = response5.decode()
            current_value_label.config(text=f"Current decay per sample: {current_value}")
    except Exception as e:
        messagebox.showerror("Connection Error", f"Could not connect to the server: {e}")

def set_MDCL():
    try:
        # Create a TCP socket
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            # Connect to the server
            s.connect((server_ip, server_port))

            # Get the selected option
            option_to_send = selected_option.get()

            # Map the selected option to corresponding values
            if option_to_send == "AMC":
                value_to_send = 1
            elif option_to_send == "DCC":
                value_to_send = 0

            print(f"Sending value: {value_to_send} for option: {option_to_send}")
            s.sendall(f"setMDCL:{value_to_send}\n".encode())  # Send mapped value followed by a newline

            # Receive the response from the Teensy
            response1 = s.recv(1024)
            messagebox.showinfo("Received", f"Received response from Teensy: {response1.decode()}")
    except Exception as e:
        messagebox.showerror("Connection Error", f"Could not connect to the server: {e}")

def get_MDCL():
    try:
        # Create a TCP socket
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            # Connect to the server
            s.connect((server_ip, server_port))

            # Request the current integer value
            s.sendall("getMDCL\n".encode())  # Send request for current integer

            # Receive the response from the Teensy
            response2 = s.recv(1024)
            response2 = response2.decode().strip()

            #current_value_mdcl = response.decode()
            if response2 == "1":
                current_value1 = "AMC"
            elif response2 == "0":
                current_value1 = "DCC"
            current_mdcl_label.config(text=f"Current MDCL Being Used: {current_value1}")
    except Exception as e:
        messagebox.showerror("Connection Error", f"Could not connect to the server: {e}")

def send_Adjust():
    try:
        # Create a TCP socket
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            # Connect to the server
            s.connect((server_ip, server_port))

            # Get the integer from the input field
            float_to_send = entry2.get()
            #if float_to_send.isdigit():
            print(f"setAdjust: {float_to_send}")
            s.sendall(f"setAdjust:{float_to_send}\n".encode())  # Send integer followed by a newline

            # Receive the response from the Teensy
            response4 = s.recv(1024)
            messagebox.showinfo("Received", f"Received integer from Teensy: {response4.decode()}")
            #else:
            #    messagebox.showwarning("Input Error", "Please enter a valid integer.")
    except Exception as e:
        messagebox.showerror("Connection Error", f"Could not connect to the server: {e}")

def get_Adjust():
    try:
        # Create a TCP socket
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            # Connect to the server
            s.connect((server_ip, server_port))

            # Request the current integer value
            s.sendall("getAdjust\n".encode())  # Send request for current integer

            # Receive the response from the Teensy
            response3 = s.recv(1024)
            current_value2 = response3.decode()
            current_value_label2.config(text=f"Current Adjust Value(0-1): {current_value2}")
    except Exception as e:
        messagebox.showerror("Connection Error", f"Could not connect to the server: {e}")

# Create the main window
root = tk.Tk()
root.title("AWR MDCL Board Interface")

# Set the window size (width x height)
root.geometry("1200x600")  # Width: 400 pixels, Height: 300 pixels

# Create and place the input field and buttons
label = tk.Label(root, text="Enter CPOD value to send:")
label.grid(row=0, column=0, padx=10, pady=10)

entry = tk.Entry(root)
entry.grid(row=1, column=0, padx=10, pady=10)

send_button = tk.Button(root, text="Send Value", command=send_CPOD)
send_button.grid(row=2, column=0, padx=10, pady=10)

get_button = tk.Button(root, text="Get CPOD Value", command=get_CPOD)
get_button.grid(row=3, column=0, padx=10, pady=10)

current_value_label = tk.Label(root, text="Current CPOD Value: 0")
current_value_label.grid(row=4, column=0, padx=10, pady=10)

# Define the options for the dropdown
options = ["AMC", "DCC"]
selected_option = tk.StringVar()
selected_option.set(options[0])  # Set the default selected option

# Create and place the dropdown and button
label1 = tk.Label(root, text="Select MDCL:")
label1.grid(row=0, column=1, padx=10, pady=10)

dropdown = tk.OptionMenu(root, selected_option, *options)
dropdown.grid(row=1, column=1, padx=10, pady=10)

send_button1 = tk.Button(root, text="Update MDCL Board", command=set_MDCL)
send_button1.grid(row=2, column=1, padx=10, pady=10)

current_mdcl_label = tk.Label(root, text="Current MDCL Being Used: AMC")
current_mdcl_label.grid(row=3, column=1, padx=10, pady=10)

get_button1 = tk.Button(root, text="Get MDCL Value", command=get_MDCL)
get_button1.grid(row=4, column=1, padx=10, pady=10)

# Create and place the input field and buttons
label2 = tk.Label(root, text="Enter Adjust value to send(0-1):")
label2.grid(row=0, column=3, padx=10, pady=10)

entry2 = tk.Entry(root)
entry2.grid(row=1, column=3, padx=10, pady=10)

send_button2 = tk.Button(root, text="Send Adjust Value", command=send_Adjust)
send_button2.grid(row=2, column=3, padx=10, pady=10)

get_button2 = tk.Button(root, text="Get Current Adjust Value", command=get_Adjust)
get_button2.grid(row=3, column=3, padx=10, pady=10)

current_value_label2 = tk.Label(root, text="Current Adjust Value(0-1): 0")
current_value_label2.grid(row=4, column=3, padx=10, pady=10)

# Start the GUI event loop
root.mainloop()
