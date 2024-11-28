#ifndef CHANNEL_H
#define CHANNEL_H

#include <Arduino.h>

class Channel {
private:
    String name;       // Name of the channel
    bool reverse;      // Channel reversing
    int trim;          // Trim adjustment
    int minEndpoint;   // Minimum endpoint
    int maxEndpoint;   // Maximum endpoint
    int centerPoint;   // Center point adjustment

    // Configurable items (fixed-size array)
    const char* configurableItems[5] = {
        "Reverse", "Trim", "Min Endpoint", "Max Endpoint", "Center Point"
    };

public:
    Channel(int number)
        : name("CH" + String(number)), reverse(false), trim(0),
          minEndpoint(1000), maxEndpoint(2000), centerPoint(1500) {}

    String getName() const { return name; }

    // Get configurable items as a fixed-size array
    const char** getConfigurableItems() const {
        return configurableItems;
    }

    int getConfigurableItemCount() const {
        return sizeof(configurableItems) / sizeof(configurableItems[0]);
    }

    void configureItem(int itemIndex) {
        // Placeholder logic for configuration
        switch (itemIndex) {
            case 0: Serial.println("Configure Reverse"); break;
            case 1: Serial.println("Configure Trim"); break;
            case 2: Serial.println("Configure Min Endpoint"); break;
            case 3: Serial.println("Configure Max Endpoint"); break;
            case 4: Serial.println("Configure Center Point"); break;
        }
    }
};

#endif
