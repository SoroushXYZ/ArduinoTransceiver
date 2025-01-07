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
            case REVERSE:
                displayReverse();
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
            case ENDPOINT:
                displayEndpoint();
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

        char buffer[20];
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

    void displayReverse() {
        lcd->clear();
        lcd->setCursor(0, 0);
        
        // Display the title "Reverse <channel name>"
        lcd->print(F("Reverse "));
        lcd->print(channels[selectedIndex].getName());

        // Options: "True", "False", "Back"
        const char* options[] = {"True", "False", "Back"};
        uint8_t optionCount = 3;  // Number of options

        // Display the scrollable options
        for (uint8_t i = 0; i < optionCount; i++) {
            lcd->setCursor(0, i + 1);  // Start at the second line (i + 1)
            if (subMenuIndex == i) {
                lcd->print(F("> "));  // Highlight the selected option
            } else {
                lcd->print(F("  "));  // Indent unselected options
            }
            lcd->print(options[i]);
        }
    }

    void displaySelectDevice() {
        lcd->clear();
        lcd->setCursor(0, 0);
        lcd->print(F("Select Device"));  // Static title line

        char typeBuffer[16];  // Buffer to hold the device type name

        for (uint8_t i = 0; i < maxVisibleItems; i++) {
            uint8_t optionIndex = scrollOffset + i;
            lcd->setCursor(0, i + 1);
            if (optionIndex == subMenuIndex) {
                lcd->print(F("> "));
            } else {
                lcd->print(F("  "));
            }

            // Check if it's the last option for "Back"
            if (optionIndex == numDeviceOptions) {
                lcd->print(F("Back"));
            } else if (optionIndex < numDeviceOptions) {
                uint8_t number;
                if (optionIndex < 4) {
                    strcpy_P(typeBuffer, (PGM_P)deviceTypeNames[0]);  // "Joystick"
                    number = optionIndex + 1;
                } else if (optionIndex < 8) {
                    strcpy_P(typeBuffer, (PGM_P)deviceTypeNames[1]);  // "Analog"
                    number = optionIndex - 3;
                } else if (optionIndex < 10) {
                    strcpy_P(typeBuffer, (PGM_P)deviceTypeNames[2]);  // "Switch"
                    number = optionIndex - 7;
                } else if (optionIndex < 13) {
                    strcpy_P(typeBuffer, (PGM_P)deviceTypeNames[3]);  // "Digital"
                    number = optionIndex - 9;
                } else {
                    strcpy_P(typeBuffer, (PGM_P)deviceTypeNames[4]);  // "Null" (fallback)
                    number = 0;
                }

                lcd->print(typeBuffer);  // Print type (e.g., "Joystick")
                lcd->print(number);      // Print number (e.g., "1")
            }
        }
    }

    void displayCalibrate() {
        lcd->clear();
        lcd->setCursor(0, 0);
        lcd->print(F("Calibrate:"));
        lcd->setCursor(0, 1);

        uint16_t analogValue = channels[selectedIndex].getAnalogValue();  // Current analog value
        uint16_t analogReadMin = channels[selectedIndex].analogReadMin;  // Min endpoint
        uint16_t analogReadMax = channels[selectedIndex].analogReadMax;  // Max endpoint
        uint16_t mean = (analogReadMin + analogReadMax) / 2;  // Zero point (mean)

        // Display endpoints and analog value
        lcd->print(F("Min:"));
        lcd->print(analogReadMin);
        lcd->print(F(" Max:"));
        lcd->print(analogReadMax);

        // Clear line 3 for displaying the blocks
        lcd->setCursor(0, 2);

        // Calculate the number of blocks based on the analog value
        int numBlocks = map(abs((int)analogValue - (int)mean), 0, (analogReadMax - analogReadMin) / 2, 0, 10);

        for (int i = 0; i < 20; i++) {
            if (analogValue > mean && i >= 10 && i < 10 + numBlocks) {
                lcd->write(3);  // Full block to the right
            } else if (analogValue < mean && i < 10 && i >= 10 - numBlocks) {
                lcd->write(3);  // Full block to the left
            } else {
                lcd->write(' ');  // Empty space
            }
        }

        lcd->setCursor(0, 3);
        lcd->print(F("> Back"));
    }

    void displayTrim() {
        lcd->clear();
        lcd->setCursor(0, 0);
        lcd->print(F("Trim Adjust:"));

        int8_t trimValue = channels[selectedIndex].trim;  // Current trim value (-127 to +127)

        // Display trim value at the center for feedback
        lcd->setCursor(0, 1);
        lcd->print(F("Trim: "));
        lcd->print(trimValue);

        // Line 3: Add "Back" option
        lcd->setCursor(0, 3);
        lcd->print(F("> Back"));
    }

    void displayEndpoint() {
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print(F("Endpoint Adjust:"));

    // Get the min and max endpoint values
    uint16_t minEndpoint = channels[selectedIndex].minEndpoint;
    uint16_t maxEndpoint = channels[selectedIndex].maxEndpoint;

    // Calculate the number of filled blocks (proportional to range)
    int range = maxEndpoint - minEndpoint;
    int blocksToFill = map(range, 0, 255, 0, 10);  // Map range to 10 blocks

    // Calculate left and right empty blocks
    int leftEmptyBlocks = (10 - blocksToFill) / 2;
    int rightEmptyBlocks = 10 - blocksToFill - leftEmptyBlocks;

    // Draw the progress bar
    lcd->setCursor(0, 1);
    lcd->print(F("["));

    for (int i = 0; i < 10; i++) {
        if (i < leftEmptyBlocks || i >= 10 - rightEmptyBlocks) {
            lcd->write(0);  // Empty block
        } else {
            lcd->write(3);  // Full block
        }
    }

    lcd->print(F("]"));

    // Display the current range
    lcd->setCursor(0, 2);
    lcd->print(F("Min:"));
    lcd->print(minEndpoint);
    lcd->print(F(" Max:"));
    lcd->print(maxEndpoint);

    // Add "Back" option
    lcd->setCursor(0, 3);
    lcd->print(F("> Back"));
}


void updateEncoder(int8_t direction, bool buttonPressed) {
    unsigned long currentTime = millis();

    switch (menuLevel) {
        case CHANNEL_LIST: {
            selectedIndex = (selectedIndex - direction + channelCount) % channelCount;

            if (selectedIndex < scrollOffset) {
                scrollOffset = selectedIndex;
            } else if (selectedIndex >= scrollOffset + maxVisibleItems) {
                scrollOffset = selectedIndex - maxVisibleItems + 1;
            }

            if (buttonPressed && (currentTime - lastButtonPressTime > buttonTimeout)) {
                lastButtonPressTime = currentTime;
                menuLevel = CHANNEL_SETTINGS;  // Move to CHANNEL_SETTINGS
                loadChannelSettings(selectedIndex);
                subMenuIndex = 0;
                scrollOffset = 0;  // Reset to top
            }
            break;
        }

        case CHANNEL_SETTINGS: {
            uint8_t itemCount = channels[selectedIndex].getMenuOptionCount() + 1;  // +1 for Back option
            subMenuIndex = (subMenuIndex - direction + itemCount) % itemCount;

            if (subMenuIndex < scrollOffset) {
                scrollOffset = subMenuIndex;
            } else if (subMenuIndex >= scrollOffset + maxVisibleItems) {
                scrollOffset = subMenuIndex - maxVisibleItems + 1;
            }

            if (buttonPressed && (currentTime - lastButtonPressTime > buttonTimeout)) {
                lastButtonPressTime = currentTime;

                if (subMenuIndex == itemCount - 1) {
                    // "Back" selected
                    menuLevel = CHANNEL_LIST;
                    loadChannelSettings(selectedIndex);
                    subMenuIndex = 0;
                    scrollOffset = selectedIndex - (selectedIndex % maxVisibleItems);  // Align with the selected index
                } else if (channels[selectedIndex].configureItem(subMenuIndex) == SELECT_DEVICE) {
                    menuLevel = SELECT_DEVICE;
                    subMenuIndex = 0;  // Reset to start at Joystick1
                    scrollOffset = 0;  // Reset scroll to the top
                } else {
                    menuLevel = channels[selectedIndex].configureItem(subMenuIndex);
                }
            }
            break;
        }

        case READ_VALUE: {
            if (buttonPressed && (currentTime - lastButtonPressTime > buttonTimeout)) {
                lastButtonPressTime = currentTime;
                menuLevel = CHANNEL_SETTINGS;  // Go back to CHANNEL_SETTINGS
                loadChannelSettings(selectedIndex);
                subMenuIndex = 0;
                scrollOffset = 0;
            }
            break;
        }

        case REVERSE: {
            uint8_t optionCount = 3;  // "True", "False", "Back"
            subMenuIndex = (subMenuIndex - direction + optionCount) % optionCount;

            if (buttonPressed && (currentTime - lastButtonPressTime > buttonTimeout)) {
                lastButtonPressTime = currentTime;

                switch (subMenuIndex) {
                    case 0:  // "True" selected
                        if(!channels[selectedIndex].reverse){
                          reverseChannel(selectedIndex);
                        }
                        break;

                    case 1:  // "False" selected
                        if(channels[selectedIndex].reverse){
                          reverseChannel(selectedIndex);
                        }
                        break;
                }
                delay(100);
                menuLevel = CHANNEL_SETTINGS;  // Go back to settings menu
                loadChannelSettings(selectedIndex);
                subMenuIndex = 0;
                scrollOffset = 0;
            }
            break;
        }

        case SELECT_DEVICE: {
            uint8_t itemCount = numDeviceOptions + 1;  // +1 for Back option
            subMenuIndex = (subMenuIndex - direction + itemCount) % itemCount;

            if (subMenuIndex < scrollOffset) {
                scrollOffset = subMenuIndex;
            } else if (subMenuIndex >= scrollOffset + maxVisibleItems) {
                scrollOffset = subMenuIndex - maxVisibleItems + 1;
            }

            if (buttonPressed && (currentTime - lastButtonPressTime > buttonTimeout)) {
                lastButtonPressTime = currentTime;

                if (subMenuIndex == itemCount - 1) {
                    // "Back" selected
                    loadChannelSettings(selectedIndex);
                    subMenuIndex = 0;
                    scrollOffset = 0;
                } else {
                    selectDevice(selectedIndex, subMenuIndex);
                    delay(100);
                    loadChannelSettings(selectedIndex);
                    subMenuIndex = 0;
                    scrollOffset = 0;
                    // Serial.print(F("Selected Device Option: "));
                    // Serial.println(subMenuIndex);
                    // Handle the device selection here
                }
                menuLevel = CHANNEL_SETTINGS;
            }
            break;
        }

        case CALIBRATE: {
            if (buttonPressed && (currentTime - lastButtonPressTime > buttonTimeout)) {
                lastButtonPressTime = currentTime;
                sendCalibrationData(selectedIndex);
                delay(100);
                menuLevel = CHANNEL_SETTINGS;  // Go back to CHANNEL_SETTINGS
                loadChannelSettings(selectedIndex);
                subMenuIndex = 0;
                scrollOffset = 0;  // Reset scroll position
            }
            break;
        }
        case TRIM: {
            // Increment or decrement the trim value
            channels[selectedIndex].trim += direction;

            // Clamp trim value between -127 and +127
            if (channels[selectedIndex].trim > 127) channels[selectedIndex].trim = 127;
            if (channels[selectedIndex].trim < -127) channels[selectedIndex].trim = -127;

            if (buttonPressed && (currentTime - lastButtonPressTime > buttonTimeout)) {
                lastButtonPressTime = currentTime;
                sendTrim(selectedIndex);
                delay(100);
                menuLevel = CHANNEL_SETTINGS;  // Go back to CHANNEL_SETTINGS
                loadChannelSettings(selectedIndex);
                subMenuIndex = 0;
                scrollOffset = 0;  // Reset scroll position
            }
            break;
        }
        case ENDPOINT: {
            // Adjust both endpoints simultaneously
            if(channels[selectedIndex].minEndpoint + direction > 125){
              return;
            }
            channels[selectedIndex].minEndpoint += direction;
            channels[selectedIndex].maxEndpoint -= direction;

            // Clamp min and max values between 0 and 255
            if (channels[selectedIndex].minEndpoint < 0) channels[selectedIndex].minEndpoint = 0;
            if (channels[selectedIndex].maxEndpoint > 255) channels[selectedIndex].maxEndpoint = 255;
            if (channels[selectedIndex].minEndpoint >= channels[selectedIndex].maxEndpoint) {
                channels[selectedIndex].maxEndpoint = channels[selectedIndex].maxEndpoint - 1;
            }

            // Refresh the display
            displayMenu();

            // Handle button press to go back to the settings menu
            if (buttonPressed && (currentTime - lastButtonPressTime > buttonTimeout)) {
                lastButtonPressTime = currentTime;
                sendEndpoints(selectedIndex);
                delay(100);
                menuLevel = CHANNEL_SETTINGS;  // Go back to CHANNEL_SETTINGS
                loadChannelSettings(selectedIndex);
                subMenuIndex = 0;
                scrollOffset = 0;  // Reset scroll position
            }
            break;
        }

        default:
            break;
    }

    displayMenu();  // Refresh the display after every update
}


    MenuLevel getMenuLevel() const {
        return menuLevel;
    }

    uint8_t getSelectedIndex() const {
        return selectedIndex;
    }

    void loadChannelSettings(int channelIndex){
      updateChannelValues();
      updateChannelConfigs(channelIndex);
    }
};

#endif
