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

    // Menu options stored in PROGMEM to save RAM
    static const char menuOptions[5][16] PROGMEM;

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

    void getMenuOption(uint8_t index, char* buffer, size_t bufferSize) const {
        if (index < 5) {
            strncpy_P(buffer, menuOptions[index], bufferSize - 1);
            buffer[bufferSize - 1] = '\0'; // Ensure null-termination
        }
    }

    uint8_t getMenuOptionCount() const {
        return sizeof(menuOptions) / sizeof(menuOptions[0]);
    }

    MenuLevel configureItem(uint8_t itemIndex) {
        switch (itemIndex) {
            case 0: return READ_VALUE;     // Show current value
            case 1: return SELECT_DEVICE;  // Go to device selection menu
            case 2: return CALIBRATE;      // Calibration process
            case 3: reverse = !reverse;    // Toggle reverse
                    return CHANNEL_SETTINGS;  // Stay in settings
            case 4: trim = 0;              // Reset trim
                    return TRIM;           // Go to trim adjustment menu
            default: return CHANNEL_SETTINGS; // Default fallback
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

// Definition of menu options in PROGMEM
const char Channel::menuOptions[5][16] PROGMEM = {
    "Read Value", "Select Device", "Calibrate", "Reverse", "Trim"
};

#endif
