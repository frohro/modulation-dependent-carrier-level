#include <Arduino.h>
#include <vector>

// Define a structure to store the messages and their metadata
struct Message {
  unsigned long message_number;
  Stream* serial_port;
  String message_to_print;
  bool printed; // Flag to indicate if the message has been printed
};

// Create a list to store the messages
std::vector<Message> message_list;

// Function to print messages every specified interval
void print_after_millis(unsigned long time_to_print, unsigned long message_number, Stream* serial_port, const char* message_to_print) {
  static unsigned long lastMillis = 0;

  // Add the message to the list
  Message new_message = {message_number, serial_port, String(message_to_print), false};
  message_list.push_back(new_message);

  // Check if it's time to print the messages
  if (millis() - lastMillis >= time_to_print) {
    lastMillis = millis();

    // Print the messages in the order they were received
    for (auto& msg : message_list) {
      if (msg.message_number == message_number && !msg.printed) {
        msg.serial_port->printf("%s", msg.message_to_print.c_str());
        msg.printed = true; // Mark the message as printed
      }
    }

    // Remove printed messages from the list
    message_list.erase(
      std::remove_if(message_list.begin(), message_list.end(), [](const Message& msg) { return msg.printed; }),
      message_list.end()
    );
  }
}