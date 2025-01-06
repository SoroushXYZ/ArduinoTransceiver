#ifndef INPUT_HANDLER_H
#define INPUT_HANDLER_H

#include <EEPROM.h>

// Max channels and constants
const int MAX_CHANNELS = 10;
const int CHANNEL_BLOCK_SIZE = 100;
const int EEPROM_START_ADDRESS = 0;
const uint8_t CONFIG_VERSION = 2; // Increment this when structure changes

// Device types
enum DeviceType : uint8_t {
    JOYSTICK = 'J',
    ANALOG_INPUT = 'A',
    THREE_STATE_SWITCH = 'S',
    DIGITAL_INPUT = 'D',
    INVALID = 0xFF  // To handle invalid device types
};

// Predefined device mappings
struct PredefinedDevice {
    char type;
    uint8_t id;
    int primaryPin;
    int secondaryPin;
};

const PredefinedDevice predefinedDevices[] = {
    {'J', 1, A4, -1}, {'J', 2, A5, -1}, {'J', 3, A6, -1}, {'J', 4, A7, -1},
    {'A', 1, A0, -1}, {'A', 2, A1, -1}, {'A', 3, A2, -1}, {'A', 4, A3, -1},
    {'S', 1, 2, 3}, {'S', 2, 4, 5},
    {'D', 1, 6, -1}, {'D', 2, 7, -1}, {'D', 3, 8, -1}
};

// Channel configuration structure
struct ChannelConfig {
    uint8_t version;       // Configuration version
    char deviceType;       // Device type ('J', 'A', 'S', 'D')
    uint8_t deviceId;      // Device ID
    bool reverse;          // Channel reversing
    int8_t trim;              // Trim adjustment
    int analogReadMin;     // Minimum analog read value
    int analogReadMax;     // Maximum analog read value
    uint8_t minEndpoint;       // Minimum endpoint
    uint8_t maxEndpoint;       // Maximum endpoint
    uint8_t centerPoint;       // Center point adjustment
};

// InputHandler Class
class InputHandler {
public:
    ChannelConfig channels[MAX_CHANNELS];

    // Load from EEPROM with validation
    void loadFromEEPROM() {
        for (int i = 0; i < MAX_CHANNELS; ++i) {
            int address = EEPROM_START_ADDRESS + i * CHANNEL_BLOCK_SIZE;
            ChannelConfig tempConfig;
            EEPROM.get(address, tempConfig);

            // Validate version and device type
            if (tempConfig.version != CONFIG_VERSION || !isValidDeviceType(tempConfig.deviceType)) {
                initializeChannel(i, INVALID, 0); // Reset to default
            } else {
                channels[i] = tempConfig;
            }
        }
    }

    // Save to EEPROM
    void saveToEEPROM() {
        for (int i = 0; i < MAX_CHANNELS; ++i) {
            int address = EEPROM_START_ADDRESS + i * CHANNEL_BLOCK_SIZE;
            EEPROM.put(address, channels[i]);
        }
    }

    // Initialize a channel with defaults
    void initializeChannel(int index, char deviceType, uint8_t deviceId) {
        if (index < 0 || index >= MAX_CHANNELS) return;

        channels[index] = {
            CONFIG_VERSION,
            deviceType,
            deviceId,
            false, // Reverse
            0,     // Trim
            0,     // Analog Min
            1023,  // Analog Max
            0,     // Min Endpoint
            255,   // Max Endpoint
            127    // Center Point
        };
    }

    // Validate device type
    bool isValidDeviceType(char deviceType) {
        return deviceType == JOYSTICK || deviceType == ANALOG_INPUT ||
               deviceType == THREE_STATE_SWITCH || deviceType == DIGITAL_INPUT;
    }

    // Get predefined device mapping
    const PredefinedDevice* getPredefinedDevice(char type, uint8_t id) {
        for (const auto& device : predefinedDevices) {
            if (device.type == type && device.id == id) return &device;
        }
        return nullptr;
    }

    void printChannelConfig(int index) {
        if (index < 0 || index >= MAX_CHANNELS) return;

        ChannelConfig& config = channels[index];
        Serial.print("Channel ");
        Serial.print(index);
        Serial.print(": Type=");
        Serial.print(config.deviceType);
        Serial.print(config.deviceId);
        Serial.print(", Reverse=");
        Serial.println(config.reverse ? "Yes" : "No");
    }
};

#endif
