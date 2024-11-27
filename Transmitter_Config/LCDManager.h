#ifndef LCDMANAGER_H
#define LCDMANAGER_H

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

class LCDManager {
private:
    LiquidCrystal_I2C lcd;

public:
    LCDManager(uint8_t address, uint8_t cols, uint8_t rows)
        : lcd(address, cols, rows) {}

    void init() {
        lcd.init();       // Use init() for this library
        lcd.backlight();  // Turn on the backlight
        lcd.clear();      // Clear the display
        lcd.setCursor(0, 0);
        lcd.print("Encoder Status:");
    }

    void displayStatus(int position, bool switchState) {
        lcd.setCursor(0, 1);
        lcd.print("Pos: ");
        lcd.print(position);
        lcd.print("    ");  // Clear remaining digits

        lcd.setCursor(0, 2);
        lcd.print("Switch: ");
        lcd.print(switchState ? "Pressed" : "Released");
    }
};

#endif
