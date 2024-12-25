#include "Channel.h"  // Include the Channel class for accessing channel readings
#include <Arduino.h>  // For millis()

// Define modes for communication
enum CommunicationMode {
    SENDUPDATES,
    SENDCONFIG
};

class CommunicationHandler {
private:
    CommunicationMode mode;  // Current communication mode
    unsigned long lastSendTime; // Keeps track of the last send time
    ChannelValues* channelValues;   // Pointer to the struct holding the channel readings
    RF24* radio;             // Pointer to the RF24 instance for communication

    void updateInputs() {
        // Populate the channelValues structure with channel readings
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

public:
    // Constructor
    CommunicationHandler(ChannelValues* dataStruct, RF24* rfModule)
        : mode(SENDUPDATES), lastSendTime(0), channelValues(dataStruct), radio(rfModule) {}

    // Set the communication mode
    void setMode(CommunicationMode newMode) {
        mode = newMode;
    }

    // Get the current mode
    CommunicationMode getMode() const {
        return mode;
    }

    // Loop method to handle communication
    void loop() {
        updateInputs();
        if (mode == SENDUPDATES) {
            // Check if 100ms has passed
            if (millis() - lastSendTime >= 100) {
                // Update the time
                lastSendTime = millis();

                // Send the data over Serial
                Serial.write((uint8_t*)channelValues, sizeof(ChannelValues));

                // Send the data over the radio (optional)
                if (radio) {
                    radio->write(channelValues, sizeof(ChannelValues));
                }
            }
        }

        // For SENDCONFIG mode or other future modes, add logic here
        if (mode == SENDCONFIG) {
            // Configuration sending logic can go here later
        }
    }
};
