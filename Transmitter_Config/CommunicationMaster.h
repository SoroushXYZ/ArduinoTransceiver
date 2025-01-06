#include "HardwareSerial.h"
#include <avr/common.h>
#include "Channel.h"

// Assuming you have an array of 10 channels
extern Channel channels[10];

void clearSerialBuffer() {
    while (Serial.available() > 0) {
        Serial.read();  // Discard any incoming data
    }
}

void updateChannelValues() {
    // Send the request key "X" to the slave Arduino
    clearSerialBuffer();  // Clear any previous data in the buffer
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

void updateAnalogValue(int channelIndex){
    clearSerialBuffer();  // Clear any previous data in the buffer
    Serial.print(F("A"));  // Send the request command "A"
    Serial.println(channelIndex);  // Send the channel index

    // Wait for 2 bytes of data (since analog values are 10 bits, packed into 2 bytes)
    unsigned long startTime = millis();
    while (Serial.available() < 2) {  // Expecting 2 bytes for `uint16_t`
        if (millis() - startTime > 50) {
            Serial.println(F("Timeout: No response"));
            return;  // Exit the function if no data is received within 50ms
        }
    }

    // Read the 2-byte analog value
    int analogValue = -1;
    Serial.readBytes((char*)&analogValue, 2);

    // Update the channel value with the received analog value
    channels[channelIndex].setAnalogValue(analogValue);
}

// Function to request a channel configuration
void updateChannelConfigs(int channelIndex) {
    // Send request string, e.g., "C0" for channel 0
    clearSerialBuffer();  // Clear any previous data in the buffer
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

void reverseChannel(int channelIndex) {
    // Send the request key "R" followed by the channel index
    clearSerialBuffer();  // Clear any previous data in the buffer
    Serial.print(F("R"));
    Serial.println(channelIndex);

    // Wait for the confirmation message
    unsigned long startTime = millis();
    while (!Serial.available()) {
        if (millis() - startTime > 1000) {
            Serial.println(F("Timeout: No response"));
            return;
        }
    }

    // Read the confirmation message to confirm the device change
    while (Serial.available()) {
        Serial.read();  // Discard any incoming data
    }
}

void selectDevice(int channelIndex, int deviceIndex) {
    // Send the request key "D" followed by the channel index and device index
    clearSerialBuffer();  // Clear any previous data in the buffer
    Serial.print(F("D"));
    Serial.print(channelIndex);
    Serial.print(F("="));
    Serial.println(deviceIndex);

    // Wait for the confirmation message
    unsigned long startTime = millis();
    while (!Serial.available()) {
        if (millis() - startTime > 1000) {
            Serial.println(F("Timeout: No response"));
            return;
        }
    }

    // Read the confirmation message to confirm the device change
    while (Serial.available()) {
        Serial.read();  // Discard any incoming data
    }
}

void sendCalibrationData(int selectedIndex){
    // Send the request key "Z" followed by the channel index and endpoints
    delay(100);
    clearSerialBuffer();  // Clear any previous data in the buffer
    Serial.print(F("Z"));
    Serial.print(selectedIndex);
    Serial.print(F(","));
    Serial.print(channels[selectedIndex].analogReadMin);
    Serial.print(F(","));
    Serial.println(channels[selectedIndex].analogReadMax);

    // Wait for the confirmation message
    unsigned long startTime = millis();
    while (!Serial.available()) {
        if (millis() - startTime > 1000) {
            Serial.println(F("Timeout: No response"));
            return;
        }
    }

    // Read the confirmation message to confirm the device change
    while (Serial.available()) {
        Serial.read();  // Discard any incoming data
    }
}

void sendTrim(int selectedIndex) {
    // Send the request key "T" followed by the channel index and trim value
    clearSerialBuffer();  // Clear any previous data in the buffer
    Serial.print(F("T"));
    Serial.print(selectedIndex);
    Serial.print(F(","));
    Serial.println(channels[selectedIndex].trim);

    // Wait for the confirmation message
    unsigned long startTime = millis();
    while (!Serial.available()) {
        if (millis() - startTime > 1000) {
            Serial.println(F("Timeout: No response"));
            return;
        }
    }

    // Clear any remaining bytes in the serial buffer
    while (Serial.available()) {
        Serial.read();
    }
}
