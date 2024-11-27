#ifndef ENCODERMANAGER_H
#define ENCODERMANAGER_H

#include <Arduino.h>

class EncoderManager {
private:
    int clkPin, dataPin, switchPin;
    volatile int position;           // Encoder position
    volatile bool encoderMoved;      // Flag for movement
    volatile bool buttonPressed;     // Button press status
    static EncoderManager* instance; // Static instance pointer
    bool lastSwitchState;
    unsigned long lastDebounceTime;
    const unsigned long debounceDelay = 50;

public:
    EncoderManager() : clkPin(0), dataPin(0), switchPin(0), position(0), encoderMoved(false), buttonPressed(false), lastSwitchState(HIGH), lastDebounceTime(0) {}

    void init(int clk, int data, int sw) {
        clkPin = clk;
        dataPin = data;
        switchPin = sw;

        pinMode(clkPin, INPUT_PULLUP);
        pinMode(dataPin, INPUT_PULLUP);
        pinMode(switchPin, INPUT_PULLUP);

        // Assign this instance to the static pointer
        instance = this;

        attachInterrupt(digitalPinToInterrupt(clkPin), handleInterrupt, CHANGE);
    }

    void update() {
        // Handle button debounce
        bool currentSwitchState = digitalRead(switchPin);
        if (currentSwitchState != lastSwitchState) {
            lastDebounceTime = millis();
        }

        if ((millis() - lastDebounceTime) > debounceDelay) {
            if (currentSwitchState == LOW && !buttonPressed) {
                buttonPressed = true;
            } else if (currentSwitchState == HIGH && buttonPressed) {
                buttonPressed = false;
            }
        }
        lastSwitchState = currentSwitchState;
    }

    int getPosition() const {
        return position;
    }

    bool isSwitchPressed() const {
        return buttonPressed;
    }

    static void handleInterrupt() {
        // Call the instance's method
        if (instance) {
            instance->updateEncoder();
        }
    }

private:
    void updateEncoder() {
        int clkState = digitalRead(clkPin);
        int dtState = digitalRead(dataPin);

        if (clkState != dtState) {
            position++;
        } else {
            position--;
        }
        encoderMoved = true;
    }
};

// Initialize the static instance pointer
EncoderManager* EncoderManager::instance = nullptr;

#endif
