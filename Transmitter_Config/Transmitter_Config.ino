#include "LCDManager.h"
#include "EncoderManager.h"

// Define pins for the rotary encoder
#define CLK_PIN 2
#define DATA_PIN 3
#define SWITCH_PIN 7

LCDManager lcdManager(0x27, 20, 4); // Initialize LCD
EncoderManager encoderManager;      // Initialize Encoder Manager

void setup() {
    lcdManager.init();                                // Initialize LCD
    encoderManager.init(CLK_PIN, DATA_PIN, SWITCH_PIN); // Initialize encoder with interrupt
}

void loop() {
    // Update and display encoder position and button status
    encoderManager.update();

    // Display encoder data on LCD
    lcdManager.displayStatus(
        encoderManager.getPosition(), 
        encoderManager.isSwitchPressed()
    );
    
    delay(50); // Small delay for stability
}
