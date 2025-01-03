#ifndef CHANNEL_H
#define CHANNEL_H

#include <Arduino.h>

class Channel {
private:
    char name[10];           // Name of the channel (fixed-size buffer)
    uint16_t value;          // Computed value of the channel (fits within 0-1023)
    bool reverse;            // Channel reversing
    int16_t trim;            // Trim adjustment (int16_t to minimize memory usage)
    uint16_t minEndpoint;    // Minimum endpoint
    uint16_t maxEndpoint;    // Maximum endpoint
    uint16_t centerPoint;    // Center point adjustment

    // Configurable items stored in PROGMEM to save RAM
    static const char configurableItems[5][16] PROGMEM;

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

    uint16_t getValue() const { return value; }
    void setValue(uint16_t newValue) { value = newValue; }

    void getConfigurableItem(uint8_t index, char* buffer, size_t bufferSize) const {
        if (index < 5) {
            strncpy_P(buffer, configurableItems[index], bufferSize - 1);
            buffer[bufferSize - 1] = '\0'; // Ensure null-termination
        }
    }

    uint8_t getConfigurableItemCount() const {
        return sizeof(configurableItems) / sizeof(configurableItems[0]);
    }

    void configureItem(uint8_t itemIndex) {
        switch (itemIndex) {
            case 0: reverse = !reverse; break; // Toggle reverse
            case 1: trim = 0; break;          // Reset trim
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

// Definition of configurable items in PROGMEM
const char Channel::configurableItems[5][16] PROGMEM = {
    "Reverse", "Trim", "Min Endpoint", "Max Endpoint", "Center Point"
};

#endif
