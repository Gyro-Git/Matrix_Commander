#include "Arduino.h"
#include <SPI.h>
#include <RF24.h>
#include "HID-Project.h"
#include <EEPROM.h>

// This is just the way the RF24 library works:
// Hardware configuration: Set up nRF24L01 radio on SPI bus (pins 10, 11, 12, 13) plus pins 7 & 8
RF24 radio(9, 10);

byte addresses[][6] = {"1Node", "2Node"};
byte currentConf[128];

void setup()
{
  Keyboard.begin();
  Consumer.begin();
  if (!radio.begin()) //if radio can't be found turn on all LEDs and do nothing.
  {
    while (1); // Wait here forever. MUST BE CHANGED!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    Keyboard.print("Dongle is broken");
  }
  radio.setPALevel(RF24_PA_MAX);  //adjustable power level
  radio.setDataRate(RF24_250KBPS); //slowest speed for higher range
  radio.setChannel(124);
  radio.setPayloadSize(1);

  radio.openWritingPipe(getPipe(1));
  radio.openReadingPipe(1, getPipe(0));
  pinMode(8, OUTPUT);
  radio.startListening();
  //Serial.begin(115200);
  loadConfig();
}

void loop() {
  radio.startListening();
  // This is what we receive from the other device (the transmitter)
  byte data;
  byte confirmation = 0;

  // Is there any data for us to get?
  if ( radio.available()) {

    // Go and read the data and put it into that variable
    while (radio.available()) {
      radio.read( &data, sizeof(byte));

      if (data == 8) {    //check config
        bool isMatch = checkConfig();
        if (isMatch) {}//Serial.println("Config matches");
        else {
          //Serial.println("Config doesn't match");
          updateRemote();
        }
      }
      else {   //press button as HID after confirmation message
        byte confirmation = 0;
        unsigned long startTime = millis();
        while (millis() - startTime < 10) {
          if ( radio.available()) {
            while (radio.available()) {
              radio.read( &confirmation, sizeof(byte));
              //Serial.println(confirmation);
            }
            if (confirmation == 249) HIDaction(data);
          }
        }
      }
      data = 0;

    }
  }
}

bool checkConfig() {
  //Serial.println("checking configuration");
  //Keyboard.print("checking configuration");
  unsigned long startedWaitingAt = millis();
  int count = 0;
  bool isConfRight = true;
  byte data;
  byte Tx;
  while (1) {
    if ( radio.available()) {
      while (radio.available()) {
        radio.read( &data, sizeof(byte));
        if (count == 128 && isConfRight && data == 16) break;
        if (data != currentConf[count]) isConfRight = false;
        //Serial.print(data);
        //Serial.print(" | ");
        //Serial.println(currentConf[count]);
        count++;
        data = 0;
      }
    }
    if (millis() - startedWaitingAt > 200) {
      //Keyboard.print(" communication interrupted");
      return true;
    }
    if (count >= 129) break;
  }
  if (isConfRight) {
    radio.stopListening();
    Tx = 16;
    if (!radio.write( &Tx, sizeof(byte)))
    {
      //Serial.println("Couldn't send confirmation");
    }
  }
  //Keyboard.print(" cofing is ok");
  return isConfRight;
}

bool updateRemote() {
  //Serial.println("updating remote");
  //Keyboard.print("updating remote");
  radio.stopListening();
  byte Tx;
  for (int i = 0; i < sizeof(currentConf); i++) {
    Tx = currentConf[i];
    if (!radio.write( &Tx, sizeof(byte)))
    {
      //Serial.println("failed updating");
    }
  }
}

uint64_t getPipe(byte index) {
  uint64_t out;
  if (index == 1) index = 5;
  else index = 0;
  out += EEPROM.read(200 + index);
  out = out << 8;
  out += EEPROM.read(201 + index);
  out = out << 8;
  out += EEPROM.read(202 + index);
  out = out << 8;
  out += EEPROM.read(203 + index);
  out = out << 8;
  out += EEPROM.read(204 + index);
  return out;
}
