#ifndef CHANNEL_H
#define CHANNEL_H

#include <Arduino.h>

// Device type labels stored in PROGMEM
const char deviceType_J[] PROGMEM = "Joystick";
const char deviceType_A[] PROGMEM = "Analog";
const char deviceType_S[] PROGMEM = "Switch";
const char deviceType_D[] PROGMEM = "Digital";
const char deviceType_N[] PROGMEM = "Null";

// Lookup table for device types
const char* const deviceTypeNames[] PROGMEM = {
    deviceType_J,  // 'J'
    deviceType_A,  // 'A'
    deviceType_S,  // 'S'
    deviceType_D,  // 'D'
    deviceType_N   // 'N'
};

// Menu option labels stored in PROGMEM to save RAM
const char menuOption1[] PROGMEM = "Value:";
const char menuOption2[] PROGMEM = "Reverse:";
const char menuOption3[] PROGMEM = "Trim:";
const char menuOption4[] PROGMEM = "Device:";
const char menuOption5[] PROGMEM = "Calibrate";

const char* const menuOptions[] PROGMEM = {
    menuOption1, menuOption2, menuOption3, menuOption4, menuOption5
};

class Channel {
private:
    char name[10];           // Name of the channel (fixed-size buffer)
    uint8_t value;          // Computed value of the channel (fits within 0-255)
    int analogValue;          // Read analog value of the channel (fits within 0-1023)

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

    uint8_t getValue() const { return value; }
    void setValue(uint16_t newValue) { value = newValue; }
    int getAnalogValue() const { return analogValue; }
    void setAnalogValue(int newValue) { analogValue = newValue; }

    // Get the device type name from PROGMEM
    const char* getDeviceTypeName() const {
        switch (deviceType) {
            case 'J': return deviceType_J;
            case 'A': return deviceType_A;
            case 'S': return deviceType_S;
            case 'D': return deviceType_D;
            default:  return deviceType_N;
        }
    }

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
                char deviceTypeBuffer[16];  // Temporary buffer in RAM for the device type
                strncpy_P(deviceTypeBuffer, getDeviceTypeName(), sizeof(deviceTypeBuffer) - 1);
                deviceTypeBuffer[sizeof(deviceTypeBuffer) - 1] = '\0';  // Null-terminate
                snprintf(buffer, bufferSize, "%s %s%d", optionLabel, deviceTypeBuffer, deviceId);
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

    void startCalibration(){
      minEndpoint = 1023;
      maxEndpoint = 0;
    }

    void calibrationLoop(){
      if(analogValue != -1){
        if(minEndpoint > analogValue){ minEndpoint = analogValue; }
        if(maxEndpoint < analogValue){ maxEndpoint = analogValue; }
      }
    }

    MenuLevel configureItem(uint8_t itemIndex) {
        switch (itemIndex) {
            case 0: return READ_VALUE;     // Show current value
            case 1: return REVERSE;        // Toggle reverse
            case 2: trim = 0;              // Reset trim
                    return TRIM;           // Go to trim adjustment menu
            case 3: return SELECT_DEVICE;  // Go to device selection menu
            case 4: startCalibration();
              return CALIBRATE;      // Calibration process
            default: return CHANNEL_SETTINGS; // Default fallback
        }
    }
};

#endif
