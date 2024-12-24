#ifndef CHANNEL_H
#define CHANNEL_H

#include <Arduino.h>

class Channel {
public:
    int pin;           // Pin number
    bool isAnalog;     // Analog or digital
    bool reverse;      // Channel reversing
    int trim;          // Trim adjustment
    int analogReadMin = 0;       // Minimum value from analogRead
    int analogReadMax = 1023;    // Maximum value from analogRead
    int minEndpoint;   // Minimum endpoint
    int maxEndpoint;   // Maximum endpoint
    int centerPoint;   // Center point adjustment

    Channel()
        : pin(99), isAnalog(false), reverse(false), trim(0), analogReadMin(0), analogReadMax(1023),
          minEndpoint(0), maxEndpoint(255), centerPoint(127) {}

    void resetToDefault() {
        // isAnalog = false;
        // reverse = false;
        // trim = 0;
        // minEndpoint = 1000;
        // maxEndpoint = 2000;
        // centerPoint = 1500;
    }

    int read() {
        int value = 0;
        if (isAnalog) {
            value = analogRead(pin);
            value = map(value, analogReadMin, analogReadMax, minEndpoint, maxEndpoint);
        } else {
            value = digitalRead(pin);
            value = map(value, LOW, HIGH, minEndpoint, maxEndpoint);
        }
        if (reverse) {
            value = maxEndpoint - value;
        }
        value = constrain(value + trim, minEndpoint, maxEndpoint);
        return value;
    }

    // sent_data.ch1 = map( analogRead(A4), 0, 1024, 0, 255);
};

#endif
