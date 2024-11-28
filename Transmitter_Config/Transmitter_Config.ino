#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Channel.h"
#include "MenuManager.h"

// Rotary Encoder Pins
#define CLK_PIN 2
#define DATA_PIN 3
#define SWITCH_PIN 4

// Initialize LCD
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Initialize Channels
Channel channels[10] = {
    Channel(1), Channel(2), Channel(3), Channel(4), Channel(5),
    Channel(6), Channel(7), Channel(8), Channel(9), Channel(10)
};

// Initialize Menu Manager
MenuManager menuManager(&lcd, channels, 10);

// Variables for encoder state
volatile int encoderDirection = 0;
volatile bool buttonPressed = false;

void setup() {
    // Encoder setup
    pinMode(CLK_PIN, INPUT_PULLUP);
    pinMode(DATA_PIN, INPUT_PULLUP);
    pinMode(SWITCH_PIN, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(CLK_PIN), handleEncoder, CHANGE);
    attachInterrupt(digitalPinToInterrupt(SWITCH_PIN), handleButton, FALLING);

    // LCD setup
    lcd.init();
    lcd.backlight();
    menuManager.displayMenu();
}

void loop() {
    if (encoderDirection != 0 || buttonPressed) {
        menuManager.updateEncoder(encoderDirection, buttonPressed);
        encoderDirection = 0;
        buttonPressed = false;
    }
    delay(50); // Stabilize loop
}

void handleEncoder() {
    static int lastClkState = HIGH;
    int clkState = digitalRead(CLK_PIN);
    if (clkState != lastClkState) {
        encoderDirection = (digitalRead(DATA_PIN) == clkState) ? 1 : -1;
    }
    lastClkState = clkState;
}

void handleButton() {
    buttonPressed = true;
}
