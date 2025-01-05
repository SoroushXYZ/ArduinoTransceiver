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

#define numDeviceOptions 13