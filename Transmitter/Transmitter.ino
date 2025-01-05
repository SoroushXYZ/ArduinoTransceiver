#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "Channel.h"
#include "InputHandler.h"
#include "DataDefinitions.h"
#include "ChannelLoader.h"
#include "CommunicationHandler.h"

CommunicationHandler cmnHandler(&channelValues, &radio);

void setup() {
    // Initialize radio
    radio.begin();
    radio.setAutoAck(false);
    radio.setDataRate(RF24_250KBPS);
    radio.openWritingPipe(my_radio_pipe);

    // Initialize Serial for debugging
    Serial.begin(9600);
    Serial.setTimeout(10);

    loadChannels();
}

void loop() {
    cmnHandler.loop();
}
