#include "HardwareSerial.h"
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
        } else if (command.startsWith("A")) {
            // Parse the channel index, e.g., "C0", "C5"
            int channelIndex = command.substring(1).toInt();
            uint16_t analogValue = channels[channelIndex].getAnalogValue();  // Fixed semicolon
            Serial.write((uint8_t*)&analogValue, sizeof(analogValue));  // Send the 2-byte value

        } else if (command.startsWith("C")) {
            // Parse the channel index, e.g., "C0", "C5"
            int channelIndex = command.substring(1).toInt();
            sendChannelConfig(channelIndex); // Send the corresponding channel configuration
        } else if (command.startsWith("D")) {
            // Handle the "D<channel index>=<deviceIndex>" format
            int equalIndex = command.indexOf('=');
            if (equalIndex != -1) {
                int channelIndex = command.substring(1, equalIndex).toInt();
                int deviceIndex = command.substring(equalIndex + 1).toInt();  // Extract the deviceIndex after '='

                if (channelIndex >= 0 && channelIndex < 10) {
                    // Apply the received deviceIndex to the corresponding channel
                    inputHandler.channels[channelIndex].deviceType = predefinedDevices[deviceIndex].type;
                    inputHandler.channels[channelIndex].deviceId = predefinedDevices[deviceIndex].id;

                    inputHandler.saveToEEPROM();
                    loadChannels();

                    Serial.println(F("Y"));
                } else {
                    Serial.println(F("N"));
                }
            } else {
                Serial.println(F("N"));
            }
        }  else if (command.startsWith("I")) {
            // Handle the "I<channel index>" format
            int channelIndex = command.substring(1).toInt();
            if (channelIndex >= 0 && channelIndex < 10) {
                Serial.println(inputHandler.channels[channelIndex].reverse);
            } else {
                Serial.println(F("Invalid channel index for I command"));
            }
        } else if (command.startsWith("R")) {
            // Handle the "R<channel index>" format to reverse the channel
            int channelIndex = command.substring(1).toInt();
            if (channelIndex >= 0 && channelIndex < 10) {
                bool isReversed = inputHandler.channels[channelIndex].reverse;  // Check if channel is already reversed
                inputHandler.channels[channelIndex].reverse = !isReversed;        // Toggle the reverse state
                
                inputHandler.saveToEEPROM();
                loadChannels();

                Serial.println(F("Y"));
            } else {
                Serial.println(F("N"));
            }
        } else {
            Serial.println(F("N"));
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
