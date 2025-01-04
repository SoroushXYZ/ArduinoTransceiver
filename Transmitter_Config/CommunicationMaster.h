// #include "Channel.h"

// // Assuming you have an array of 10 channels
// extern Channel channels[10];

// void updateChannelValues() {
//     // Send the request key "X" to the slave Arduino
//     Serial.println(F("X"));

//     // Wait for all 10 bytes of data to arrive
//     unsigned long startTime = millis();
//     while (Serial.available() < 10) {
//         if (millis() - startTime > 50) {
//             // Timeout after 50ms if no data is received
//             return;
//         }
//     }

//     // Read the incoming data into a temporary array
//     byte channelData[10];
//     Serial.readBytes(channelData, 10);

//     // Update each channel's value
//     for (int i = 0; i < 10; i++) {
//         channels[i].setValue(channelData[i]);
//     }
// }

#include "Channel.h"

// Assuming you have an array of 10 channels
extern Channel channels[10];

// Variable to track the current value for simulation
uint8_t simulatedValue = 0;

void updateChannelValues() {
    // Increment the value and reset if it exceeds 255
    simulatedValue = (simulatedValue + 5) % 256;  // Increase by 5 for a smoother transition

    // Simulate sending the same value to all channels
    for (int i = 0; i < 10; i++) {
        channels[i].setValue(simulatedValue);
    }

    // Debug output to monitor the simulated values
    Serial.print(F("Simulated Value: "));
    Serial.println(simulatedValue);

    delay(100);  // Add a small delay to make the updates visible in real-time
}
