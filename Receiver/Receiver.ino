#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>  // To create PWM signals we need this library

const uint64_t pipeIn = 0xE8E8F0F0E1LL;     // Remember that this code is the same as in the transmitter
RF24 radio(9, 10);  // CSN and CE pins

// The size of this struct should not exceed 32 bytes
struct Received_data {
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

Received_data received_data;

Servo channel_1;
Servo channel_2;
Servo channel_3;
Servo channel_4;
Servo channel_5;
Servo channel_6;
Servo channel_7;
Servo channel_8;
Servo channel_9;
Servo channel_10;

int ch1_value = 0;
int ch2_value = 0;
int ch3_value = 0;
int ch4_value = 0;
int ch5_value = 0;
int ch6_value = 0;
int ch7_value = 0;
int ch8_value = 0;
int ch9_value = 0;
int ch10_value = 0;

void reset_the_Data() 
{
  // 'Safe' values to use when NO radio input is detected
  received_data.ch1 = 0;      // Throttle (channel 1) to 0
  received_data.ch2 = 127;
  received_data.ch3 = 127;
  received_data.ch4 = 127;
  received_data.ch5 = 127;
  received_data.ch6 = 0;
  received_data.ch7 = 0;
  received_data.ch8 = 0;
  received_data.ch9 = 0;
  received_data.ch10 = 0;
}

/**************************************************/

void setup()
{
  // Attach the servo signal on pins from D2 to D11
  channel_1.attach(2);
  channel_2.attach(3);
  channel_3.attach(4);
  channel_4.attach(5);
  channel_5.attach(6);
  channel_6.attach(7);
  channel_7.attach(8);
  channel_8.attach(A3);
  channel_9.attach(A4);
  channel_10.attach(A5);
  
  // Reset the received values
  reset_the_Data();

  // Begin radio communication and configuration
  radio.begin();
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);  
  radio.openReadingPipe(1, pipeIn);
  
  // Start listening for incoming radio signals
  radio.startListening();
}

/**************************************************/

unsigned long lastRecvTime = 0;

// We create the function that will read the data each certain time
void receive_the_data()
{
  while (radio.available()) {
    radio.read(&received_data, sizeof(Received_data));
    lastRecvTime = millis(); // Here we receive the data
  }
}

/**************************************************/

void loop()
{
  // Receive the radio data
  receive_the_data();

  // Reset data if signal is lost for 1 second
  unsigned long now = millis();
  if (now - lastRecvTime > 1000) {
    // Signal lost?
    reset_the_Data();
    // Go up and change the initial values if you want depending on
    // your applications. Put 0 for throttle in case of drones so it won't
    // fly away
  } 

  // Map the received data to PWM range (1000-2000 microseconds)
  ch1_value = map(received_data.ch1, 0, 255, 1000, 2000);
  ch2_value = map(received_data.ch2, 0, 255, 1000, 2000);
  ch3_value = map(received_data.ch3, 0, 255, 1000, 2000);
  ch4_value = map(received_data.ch4, 0, 255, 1000, 2000);
  ch5_value = map(received_data.ch5, 0, 255, 1000, 2000);
  ch6_value = map(received_data.ch6, 0, 255, 1000, 2000);
  ch7_value = map(received_data.ch7, 0, 255, 1000, 2000);
  ch8_value = map(received_data.ch8, 0, 255, 1000, 2000);
  ch9_value = map(received_data.ch9, 0, 255, 1000, 2000);
  ch10_value = map(received_data.ch10, 0, 255, 1000, 2000);

  // Create the PWM signals
  channel_1.writeMicroseconds(ch1_value);  
  channel_2.writeMicroseconds(ch2_value);  
  channel_3.writeMicroseconds(ch3_value);  
  channel_4.writeMicroseconds(ch4_value);  
  channel_5.writeMicroseconds(ch5_value);  
  channel_6.writeMicroseconds(ch6_value);  
  channel_7.writeMicroseconds(ch7_value);  
  channel_8.writeMicroseconds(ch8_value);  
  channel_9.writeMicroseconds(ch9_value);  
  channel_10.writeMicroseconds(ch10_value);  
  
} // Loop end
