#include <Arduino.h>
#include <vector>
#include <cstdarg>

// Define a structure to store the messages and their metadata
struct Message {
  unsigned long message_number;
  Stream* serial_port;
  String message_to_print;
};

// Define a class to manage the message collection and printing
class MessagePrinter {
public:
  // Constructor to initialize the maximum number of messages and the one-time flag
  MessagePrinter(size_t max_messages, bool one_time)
    : max_messages(max_messages), one_time(one_time), messages_printed(false) {}

  // Variadic template function to collect and print messages with formatting
  void print_after_n_messages(unsigned long message_number, Stream* serial_port, const char* format, ...) {
    // Format the message
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    // Add the formatted message to the list
    Message new_message = {message_number, serial_port, String(buffer)};
    message_list.push_back(new_message);

    // Check if the number of stored messages has reached the maximum
    if (message_list.size() >= max_messages) {
      // Print the messages in the order they were received
      if (!one_time || (one_time && !messages_printed)) {
        for (const auto& msg : message_list) {
          msg.serial_port->print(msg.message_to_print);
        }
        messages_printed = true; // Set the flag to true if one_time is true
      }

      // Clear the list after printing if one_time is false
      if (!one_time) {
        message_list.clear();
        messages_printed = false; // Reset the flag
      }
    }
  }

private:
  std::vector<Message> message_list; // List to store the messages
  size_t max_messages;               // Maximum number of messages to store
  bool one_time;                     // Flag to indicate if messages should be printed only once
  bool messages_printed;             // Flag to track if messages have been printed
};

void setup() {
  Serial.begin(250000);
  Serial3.begin(250000);
  while (!Serial) {
    ; // Wait for Serial to be ready
  }
  while (!Serial3) {
    ; // Wait for Serial3 to be ready
  }
}

void loop() {
  // Create instances of MessagePrinter with different configurations
  static MessagePrinter printer1(16, true);
  static int i = 0;

  // Example usage
  printer1.print_after_n_messages(1, &Serial3, "Variable_1");

  printer1.print_after_n_messages(1, &Serial3, "%d,",i++);

  // printer1.print_after_n_messages(1, &Serial3, "Variable_2");

  // printer1.print_after_n_messages(1, &Serial3, "%d\n",2*i++);

  // Simulate calling the function every so many microseconds
  delayMicroseconds(10);
}