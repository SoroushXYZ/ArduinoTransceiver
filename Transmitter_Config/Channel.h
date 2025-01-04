#ifndef CHANNEL_H
#define CHANNEL_H

#include <Arduino.h>

// Menu option labels stored in PROGMEM to save RAM
const char menuOption1[] PROGMEM = "Value:";
const char menuOption2[] PROGMEM = "Reverse:";
const char menuOption3[] PROGMEM = "Trim:";
const char menuOption4[] PROGMEM = "Device";
const char menuOption5[] PROGMEM = "Calibrate";

const char* const menuOptions[] PROGMEM = {
    menuOption1, menuOption2, menuOption3, menuOption4, menuOption5
};

class Channel {
private:
    char name[10];           // Name of the channel (fixed-size buffer)
    uint8_t value;          // Computed value of the channel (fits within 0-1023)

public:
    bool reverse;            // Channel reversing
    int16_t trim;            // Trim adjustment (int16_t to minimize memory usage)
    uint16_t minEndpoint;    // Minimum endpoint
    uint16_t maxEndpoint;    // Maximum endpoint
    char deviceType;       // Device type ('J', 'A', 'S', 'D', 'N')
    uint8_t deviceId;      // Device ID

    Channel(int number)
        : value(0), reverse(false), trim(0),
          minEndpoint(0), maxEndpoint(1023), deviceType('N'), deviceId(0){
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
        if (index >= sizeof(menuOptions) / sizeof(menuOptions[0])) {
            strncpy(buffer, "Invalid", bufferSize);
            return;
        }

        // Read the option label from PROGMEM
        char optionLabel[16];
        strncpy_P(optionLabel, (PGM_P)pgm_read_word(&(menuOptions[index])), sizeof(optionLabel));

        switch (index) {
            case 0:  // "Value"
                snprintf(buffer, bufferSize, "%s %u", optionLabel, value);
                break;

            case 1:  // "Reverse"
                snprintf(buffer, bufferSize, "%s %s", optionLabel, reverse ? "True" : "False");
                break;

            case 2:  // "Trim"
                snprintf(buffer, bufferSize, "%s %+d", optionLabel, trim);
                break;

            case 3:  // "Device"
                snprintf(buffer, bufferSize, "%s", optionLabel);  // Static text, no extra value
                break;

            case 4:  // "Calibrate"
                snprintf(buffer, bufferSize, "%s", optionLabel);  // Static text, no extra value
                break;

            default:
                snprintf(buffer, bufferSize, "Unknown");
                break;
        }
    }

    uint8_t getMenuOptionCount() const {
        return sizeof(menuOptions) / sizeof(menuOptions[0]);
    }

    MenuLevel configureItem(uint8_t itemIndex) {
        switch (itemIndex) {
            case 0: return READ_VALUE;     // Show current value
            case 1: reverse = !reverse;    // Toggle reverse
                    return CHANNEL_SETTINGS;  // Stay in settings
            case 2: trim = 0;              // Reset trim
                    return TRIM;           // Go to trim adjustment menu
            case 3: return SELECT_DEVICE;  // Go to device selection menu
            case 4: return CALIBRATE;      // Calibration process
            default: return CHANNEL_SETTINGS; // Default fallback
        }
    }
};

#endif
