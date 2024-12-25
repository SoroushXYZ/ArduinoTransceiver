#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "Channel.h"
#include "InputHandler.h"

const uint64_t my_radio_pipe = 0xE8E8F0F0E1LL; // Remember that this code should be the same for the receiver

RF24 radio(9, 10);  // Set CE and CSN pins

// The sizeof this struct should not exceed 32 bytes
struct Data_to_be_sent {
    byte ch1;
    byte ch2;
    byte ch3;
    byte ch4;
    byte ch5;
    byte ch6;
    byte ch7;
    byte ch8;
    byte ch9;
    byte ch10;
};

// Create a variable with the structure above and name it sent_data
Data_to_be_sent sent_data;

// Input handler and channel array
InputHandler inputHandler;
Channel channels[10];

void setup() {
    // Initialize radio
    radio.begin();
    radio.setAutoAck(false);
    radio.setDataRate(RF24_250KBPS);
    radio.openWritingPipe(my_radio_pipe);

    // Initialize Serial for debugging
    Serial.begin(9600);

    // Load channel configurations
    inputHandler.loadFromEEPROM();

    // Configure channels dynamically
    for (int i = 0; i < 10; ++i) {
      ChannelConfig& config = inputHandler.channels[i];
      DeviceType deviceType = (DeviceType)config.deviceType;

      // Set input type based on device type
      switch (deviceType) {
          case JOYSTICK:
          case ANALOG_INPUT:
              channels[i].setInputType(ANALOG);
              break;
          case DIGITAL_INPUT:
              channels[i].setInputType(DIGITAL);
              break;
          case THREE_STATE_SWITCH:
              channels[i].setInputType(THREE_STATE);
              break;
          default:
              channels[i].setInputType(ANALOG); // Default to ANALOG
              break;
      }

      // Assign default pin values from predefined devices
      const PredefinedDevice* device = inputHandler.getPredefinedDevice(config.deviceType, config.deviceId);
      if (device != nullptr) {
          // Setup pins based on device requirements
          if (deviceType == THREE_STATE_SWITCH) {
              channels[i].setPin(device->primaryPin, device->secondaryPin); // Requires two pins
          } else {
              channels[i].setPin(device->primaryPin); // Requires one pin
          }
      } else {
          // Default pin setup if no predefined device
          if (deviceType == THREE_STATE_SWITCH) {
              channels[i].setPin(A0 + (i * 2), A0 + (i * 2) + 1); // Two pins for three-state switch
          } else {
              channels[i].setPin(A0 + i); // One pin for other types
          }
      }

      // Set other properties from the config
      channels[i].setReverse(config.reverse);
    }

    inputHandler.printChannelConfig(0);
}

void loop() {
    // Read values from each channel and assign them to the sent_data structure
    sent_data.ch1 = channels[0].read();
    sent_data.ch2 = channels[1].read();
    sent_data.ch3 = channels[2].read();
    sent_data.ch4 = channels[3].read();
    sent_data.ch5 = channels[4].read();
    sent_data.ch6 = channels[5].read();
    sent_data.ch7 = channels[6].read();
    sent_data.ch8 = channels[7].read();
    sent_data.ch9 = channels[8].read();
    sent_data.ch10 = channels[9].read();

    // Send the updated data
    radio.write(&sent_data, sizeof(Data_to_be_sent));
}
