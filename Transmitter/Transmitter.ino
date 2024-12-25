#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include "Channel.h"

const uint64_t my_radio_pipe = 0xE8E8F0F0E1LL; //Remember that this code should be the same for the receiver

RF24 radio(9, 10);  //Set CE and CSN pins

// The sizeof this struct should not exceed 32 bytes
struct Data_to_be_sent {
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

//Create a variable with the structure above and name it sent_data
Data_to_be_sent sent_data;

Channel channels[10];

void setup()
{
  radio.begin();
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(my_radio_pipe);

  //Reset each channel value
  sent_data.ch1 = 127;
  sent_data.ch2 = 127;
  sent_data.ch3 = 127;
  sent_data.ch4 = 127;
  sent_data.ch5 = 0;
  sent_data.ch6 = 0;
  sent_data.ch7 = 0;
  sent_data.ch8 = 127;
  sent_data.ch9 = 127;
  sent_data.ch10 = 0;

  // Define channels and their corresponding input pins
  channels[0].setPin(A4);
  channels[0].setInputType(ANALOG);
  channels[0].setReverse(true);

}

/**************************************************/

void loop()
{
  /*If your channel is reversed, just swap 0 to 255 by 255 to 0 below
  EXAMPLE:
  Normal:    data.ch1 = map( analogRead(A0), 0, 1024, 0, 255);
  Reversed:  data.ch1 = map( analogRead(A0), 0, 1024, 255, 0);  */
  
  sent_data.ch1 = channels[0].read();
  sent_data.ch2 = map(analogRead(A5), 0, 1024, 0, 255);
  sent_data.ch3 = map(analogRead(A6), 0, 1024, 0, 255);
  sent_data.ch4 = map(analogRead(A7), 0, 1024, 0, 255);
  
  // Channels 4 and 5, now mapped to 0 and 255
  sent_data.ch5 = digitalRead(2) == HIGH ? 255 : 0;
  sent_data.ch6 = digitalRead(3) == HIGH ? 255 : 0;
  
  sent_data.ch7 = map(analogRead(A3), 0, 1024, 0, 255);
  sent_data.ch8 = map(analogRead(A0), 0, 1024, 0, 255);
  sent_data.ch9 = map(analogRead(A1), 0, 1024, 0, 255);
  sent_data.ch10 = map(analogRead(A2), 0, 1024, 0, 255);

  // Send the updated data
  radio.write(&sent_data, sizeof(Data_to_be_sent));
}
