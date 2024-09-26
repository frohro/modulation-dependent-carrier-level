#include <Arduino.h>

// Function to print data for plotting (N <= 500)
void print_for_plot(uint16_t N, uint16_t variable1, const char* label1, 
                    uint16_t variable2 = 0, const char* label2 = "", 
                    uint16_t variable3 = 0, const char* label3 = "") {
    static uint16_t data_counter = 0;
    static uint16_t var1[500];
    static uint16_t var2[500];
    static uint16_t var3[500];

    // Store the variables in the arrays
    if (data_counter < N) {
        var1[data_counter] = variable1;
        var2[data_counter] = variable2;
        var3[data_counter] = variable3;
        data_counter++;
    }

    // Once we have N values, wait for the serial monitor to connect
    if (data_counter == N) {
        while (!Serial3) {
            // Wait for serial monitor to connect
        }

        // Print the labels
        Serial3.print("index,");
        Serial3.print(label1);
        if (label2[0] != '\0') {
            Serial3.print(",");
            Serial3.print(label2);
        }
        if (label3[0] != '\0') {
            Serial3.print(",");
            Serial3.print(label3);
        }
        Serial3.println();

        // Print the values
        for (uint16_t i = 0; i < N; i++) {
            Serial3.printf("%u,%u", i, var1[i]);
            if (label2[0] != '\0') {
                Serial3.printf(",%u", var2[i]);
            }
            if (label3[0] != '\0') {
                Serial3.printf(",%u", var3[i]);
            }
            Serial3.println();
        }

        // Reset the counter to avoid printing again
        data_counter++;
    }
}