#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "Definitions.h"
#include "Channel.h"
#include "CommunicationMaster.h"
#include "MenuManager.h"
#include "TimedUpdateHandler.h"

// Rotary Encoder Pins
#define CLK 11
#define DT 12
#define SW A0  // Switch connected to A0

// Buzzer Pin
#define BUZZER_PIN 10  // Pin 10 connected to the buzzer

// LCD Initialization
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Menu Setup
Channel channels[] = {
    Channel(1), Channel(2), Channel(3), Channel(4), Channel(5),
    Channel(6), Channel(7), Channel(8), Channel(9), Channel(10)
};
MenuManager menu(&lcd, channels, 10);

// Encoder Variables
int lastStateCLK;
int lastButtonState;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

// Custom characters stored in flash memory (PROGMEM)
const byte customChars[8][8] PROGMEM = {
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  // 0 - Empty block (0/5)
    {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10},  // 1 - 1/5 right-filled
    {0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C, 0x1C},  // 2 - 3/5 right-filled
    {0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F},  // 3 - Full block (5/5 right)
    {0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01},  // 4 - 1/5 left-filled (mirrored)
    {0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07},  // 5 - 3/5 left-filled (mirrored)
    {0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F},  // 6 - 5/5 left-filled (mirrored)
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}   // 7 - Empty (placeholder)
};

// Load custom characters from PROGMEM to the LCD
void setupCustomCharacters(LiquidCrystal_I2C &lcd) {
    for (uint8_t i = 0; i < 8; i++) {
        byte buffer[8];
        for (uint8_t j = 0; j < 8; j++) {
            buffer[j] = pgm_read_byte(&(customChars[i][j]));
        }
        lcd.createChar(i, buffer);
    }
}

void setup() {
  Serial.begin(9600);  // Initialize Serial
  delay(1000);
  Serial.println(F("Setup Complete?"));

  // Initialize channel names
  channels[0].setName("Throttle");
  channels[1].setName("Ailerons");
  channels[2].setName("Elevator");
  channels[3].setName("Rudder");
  channels[4].setName("Flaps");

  // Initialize pins
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);  // Set buzzer pin as output

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  setupCustomCharacters(lcd);

  // Initialize encoder state
  lastStateCLK = digitalRead(CLK);
  lastButtonState = digitalRead(SW);

  // Display the initial menu
  menu.displayMenu();
}

void loop() {
  handleEncoder();
  handleTimedUpdates(menu);
}

void handleEncoder() {
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
      tone(BUZZER_PIN, 1700, 10);  // Play buzzer sound (1700 Hz, 10 ms)
    }
  }

  lastStateCLK = currentStateCLK;

  // Handle button press
  if (currentButtonState != lastButtonState) {
    if (millis() - lastDebounceTime > debounceDelay) {
      if (currentButtonState == LOW) {
        buttonPressed = true;
        tone(BUZZER_PIN, 1400, 50);  // Play buzzer sound for button press
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