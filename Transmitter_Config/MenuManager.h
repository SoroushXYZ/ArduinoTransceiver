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
    int channelCount;
    int selectedIndex;  // The currently selected channel index
    MenuLevel menuLevel; // Enum to track the current menu level
    int subMenuIndex;   // The index of the selected setting
    int scrollOffset;   // The topmost visible channel index

    static const int maxVisibleItems = 4; // Number of lines on the LCD

public:
    MenuManager(LiquidCrystal_I2C* lcd, Channel* channels, int count)
        : lcd(lcd), channels(channels), channelCount(count), selectedIndex(0),
          menuLevel(CHANNEL_LIST), subMenuIndex(0), scrollOffset(0) {}

    void displayMenu() {
        lcd->clear();

        if (menuLevel == CHANNEL_LIST) {
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
        } else if (menuLevel == CHANNEL_SETTINGS) {
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
        if (menuLevel == CHANNEL_LIST) {
            // Navigate channels
            selectedIndex = (selectedIndex + direction + channelCount) % channelCount;

            // Update scroll offset
            if (selectedIndex < scrollOffset) {
                scrollOffset = selectedIndex;
            } else if (selectedIndex >= scrollOffset + maxVisibleItems) {
                scrollOffset = selectedIndex - maxVisibleItems + 1;
            }
        } else if (menuLevel == CHANNEL_SETTINGS) {
            // Navigate settings
            int itemCount = channels[selectedIndex].getConfigurableItemCount();
            subMenuIndex = (subMenuIndex + direction + itemCount) % itemCount;
        }

        if (buttonPressed) {
            if (menuLevel == CHANNEL_LIST) {
                menuLevel = CHANNEL_SETTINGS; // Enter settings
                subMenuIndex = 0;
            } else if (menuLevel == CHANNEL_SETTINGS) {
                lcd->clear();
                lcd->setCursor(0, 0);
                lcd->print("HelloWorld!");
                channels[selectedIndex].configureItem(subMenuIndex);
                delay(2000);
                menuLevel = CHANNEL_LIST; // Return to channel list
            }
        }

        displayMenu();
    }
};

#endif
