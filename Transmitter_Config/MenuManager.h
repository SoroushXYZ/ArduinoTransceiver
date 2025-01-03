#ifndef MENUMANAGER_H
#define MENUMANAGER_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "Channel.h"

// Enum for Menu Levels
enum MenuLevel {
    CHANNEL_LIST,
    CHANNEL_SETTINGS
};

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

    static const uint8_t maxVisibleItems = 3; // Number of lines for the scroll menu

public:
    MenuManager(LiquidCrystal_I2C* lcd, Channel* channels, uint8_t count)
        : lcd(lcd), channels(channels), channelCount(count), selectedIndex(0),
          menuLevel(CHANNEL_LIST), subMenuIndex(0), scrollOffset(0), lastButtonPressTime(0) {}

void displayMenu() {
    lcd->clear();

    if (menuLevel == CHANNEL_LIST) {
        // Display channel list
        for (uint8_t i = 0; i < 4; i++) {
            uint8_t channelIndex = scrollOffset + i;
            if (channelIndex >= channelCount) break;

            lcd->setCursor(0, i);

            // Display cursor for the selected channel
            lcd->print(channelIndex == selectedIndex ? F("> ") : F("  "));

            // Display the channel name
            lcd->print(channels[channelIndex].getName());
        }
    } else if (menuLevel == CHANNEL_SETTINGS) {
        // Display channel name on top
        lcd->setCursor(0, 0);
        lcd->print(channels[selectedIndex].getName());

        // Display scrollable settings menu
        char buffer[16];
        for (uint8_t i = 0; i < maxVisibleItems; i++) {
            uint8_t itemIndex = scrollOffset + i;
            lcd->setCursor(0, i + 1);
            lcd->print(itemIndex == subMenuIndex ? F("> ") : F("  "));

            if (itemIndex < channels[selectedIndex].getConfigurableItemCount()) {
                channels[selectedIndex].getConfigurableItem(itemIndex, buffer, sizeof(buffer));
                lcd->print(buffer);
            } else if (itemIndex == channels[selectedIndex].getConfigurableItemCount()) {
                lcd->print(F("Reset to Default"));
            } else {
                lcd->print(F("Back"));
            }
        }
    }
}




    void updateEncoder(int8_t direction, bool buttonPressed) {
        unsigned long currentTime = millis();

        if (menuLevel == CHANNEL_LIST) {
            // Navigate channels (reverse scroll direction)
            selectedIndex = (selectedIndex - direction + channelCount) % channelCount;

            // Update scroll offset
            if (selectedIndex < scrollOffset) {
                scrollOffset = selectedIndex;
            } else if (selectedIndex >= scrollOffset + 4) {
                scrollOffset = selectedIndex - 4 + 1;
            }
        } else if (menuLevel == CHANNEL_SETTINGS) {
            // Navigate settings (reverse scroll direction)
            uint8_t itemCount = channels[selectedIndex].getConfigurableItemCount() + 2; // Add Reset and Back
            subMenuIndex = (subMenuIndex - direction + itemCount) % itemCount;

            // Update scroll offset
            if (subMenuIndex < scrollOffset) {
                scrollOffset = subMenuIndex;
            } else if (subMenuIndex >= scrollOffset + maxVisibleItems) {
                scrollOffset = subMenuIndex - maxVisibleItems + 1;
            }
        }

        if (buttonPressed && (currentTime - lastButtonPressTime > buttonTimeout)) {
            lastButtonPressTime = currentTime; // Update the last button press time

            if (menuLevel == CHANNEL_LIST) {
                menuLevel = CHANNEL_SETTINGS; // Enter settings
                subMenuIndex = 0;
                scrollOffset = 0;
            } else if (menuLevel == CHANNEL_SETTINGS) {
                uint8_t itemCount = channels[selectedIndex].getConfigurableItemCount();
                if (subMenuIndex < itemCount) {
                    channels[selectedIndex].configureItem(subMenuIndex);
                } else if (subMenuIndex == itemCount) {
                    channels[selectedIndex].resetToDefault();
                } else {
                    // Back button logic: transition to CHANNEL_LIST
                    menuLevel = CHANNEL_LIST;
                    subMenuIndex = 0;
                    scrollOffset = selectedIndex - (selectedIndex % 4); // Ensure the list scroll aligns
                }
            }
        }

        displayMenu();
    }

    MenuLevel getMenuLevel() const {
        return menuLevel;
    }
};

#endif
