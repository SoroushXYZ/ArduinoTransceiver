#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Channel.h"
#include "MenuManager.h"

// Rotary Encoder Pins
#define CLK 11
#define DT 12
#define SW A0  // Switch connected to A0

// Buzzer Pin
#define BUZZER_PIN 10  // Pin 10 connected to the buzzer

// LCD Initialization
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Menu Setup
Channel channels[] = {Channel(1), Channel(2), Channel(3), Channel(4)};
MenuManager menu(&lcd, channels, 4);

// Encoder Variables
int lastStateCLK;
int lastButtonState;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

void setup() {
  channels[0].setName("CH1 Ailerons");
  channels[1].setName("CH2 Elevator");
  channels[2].setName("CH3 Throttle");
  channels[3].setName("CH4 Rudder");
  
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);  // Set buzzer pin as output

  lcd.init();
  lcd.backlight();

  lastStateCLK = digitalRead(CLK);
  lastButtonState = digitalRead(SW);

  menu.displayMenu();
}

void loop() {
  int currentStateCLK = digitalRead(CLK);
  int currentButtonState = digitalRead(SW);
  int direction = 0;
  bool buttonPressed = false;

  // Handle rotation
  if (currentStateCLK != lastStateCLK) {
    if (millis() - lastDebounceTime > debounceDelay) {
      // Reverse the direction by flipping the comparison
      if (digitalRead(DT) == currentStateCLK) { // Swapped condition
        direction = 1; // CW (Reversed to CCW)
      } else {
        direction = -1; // CCW (Reversed to CW)
      }
      lastDebounceTime = millis();
      tone(BUZZER_PIN, 1000, 50);  // Play buzzer sound (1000 Hz, 50 ms)
    }
  }

  lastStateCLK = currentStateCLK;

  // Handle button press
  if (currentButtonState != lastButtonState) {
    if (millis() - lastDebounceTime > debounceDelay) {
      if (currentButtonState == LOW) {
        buttonPressed = true;
        tone(BUZZER_PIN, 1000, 50);  // Play buzzer sound for button press
      }
      lastDebounceTime = millis();
    }
  }

  lastButtonState = currentButtonState;

  // Update the menu manager
  if (direction != 0 || buttonPressed) {
    menu.updateEncoder(direction, buttonPressed);
  }
}
