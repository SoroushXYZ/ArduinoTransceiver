void loadChannels(){
    // Load channel configurations
    inputHandler.loadFromEEPROM();

    // Configure channels dynamically
    for (int i = 0; i < 10; ++i) {
      ChannelConfig& config = inputHandler.channels[i];
      DeviceType deviceType = (DeviceType)config.deviceType;

      // Set input type based on device type
      switch (deviceType) {
          case JOYSTICK:
          case ANALOG_INPUT:
              channels[i].setInputType(ANALOG);
              break;
          case DIGITAL_INPUT:
              channels[i].setInputType(DIGITAL);
              break;
          case THREE_STATE_SWITCH:
              channels[i].setInputType(THREE_STATE);
              break;
          default:
              channels[i].setInputType(ANALOG); // Default to ANALOG
              break;
      }

      // Assign default pin values from predefined devices
      const PredefinedDevice* device = inputHandler.getPredefinedDevice(config.deviceType, config.deviceId);
      if (device != nullptr) {
          // Setup pins based on device requirements
          if (deviceType == THREE_STATE_SWITCH) {
              channels[i].setPin(device->primaryPin, device->secondaryPin); // Requires two pins
          } else {
              channels[i].setPin(device->primaryPin); // Requires one pin
          }
      } else {
          // Default pin setup if no predefined device
          if (deviceType == THREE_STATE_SWITCH) {
              channels[i].setPin(A0 + (i * 2), A0 + (i * 2) + 1); // Two pins for three-state switch
          } else {
              channels[i].setPin(A0 + i); // One pin for other types
          }
      }

      // Set other properties from the config
      channels[i].setAnalogReadMin(config.analogReadMin);
      channels[i].setAnalogReadMax(config.analogReadMax);
      channels[i].setTrim(config.trim);
      channels[i].setMinEndpoint(config.minEndpoint);
      channels[i].setMaxEndpoint(config.maxEndpoint);

    }
}