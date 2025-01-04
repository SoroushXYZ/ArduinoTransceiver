// Enum for Menu Levels
enum MenuLevel {
    CHANNEL_LIST,
    CHANNEL_SETTINGS,
    READ_VALUE,
    SELECT_DEVICE,
    CALIBRATE,
    TRIM,
};

// Channel configuration structure
struct ChannelConfig {
    uint8_t version;       // Configuration version
    char deviceType;       // Device type ('J', 'A', 'S', 'D')
    uint8_t deviceId;      // Device ID
    bool reverse;          // Channel reversing
    uint8_t trim;          // Trim adjustment
    int analogReadMin;     // Minimum analog read value
    int analogReadMax;     // Maximum analog read value
    uint8_t minEndpoint;   // Minimum endpoint
    uint8_t maxEndpoint;   // Maximum endpoint
    uint8_t centerPoint;   // Center point adjustment
};

struct DeviceOption {
    const char* displayName;  // Display name
    const char* shortCode;    // Short name for slave communication
};

const DeviceOption deviceOptions[] = {
    {"Joystick 1", "J1"},
    {"Joystick 2", "J2"},
    {"Joystick 3", "J3"},
    {"Joystick 4", "J4"},
    {"Analog 1", "A1"},
    {"Analog 2", "A2"},
    {"Analog 3", "A3"},
    {"Analog 4", "A4"},
    {"Switch 1", "S1"},
    {"Switch 2", "S2"},
    {"Digital 1", "D1"},
    {"Digital 2", "D2"},
    {"Digital 3", "D3"}
};

const uint8_t numDeviceOptions = sizeof(deviceOptions) / sizeof(deviceOptions[0]);  // 13 options