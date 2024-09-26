#include <Arduino.h>
#include <globals.h>

// Function to generate a triangle wave
uint16_t generate_triangle_wave(uint16_t amplitude, uint32_t period_us) {
    static int16_t current_value = 2048;
    static int16_t increment = 0;
    static uint16_t prev_amplitude = 0;
    static uint32_t prev_period_us = 0;
    static bool increasing = true;

    // Ensure amplitude is less than or equal to 2048
    if (amplitude > 2048) {
        amplitude = 2048;
    }

    // Check if the parameters have changed
    if (amplitude != prev_amplitude || period_us != prev_period_us) {
        // Compute the increment/decrement value
        uint32_t half_period_us = period_us / 2; // Half period for triangle wave
        increment = (2 * amplitude) / (half_period_us / 10); // Increment per 10 us
        Serial.printf("Increment: %d\n", increment);

        // Store the new parameters
        prev_amplitude = amplitude;
        prev_period_us = period_us;
    }

    // Generate the next point in the triangle wave
    if (increasing) {
        current_value += increment;
        if (current_value >= amplitude + 2048) {
            current_value = amplitude + 2048;
            increasing = false;
        }
    } else {
        current_value -= increment;
        if (current_value <= 2048 - amplitude) {
            current_value = 2048 - amplitude;
            increasing = true;
        }
    }
    Serial.printf("Current value: %d\n", current_value);
    return current_value;
}