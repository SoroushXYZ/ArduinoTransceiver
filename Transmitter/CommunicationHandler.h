#ifndef COMMUNICATION_HANDLER_H
#define COMMUNICATION_HANDLER_H

#include "Channel.h"
#include <Arduino.h>  // For millis()
#include <RF24.h>     // RF24 library for radio communication

class CommunicationHandler {
private:
    unsigned long lastSendTime; // Keeps track of the last send time
    ChannelValues* channelValues;   // Pointer to the struct holding the channel readings
    RF24* radio;             // Pointer to the RF24 instance for communication

    // Buffer for incoming serial data
    String commandBuffer;

    // Populate the channelValues structure with channel readings
    void updateInputs() {
        channelValues->ch1 = channels[0].read();
        channelValues->ch2 = channels[1].read();
        channelValues->ch3 = channels[2].read();
        channelValues->ch4 = channels[3].read();
        channelValues->ch5 = channels[4].read();
        channelValues->ch6 = channels[5].read();
        channelValues->ch7 = channels[6].read();
        channelValues->ch8 = channels[7].read();
        channelValues->ch9 = channels[8].read();
        channelValues->ch10 = channels[9].read();
    }

    // Send channel updates over serial
    void sendSerialUpdates() {
        Serial.write((uint8_t*)channelValues, sizeof(ChannelValues));
    }

    // Send channel configuration over serial
    void sendChannelConfig(int channelIndex) {
        if (channelIndex >= 0 && channelIndex < 10) {
            Serial.write((uint8_t*)&inputHandler.channels[channelIndex], sizeof(ChannelConfig));
        } else {
            Serial.println(F("Invalid channel index"));
        }
    }

    // Send channel updates over the radio
    void sendRadioUpdates() {
        if (radio) {
            radio->write(channelValues, sizeof(ChannelValues));
        }
    }

    // Process the received command
    void processCommand(const String& command) {
        if (command == "X") {
            updateInputs();     // Update the channel values
            sendSerialUpdates(); // Send data over Serial
        } else if (command.startsWith("C")) {
            // Parse the channel index, e.g., "C0", "C5"
            int channelIndex = command.substring(1).toInt();
            sendChannelConfig(channelIndex); // Send the corresponding channel configuration
        }
        // Add more commands as needed here
    }

public:
    // Constructor
    CommunicationHandler(ChannelValues* dataStruct, RF24* rfModule)
        : lastSendTime(0), channelValues(dataStruct), radio(rfModule) {}

    // Loop method to handle communication
    void loop() {
        // Check for serial input
        while (Serial.available() > 0) {
            char receivedChar = Serial.read();
            
            // Handle newline character
            if (receivedChar == '\n') {
                processCommand(commandBuffer); // Process the full command
                commandBuffer = "";            // Clear the buffer
            } else if (receivedChar != '\r') {
                commandBuffer += receivedChar; // Append character to buffer
            }
        }

        // Always send updates over the radio
        updateInputs();   // Update the channel values
        sendRadioUpdates();
    }
};

#endif // COMMUNICATION_HANDLER_H
