#ifndef TIMEDUPDATEHANDLER_H
#define TIMEDUPDATEHANDLER_H

#include <Arduino.h>
#include "MenuManager.h"

// Global variable to track the last update time
unsigned long lastUpdateTime = 0;

// Function to handle timed updates
void handleTimedUpdates(MenuManager& menu) {
    unsigned long currentTime = millis();

    // Check if 100 ms have passed since the last update
    if (currentTime - lastUpdateTime >= 200) {
        switch(menu.getMenuLevel()){
          case READ_VALUE:
              updateChannelValues();
              menu.displayMenu();  // Update the display
              break;
          // case CHANNEL_LIST:
          //     displayChannelList();
          //     break;
          // case CHANNEL_SETTINGS:
          //     displayChannelSettings();
          //     break;
          // case SELECT_DEVICE:
          //     displaySelectDevice();
          //     break;
          case CALIBRATE:
              updateAnalogValue(menu.getSelectedIndex());
              channels[menu.getSelectedIndex()].calibrationLoop();
              menu.displayCalibrate();
              break;
          // case TRIM:
          //     displayTrim();
          //     break;
        }
        lastUpdateTime = currentTime;  // Reset the last update time
    }
}

#endif
