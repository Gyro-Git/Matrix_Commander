#include <SPI.h>
#include <RF24.h>
#include <Bounce2.h>
#include <EEPROM.h>

const int redPin = 9; //pin for red LED
const int greenPin = 5;
const int bluePin = 6;
const int lowVPin = 8;
const int batPin = 20; //pin 20 is A6

const int rotA = 7; //Rotary encoder pin A
const int rotB = 16;
const int modeButton = 4;
const int A = 14; //button A
const int B = 15;
const int X = 17;
const int Y = 18;
const int Z = 19;

unsigned int standByTime = 3000; //in ms
unsigned int listenTime = 1000; // in ms
volatile bool input = false;
long startTime;
int message = 0;
int Alast;
int Blast;
static uint16_t state = 0, rotCount = 0;

uint8_t Red = 100;
uint8_t Green = 100;
uint8_t Blue = 50;
float Bright = 0.5;

//variables for modes
byte modes[32][3];
byte numberOfModes;
byte currentMode;
bool Awaken = false;
//temporary configuration
byte tempModes[32][3];

byte lowBattLight = 255;
unsigned long battTime;
byte battLoops = 0;
bool battAnim = true;
bool emptyBatt = false;
float fadeInAmount = 1.0;
unsigned long fadeInTime;
bool fadeInAnim = true;
float fadeOutAmount = 1.0;
unsigned long fadeOutTime;
bool fadeOutAnim = true;
float stepFadeOut = 0.04;
float fadeOutDelay = 5.0;

unsigned long buttonTime;
unsigned long repeatTime;

struct configuration {
  unsigned long mode[16];
} myconfig;

//radio configuration
RF24 radio(1, 10);
byte addresses[][6] = {"1Node", "2Node"};

Bounce bouncer1 = Bounce();
Bounce bouncer2 = Bounce();
Bounce bouncerA = Bounce();
Bounce bouncerB = Bounce();
Bounce bouncerX = Bounce();
Bounce bouncerY = Bounce();
Bounce bouncerZ = Bounce();
Bounce bouncerM = Bounce();



void loop() {
  if (input) {        //only occurs during isr method which is interrupt
    emptyBatt = false;
    startTime = millis();
    fadeInAnim = true;
    fadeInAmount = 0.0;
    fadeInTime = millis();
    fadeOutAmount = 1.0;
    repeatTime = millis();
    buttonTime = millis();
    if (isBatteryEmpty()) {
      emptyBatt = true;
      battAnim = true;
      fadeInAnim = false;
      turnOffLight();
      battLoops = 0;
      lowBattLight = 255;
      battTime = millis();
    }
    while (millis() - startTime < standByTime) {
      checkButtons();
    }
  }
  turnOffLight();
  EEPROM.update(210, currentMode);
  Awaken = false;
  checkForUpdate();
  if (!Awaken) {
    enterSleep();       //puts arduino to sleep to save power
  }
  //turnOnLight(1.0);
}


void checkButtons() {       //Scans which button is pressed and sends message
  if (emptyBatt && battAnim) {
    analogWrite(redPin, lowBattLight * Bright);
    if (millis() - battTime > 6) {
      lowBattLight -= 4;
      if (lowBattLight <= 4) {
        lowBattLight = 255;
        battLoops++;
      }
      if (battLoops >= 2) {
        battAnim = false;
        fadeInAnim = true;
      }
      battTime = millis();
    }
  }

  if (fadeInAnim) {
    if (millis() - fadeInTime > 5) {
      turnOnLight(fadeInAmount);
      fadeInAmount += 0.04;
      fadeInTime = millis();
    }
    if (fadeInAmount >= 1.0) {
      fadeInAnim = false;
      turnOnLight(1.0);
    }
  }


  if (millis() - startTime > standByTime - (1 / stepFadeOut)*fadeOutDelay) {
    if (millis() - fadeOutTime > fadeOutDelay) {
      turnOnLight(fadeOutAmount);
      fadeOutAmount -= stepFadeOut;
      fadeOutTime = millis();
    }
  }

  message = 0;
  bouncer1.update();
  bouncer2.update();
  bouncerA.update();
  bouncerB.update();
  bouncerX.update();
  bouncerY.update();
  bouncerZ.update();
  bouncerM.update();
  unsigned int longPressDelay = 400;
  unsigned int repeatPressDelay = 33;

  if (bouncerA.fell()) {
    message = 1;
    startTime = millis();
    sendButton(message);
  }
  if (!bouncerA.read()) {
    if (!bouncerA.rose()) {
      if (millis() - buttonTime > longPressDelay) {
        if (millis() - repeatTime > repeatPressDelay) {
          sendButton(1);
          startTime = millis();
          repeatTime = millis();
        }
      }
    }
    input = false;
    return;
  }

  if (bouncerB.fell()) {
    message = 2;
    startTime = millis();
    sendButton(message);
  }
  if (!bouncerB.read()) {
    if (!bouncerB.rose()) {
      if (millis() - buttonTime > longPressDelay) {
        if (millis() - repeatTime > repeatPressDelay) {
          sendButton(2);
          startTime = millis();
          repeatTime = millis();
        }
      }
    }
    input = false;
    return;
  }

  if (bouncerX.fell()) {
    message = 3;
    startTime = millis();
    sendButton(message);
  }
  if (!bouncerX.read()) {
    if (!bouncerX.rose()) {
      if (millis() - buttonTime > longPressDelay) {
        if (millis() - repeatTime > repeatPressDelay) {
          sendButton(3);
          startTime = millis();
          repeatTime = millis();
        }
      }
    }
    input = false;
    return;
  }

  if (bouncerY.fell()) {
    message = 4;
    startTime = millis();
    sendButton(message);
  }
  if (!bouncerY.read()) {
    if (!bouncerY.rose()) {
      if (millis() - buttonTime > longPressDelay) {
        if (millis() - repeatTime > repeatPressDelay) {
          sendButton(4);
          startTime = millis();
          repeatTime = millis();
        }
      }
    }
    input = false;
    return;
  }

  if (bouncerZ.fell()) {
    message = 5;
    startTime = millis();
    sendButton(message);
  }
  if (!bouncerZ.read()) {
    if (!bouncerZ.rose()) {
      if (millis() - buttonTime > longPressDelay) {
        if (millis() - repeatTime > repeatPressDelay) {
          sendButton(5);
          startTime = millis();
          repeatTime = millis();
        }
      }
    }
    input = false;
    return;
  }
  
  /*if (bouncerB.fell()) {
    message = 2;
  }

  if (bouncerX.fell()) {
    message = 3;
  }

  if (bouncerY.fell()) {
    message = 4;
  }

  if (bouncerZ.fell()) {
    message = 5;
  }*/

  if (bouncerM.fell()) {
    changeMode();
    startTime = millis();
  }


  if (bouncer1.fell()) {
    if (bouncer2.read()) {
      message = 6;
    } else {
      message = 7;
    }
  }

  if (message != 0) {
    startTime = millis();
    sendButton(message);
  }
  input = false;                        //prevents from looping
  buttonTime = millis();
}
