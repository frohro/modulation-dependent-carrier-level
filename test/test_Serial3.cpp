#include <Arduino.h>
// THis program tests Serial3 and Serial to make sure they are working.
void setup() {
  // Initialize Serial and Serial3
  Serial.begin(9600);
  Serial3.begin(9600);

  // Wait for Serial Monitor to open for debugging
  while (!Serial) {
    ; // Wait for Serial Monitor to open
  }

  // Print initial messages to both Serial and Serial3
  Serial.println("Serial is working!");
  Serial3.println("Serial3 is working!");
}

void loop() {
  // Check if data is available on Serial
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    Serial.print("Received on Serial: ");
    Serial.println(data);
  }

  // Check if data is available on Serial3
  if (Serial3.available()) {
    String data = Serial3.readStringUntil('\n');
    Serial3.print("Received on Serial3: ");
    Serial3.println(data);
  }

  // Send a test message every 2 seconds
  static unsigned long lastMillis = 0;
  if (millis() - lastMillis >= 2000) {
    lastMillis = millis();
    Serial.println("Test message from Serial");
    Serial3.println("Test message from Serial3");
  }
}