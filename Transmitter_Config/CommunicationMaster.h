#include <avr/common.h>
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

// Function to request a channel configuration
void updateChannelConfigs(int channelIndex) {
    // Send request string, e.g., "C0" for channel 0
    Serial.print(F("C"));
    Serial.println(channelIndex);

    // Wait for the incoming struct data
    unsigned long startTime = millis();
    int structSize = sizeof(ChannelConfig);
    while (Serial.available() < structSize) {
        if (millis() - startTime > 1000) {
            Serial.println(F("Timeout: No response"));
            return;
        }
    }

    // Read the struct data from the serial buffer
    byte buffer[structSize];
    Serial.readBytes(buffer, structSize);

    // Convert the bytes into a struct
    ChannelConfig config;
    memcpy(&config, buffer, structSize);

    channels[channelIndex].reverse = config.reverse;
    channels[channelIndex].trim = config.trim;
    channels[channelIndex].deviceType = config.deviceType;
    channels[channelIndex].deviceId = config.deviceId;

}