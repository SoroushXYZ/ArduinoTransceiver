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
    unsigned long lastButtonPressTime; // Track the last button press time
    static const unsigned long buttonTimeout = 500; // Timeout for button press in ms

    static const int maxVisibleItems = 3; // Number of lines for the scroll menu

public:
    MenuManager(LiquidCrystal_I2C* lcd, Channel* channels, int count)
        : lcd(lcd), channels(channels), channelCount(count), selectedIndex(0),
          menuLevel(CHANNEL_LIST), subMenuIndex(0), scrollOffset(0), lastButtonPressTime(0) {}

    void displayMenu() {
        lcd->clear();

        if (menuLevel == CHANNEL_LIST) {
            updateChannelValues();
            // Display channel list
            for (int i = 0; i < 4; i++) {
                int channelIndex = scrollOffset + i;
                if (channelIndex >= channelCount) break;

                lcd->setCursor(0, i);
                if (channelIndex == selectedIndex) {
                    lcd->print(F("> "));
                } else {
                    lcd->print(F("  "));
                }
                lcd->print(channels[channelIndex].getName()); // Use char[] name directly
                lcd->print(" ");
                lcd->print(channels[channelIndex].getValue());
            }
        } else if (menuLevel == CHANNEL_SETTINGS) {
            // Display channel name on top
            lcd->setCursor(0, 0);
            lcd->print(channels[selectedIndex].getName()); // Use char[] name directly

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
                    lcd->print(F("Reset to Default"));
                } else {
                    lcd->print(F("Back"));
                }
            }
        }
    }

    void updateEncoder(int direction, bool buttonPressed) {
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
            int itemCount = channels[selectedIndex].getConfigurableItemCount() + 2; // Add Reset and Back
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
                int itemCount = channels[selectedIndex].getConfigurableItemCount();
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

    void getMenuLevel(){
      return menuLevel;
    }
};

#endif
