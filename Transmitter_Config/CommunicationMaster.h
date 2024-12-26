#include "Channel.h"

// Assuming you have an array of 10 channels
extern Channel channels[10];

void updateChannelValues() {
    // Send the request key "X" to the slave Arduino
    Serial.println(F("X"));

    // Wait for all 10 bytes of data to arrive
    unsigned long startTime = millis();
    while (Serial.available() < 10) {
        if (millis() - startTime > 50) {
            // Timeout after 50ms if no data is received
            return;
        }
    }

    // Read the incoming data into a temporary array
    byte channelData[10];
    Serial.readBytes(channelData, 10);

    // Update each channel's value
    for (int i = 0; i < 10; i++) {
        channels[i].setValue(channelData[i]);
    }
}
