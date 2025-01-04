#ifndef MENUMANAGER_H
#define MENUMANAGER_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "Channel.h"

class MenuManager {
private:
    LiquidCrystal_I2C* lcd;
    Channel* channels;
    uint8_t channelCount;  // Number of channels
    uint8_t selectedIndex; // The currently selected channel index
    MenuLevel menuLevel;   // Enum to track the current menu level
    uint8_t subMenuIndex;  // The index of the selected setting
    uint8_t scrollOffset;  // The topmost visible item index in settings
    unsigned long lastButtonPressTime; // Track the last button press time
    static const unsigned long buttonTimeout = 500; // Timeout for button press in ms

    static const uint8_t maxVisibleItems = 3; // Number of scrollable lines (excluding the title line)

public:
    MenuManager(LiquidCrystal_I2C* lcd, Channel* channels, uint8_t count)
        : lcd(lcd), channels(channels), channelCount(count), selectedIndex(0),
          menuLevel(CHANNEL_LIST), subMenuIndex(0), scrollOffset(0), lastButtonPressTime(0) {}

    void displayMenu() {
        lcd->clear();

        switch (menuLevel) {
            case CHANNEL_LIST:
                displayChannelList();
                break;
            case CHANNEL_SETTINGS:
                displayChannelSettings();
                break;
            case READ_VALUE:
                displayReadValue();
                break;
            case SELECT_DEVICE:
                displaySelectDevice();
                break;
            case CALIBRATE:
                displayCalibrate();
                break;
            case TRIM:
                displayTrim();
                break;
        }
    }

    void displayChannelList() {
        for (uint8_t i = 0; i < 4; i++) {
            uint8_t channelIndex = scrollOffset + i;
            if (channelIndex >= channelCount) break;

            lcd->setCursor(0, i);
            lcd->print(channelIndex == selectedIndex ? F("> ") : F("  "));
            lcd->print(channels[channelIndex].getName());
        }
    }

    void displayChannelSettings() {
        lcd->setCursor(0, 0);
        lcd->print(F("Channel: "));
        lcd->print(channels[selectedIndex].getName());  // Display channel name on the first line

        char buffer[16];
        uint8_t itemCount = channels[selectedIndex].getMenuOptionCount() + 1; // +1 for the Back option

        // Display the scrollable menu options on lines 1 to 3
        for (uint8_t i = 0; i < maxVisibleItems; i++) {
            uint8_t itemIndex = scrollOffset + i;
            lcd->setCursor(0, i + 1);  // Start at the second line (i + 1)

            if (itemIndex == subMenuIndex) {
                lcd->print(F("> "));
            } else {
                lcd->print(F("  "));
            }

            if (itemIndex < itemCount - 1) {
                channels[selectedIndex].getMenuOption(itemIndex, buffer, sizeof(buffer));
                lcd->print(buffer);
            } else {
                lcd->print(F("Back"));
            }
        }
    }

    void displayReadValue() {
        lcd->clear();
        lcd->setCursor(0, 0);
        lcd->print(channels[selectedIndex].getName());
        lcd->print(F(" : "));
        lcd->print(channels[selectedIndex].getValue());


        uint16_t value = channels[selectedIndex].getValue();  // Range: 0–255
        int8_t relativeValue = value - 127;  // Centered around 127
        int numBlocks = abs(relativeValue) / 25;  // Number of full blocks (0–5)
        int partialBlockLevel = (abs(relativeValue) % 25);  // Partial block level (0–24)

        lcd->setCursor(4, 1);
        lcd->print(F("["));

        for (int i = 0; i < 5; i++) {
            if (relativeValue < 0) {
                // Negative values - fill left side
                int leftIndex = 4 - i;  // Fill from the rightmost of the left side
                if (leftIndex < numBlocks) {
                    lcd->write(6);  // Full block (mirrored)
                } else if (leftIndex == numBlocks) {
                    lcd->write((partialBlockLevel >= 12) ? 5 : 4);  // Partial block (mirrored)
                } else {
                    lcd->write(0);  // Empty block
                }
            } else {
                lcd->write(0);  // Empty block for unused left side
            }
        }

        for (int i = 5; i < 10; i++) {
            if (relativeValue > 0) {
                // Positive values - fill right side
                int rightIndex = i - 5;  // Fill from the leftmost of the right side
                if (rightIndex < numBlocks) {
                    lcd->write(3);  // Full block
                } else if (rightIndex == numBlocks) {
                    lcd->write((partialBlockLevel >= 12) ? 2 : 1);  // Partial block
                } else {
                    lcd->write(0);  // Empty block
                }
            } else {
                lcd->write(0);  // Empty block for unused right side
            }
        }

        lcd->print(F("]"));
        lcd->setCursor(0, 3);
        lcd->print(F("> Back"));
    }

    void displaySelectDevice() {
        lcd->clear();
        lcd->setCursor(0, 0);
        lcd->print(F("Select Device"));
        lcd->setCursor(0, 1);
        lcd->print(F("To be implemented"));
        lcd->setCursor(0, 3);
        lcd->print(F("> Back"));
    }

    void displayCalibrate() {
        lcd->clear();
        lcd->setCursor(0, 0);
        lcd->print(F("Calibrate:"));
        lcd->setCursor(0, 1);
        lcd->print(F("Calibrating..."));
        lcd->setCursor(0, 3);
        lcd->print(F("> Back"));
    }

    void displayTrim() {
        lcd->clear();
        lcd->setCursor(0, 0);
        lcd->print(F("Trim Adjust:"));
        lcd->setCursor(0, 1);
        lcd->print(F("To be adjusted"));
        lcd->setCursor(0, 3);
        lcd->print(F("> Back"));
    }

    void updateEncoder(int8_t direction, bool buttonPressed) {
        unsigned long currentTime = millis();

        if (menuLevel == CHANNEL_LIST) {
            selectedIndex = (selectedIndex - direction + channelCount) % channelCount;
            updateChannelValues();
            updateChannelConfigs(selectedIndex);

            if (selectedIndex < scrollOffset) {
                scrollOffset = selectedIndex;
            } else if (selectedIndex >= scrollOffset + 4) {
                scrollOffset = selectedIndex - 4 + 1;
            }
        } else if (menuLevel == CHANNEL_SETTINGS) {
            uint8_t itemCount = channels[selectedIndex].getMenuOptionCount() + 1; // +1 for Back option
            subMenuIndex = (subMenuIndex - direction + itemCount) % itemCount;

            if (subMenuIndex < scrollOffset) {
                scrollOffset = subMenuIndex;
            } else if (subMenuIndex >= scrollOffset + maxVisibleItems) {
                scrollOffset = subMenuIndex - maxVisibleItems + 1;
            }
        }

        if (buttonPressed && (currentTime - lastButtonPressTime > buttonTimeout)) {
            lastButtonPressTime = currentTime;

            if (menuLevel == CHANNEL_LIST) {
                menuLevel = CHANNEL_SETTINGS;
                subMenuIndex = 0;
                scrollOffset = 0;
            } else if (menuLevel == CHANNEL_SETTINGS) {
                uint8_t itemCount = channels[selectedIndex].getMenuOptionCount();
                if (subMenuIndex < itemCount) {
                    menuLevel = channels[selectedIndex].configureItem(subMenuIndex);  // Transition to the appropriate submenu
                } else {
                    menuLevel = CHANNEL_LIST;  // Back button logic
                    subMenuIndex = 0;
                    scrollOffset = selectedIndex - (selectedIndex % maxVisibleItems); // Ensure the list scroll aligns
                }
            } else {
                menuLevel = CHANNEL_SETTINGS; // Back to settings menu
            }
        }

        displayMenu();
    }

    MenuLevel getMenuLevel() const {
        return menuLevel;
    }
};

#endif
