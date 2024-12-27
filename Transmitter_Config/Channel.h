#ifndef CHANNEL_H
#define CHANNEL_H

#include <Arduino.h>

class Channel {
private:
    char name[10];     // Name of the channel (fixed-size buffer)
    int value;         // Computed value of the channel
    bool reverse;      // Channel reversing
    int trim;          // Trim adjustment
    int minEndpoint;   // Minimum endpoint
    int maxEndpoint;   // Maximum endpoint
    int centerPoint;   // Center point adjustment

    // Configurable items
    const char* configurableItems[5] = {
        "Reverse", "Trim", "Min Endpoint", "Max Endpoint", "Center Point"
    };

public:
    Channel(int number)
        : value(0), reverse(false), trim(0),
          minEndpoint(1000), maxEndpoint(2000), centerPoint(1500) {
        snprintf(name, sizeof(name), "CH%d", number);
    }

    const char* getName() const { return name; }
    void setName(const char* newName) {
        strncpy(name, newName, sizeof(name) - 1);
        name[sizeof(name) - 1] = '\0'; // Ensure null-termination
    }

    int getValue() const { return value; }
    void setValue(int newValue) { value = newValue; }  // Add this to set value dynamically

    const char** getConfigurableItems() const {
        return configurableItems;
    }

    int getConfigurableItemCount() const {
        return sizeof(configurableItems) / sizeof(configurableItems[0]);
    }

    void configureItem(int itemIndex) {
        switch (itemIndex) {
            case 0: reverse = !reverse; break; // Toggle reverse
            case 1: trim = 0; break;          // Reset trim (example action)
            case 2: minEndpoint = 1000; break; // Reset endpoints
            case 3: maxEndpoint = 2000; break;
            case 4: centerPoint = 1500; break; // Reset center point
        }
    }

    void resetToDefault() {
        reverse = false;
        trim = 0;
        minEndpoint = 1000;
        maxEndpoint = 2000;
        centerPoint = 1500;
    }
};

#endif
