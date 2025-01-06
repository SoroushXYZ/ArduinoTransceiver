#ifndef CHANNEL_H
#define CHANNEL_H

#include <Arduino.h>

// Enum to define types of input
enum InputType {
    ANALOG,          // Analog input
    DIGITAL,         // Digital input
    THREE_STATE      // 3-state switch input
};

class Channel {
private:
    int pin;               // Pin number (single pin for analog and digital, two pins for 3-state switch)
    int pin2;                 // Second pin for 3-state switch
    InputType inputType;   // Type of input (analog, digital, or 3-state switch)
    bool reverse;          // Channel reversing
    int trim;              // Trim adjustment
    int analogReadMin;     // Minimum value from analogRead
    int analogReadMax;     // Maximum value from analogRead
    int minEndpoint;       // Minimum endpoint
    int maxEndpoint;       // Maximum endpoint
    int centerPoint;       // Center point adjustment

public:
    Channel()
        : pin(-1), pin2(-1), inputType(ANALOG), reverse(false), trim(0), analogReadMin(0), analogReadMax(1023),
          minEndpoint(0), maxEndpoint(255), centerPoint(127) {}

    // Setter for pin with mode handling
    void setPin(int p) {
        pin = p;
        // Set the pin mode based on input type
        if (inputType == ANALOG) {
            pinMode(pin, INPUT);  // Analog inputs are set to INPUT
        } else if (inputType == DIGITAL || inputType == THREE_STATE) {
            pinMode(pin, INPUT_PULLUP);  // Digital or 3-state inputs are set to INPUT_PULLUP
        }
    }

    // Setter for pin with mode handling (two pins for 3-state switch)
    void setPin(int p, int p2) {
        pin = p;
        pin2 = p2;
        // Set the pin mode based on input type
        if (inputType == ANALOG) {
            pinMode(pin, INPUT);  // Analog inputs are set to INPUT
        } else if (inputType == DIGITAL || inputType == THREE_STATE) {
            pinMode(pin, INPUT_PULLUP);  // Digital or 3-state inputs are set to INPUT_PULLUP
            pinMode(pin2, INPUT_PULLUP);  // Set second pin to INPUT_PULLUP for 3-state switch
        }
    }

    // Getter for pin
    int getPin() const {
        return pin;
    }

    // Setter for inputType with mode handling
    void setInputType(InputType type) {
        inputType = type;
        // Set the pin mode based on the new input type
        if (inputType == ANALOG) {
            pinMode(pin, INPUT);  // Analog inputs are set to INPUT
        } else if (inputType == DIGITAL || inputType == THREE_STATE) {
            pinMode(pin, INPUT_PULLUP);  // Digital or 3-state inputs are set to INPUT_PULLUP
            pinMode(pin2, INPUT_PULLUP);  // Digital or 3-state inputs are set to INPUT_PULLUP
        }
    }

    // Getter for inputType
    InputType getInputType() const {
        return inputType;
    }

    void getReverse() {
        return reverse;
    }

    void setReverse(bool r) {
        reverse = r;
    }

    void resetToDefault() {
        // Default reset functionality (optional)
    }

    uint16_t getAnalogValue() {
        return ::analogRead(pin);
    }

    int read() {
        int value = 0;
        switch (inputType) {
            case ANALOG:
                value = analogRead(pin);
                value = map(value, analogReadMin, analogReadMax, minEndpoint, maxEndpoint);
                break;

            case DIGITAL:
                value = digitalRead(pin);
                value = map(value, LOW, HIGH, minEndpoint, maxEndpoint);
                break;

            case THREE_STATE:
                value = readSwitch();  // Read from two digital pins for 3-state switch
                break;

            default:
                value = 0;
                break;
        }

        if (reverse) {
            value = maxEndpoint - value;
        }

        value = constrain(value + trim, minEndpoint, maxEndpoint);
        return value;
    }

    int readSwitch() {
        // Read the two pins for the 3-state switch
        int state1 = digitalRead(pin);
        int state2 = digitalRead(pin2);
        
        if (state1 == LOW && state2 == LOW) {
            return centerPoint;  // State 2: LOW, HIGH
        } else if (state1 == LOW && state2 == HIGH) {
          return minEndpoint;  // State 1: both LOW
        } else if (state1 == HIGH && state2 == LOW) {
            return maxEndpoint;  // State 3: HIGH, LOW
        } else {
            return centerPoint;  // Default to center if both HIGH (optional)
        }
    }
};

#endif
