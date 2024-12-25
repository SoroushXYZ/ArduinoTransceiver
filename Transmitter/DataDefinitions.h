#ifndef DATA_DEFINITIONS_H
#define DATA_DEFINITIONS_H

const uint64_t my_radio_pipe = 0xE8E8F0F0E1LL; // Remember that this code should be the same for the receiver

RF24 radio(9, 10);  // Set CE and CSN pins

// Input handler and channel array
InputHandler inputHandler;
Channel channels[10];

// The sizeof this struct should not exceed 32 bytes
struct ChannelValues {
    byte ch1;
    byte ch2;
    byte ch3;
    byte ch4;
    byte ch5;
    byte ch6;
    byte ch7;
    byte ch8;
    byte ch9;
    byte ch10;
};

// Create a variable with the structure above and name it sent_data
ChannelValues channelValues;

#endif // DATA_DEFINITIONS_H
