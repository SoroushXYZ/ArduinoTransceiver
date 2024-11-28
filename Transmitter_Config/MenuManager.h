#ifndef MENUMANAGER_H
#define MENUMANAGER_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include "Channel.h"

class MenuManager {
private:
    LiquidCrystal_I2C* lcd;
    Channel* channels;
    int channelCount;
    int selectedIndex; // The currently selected channel index
    int menuLevel;     // 0 = Channel List, 1 = Channel Settings
    int subMenuIndex;  // The index of the selected setting
    int scrollOffset;  // The topmost visible channel index

    static const int maxVisibleItems = 4; // Number of lines on the LCD

public:
    MenuManager(LiquidCrystal_I2C* lcd, Channel* channels, int count)
        : lcd(lcd), channels(channels), channelCount(count), selectedIndex(0),
          menuLevel(0), subMenuIndex(0), scrollOffset(0) {}

    void displayMenu() {
        lcd->clear();

        if (menuLevel == 0) {
            // Display channel list
            for (int i = 0; i < maxVisibleItems; i++) {
                int channelIndex = scrollOffset + i;
                if (channelIndex >= channelCount) break;

                lcd->setCursor(0, i);
                if (channelIndex == selectedIndex) {
                    lcd->print("> ");
                } else {
                    lcd->print("  ");
                }
                lcd->print(channels[channelIndex].getName());
            }
        } else if (menuLevel == 1) {
            // Display channel settings
            lcd->clear();
            lcd->setCursor(0, 0);
            lcd->print("Settings for:");
            lcd->setCursor(0, 1);
            lcd->print(channels[selectedIndex].getName());

            const char** items = channels[selectedIndex].getConfigurableItems();
            lcd->setCursor(0, 2);
            lcd->print("> ");
            lcd->print(items[subMenuIndex]);
        }
    }

    void updateEncoder(int direction, bool buttonPressed) {
        Serial.print("Menu Level: ");
        Serial.println(menuLevel); // Debug current menu level

        if (menuLevel == 0) {
            // Navigate channels
            selectedIndex = (selectedIndex + direction + channelCount) % channelCount;

            // Update scroll offset
            if (selectedIndex < scrollOffset) {
                scrollOffset = selectedIndex;
            } else if (selectedIndex >= scrollOffset + maxVisibleItems) {
                scrollOffset = selectedIndex - maxVisibleItems + 1;
            }
        } else if (menuLevel == 1) {
            // Navigate settings
            int itemCount = channels[selectedIndex].getConfigurableItemCount();
            subMenuIndex = (subMenuIndex + direction + itemCount) % itemCount;
        }

        if (buttonPressed) {
            if (menuLevel == 0) {
                Serial.println("Entering Channel Settings"); // Debug menu transition
                menuLevel = 1;
                subMenuIndex = 0; // Start at the first configurable item
            } else if (menuLevel == 1) {
                Serial.println("Configuring Item"); // Debug configuration action
                lcd->clear();
                lcd->setCursor(0, 0);
                lcd->print("HelloWorld!");
                channels[selectedIndex].configureItem(subMenuIndex);
                delay(2000); // Pause for feedback
                menuLevel = 0; // Return to channel list
            }
        }

        displayMenu();
    }

    // Getter for current menu level
    int getMenuLevel() const { return menuLevel; }
};

#endif
