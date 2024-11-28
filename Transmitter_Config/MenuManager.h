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
    int scrollOffset;   // The topmost visible item index in settings

    static const int maxVisibleItems = 3; // Number of lines for the scroll menu

public:
    MenuManager(LiquidCrystal_I2C* lcd, Channel* channels, int count)
        : lcd(lcd), channels(channels), channelCount(count), selectedIndex(0),
          menuLevel(CHANNEL_LIST), subMenuIndex(0), scrollOffset(0) {}

    void displayMenu() {
        lcd->clear();

        if (menuLevel == CHANNEL_LIST) {
            // Display channel list
            for (int i = 0; i < 4; i++) {
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
            // Display channel number on top
            lcd->setCursor(0, 0);
            lcd->print(channels[selectedIndex].getName());

            // Display scrollable settings menu
            const char** items = channels[selectedIndex].getConfigurableItems();
            int itemCount = channels[selectedIndex].getConfigurableItemCount();

            for (int i = 0; i < maxVisibleItems; i++) {
                int itemIndex = scrollOffset + i;
                if (itemIndex >= itemCount + 2) break;

                lcd->setCursor(0, i + 1); // Start from the second row
                if (itemIndex == subMenuIndex) {
                    lcd->print("> ");
                } else {
                    lcd->print("  ");
                }
                if (itemIndex < itemCount) {
                    lcd->print(items[itemIndex]);
                } else if (itemIndex == itemCount) {
                    lcd->print("Reset to Default");
                } else {
                    lcd->print("Back");
                }
            }
        }
    }

    void updateEncoder(int direction, bool buttonPressed) {
        if (menuLevel == CHANNEL_LIST) {
            // Navigate channels
            selectedIndex = (selectedIndex + direction + channelCount) % channelCount;

            // Update scroll offset
            if (selectedIndex < scrollOffset) {
                scrollOffset = selectedIndex;
            } else if (selectedIndex >= scrollOffset + 4) {
                scrollOffset = selectedIndex - 4 + 1;
            }
        } else if (menuLevel == CHANNEL_SETTINGS) {
            // Navigate settings
            int itemCount = channels[selectedIndex].getConfigurableItemCount() + 2; // Add Reset and Back
            subMenuIndex = (subMenuIndex + direction + itemCount) % itemCount;

            // Update scroll offset
            if (subMenuIndex < scrollOffset) {
                scrollOffset = subMenuIndex;
            } else if (subMenuIndex >= scrollOffset + maxVisibleItems) {
                scrollOffset = subMenuIndex - maxVisibleItems + 1;
            }
        }

        if (buttonPressed) {
            if (menuLevel == CHANNEL_LIST) {
                menuLevel = CHANNEL_SETTINGS; // Enter settings
                subMenuIndex = 0;
                scrollOffset = 0;
            } else if (menuLevel == CHANNEL_SETTINGS) {
                int itemCount = channels[selectedIndex].getConfigurableItemCount();
                if (subMenuIndex < itemCount) {
                    channels[selectedIndex].configureItem(subMenuIndex);
                } else if (subMenuIndex == itemCount) {
                    channels[selectedIndex].resetToDefault();
                } else {
                    menuLevel = CHANNEL_LIST; // Back to channel list
                    scrollOffset = 0;
                }
            }
        }

        displayMenu();
    }
};

#endif
