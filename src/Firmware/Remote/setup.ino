void setup()
{
  //Serial.begin(115200);
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(lowVPin, OUTPUT);

  pinMode(rotA, INPUT_PULLUP);
  bouncer1.attach(rotA);
  bouncer1.interval(5);
  pinMode(rotB, INPUT_PULLUP);
  bouncer2.attach(rotB);
  bouncer2.interval(5);
  pinMode(modeButton, INPUT);
  bouncerM.attach(modeButton);
  bouncerM.interval(5);
  pinMode(A, INPUT);
  bouncerA.attach(A);
  bouncerA.interval(5);
  pinMode(B, INPUT);
  bouncerB.attach(B);
  bouncerB.interval(5);
  pinMode(X, INPUT);
  bouncerX.attach(X);
  bouncerX.interval(5);
  pinMode(Y, INPUT);
  bouncerY.attach(Y);
  bouncerY.interval(5);
  pinMode(Z, INPUT);
  bouncerZ.attach(Z);
  bouncerZ.interval(5);

  Alast = digitalRead(rotA);
  Blast = digitalRead(rotB);

  /*
    Radio    Arduino
    CE    -> 9
    CSN   -> 10 (Hardware SPI SS)
    MOSI  -> 11 (Hardware SPI MOSI)
    MISO  -> 12 (Hardware SPI MISO)
    SCK   -> 13 (Hardware SPI SCK)
    IRQ   -> No connection
    VCC   -> No more than 3.6 volts
    GND   -> GND
  */
  digitalWrite(lowVPin, HIGH);
  loadConfig();
  delay(3);
  //blinkLights(0, 0, 1, 3, 200);
  //checkBattery();                                               //checks battery status
  initRadio();                                                  //initialization of radio
  sendButton(0);
  digitalWrite(lowVPin, LOW);
  //attachInterrupt(digitalPinToInterrupt(2), isr, CHANGE);      //Interrupt setup
  //attachInterrupt(digitalPinToInterrupt(3), isr, CHANGE);      //Interrupt setup
  setupSleep();
  currentMode = EEPROM.read(210);
  setMode(currentMode);
  turnOffLight();
  enterSleep();
}

void initRadio() {    //initialization of radio
  if (!radio.begin()) //if radio can't be found turn on all LEDs and do nothing.
  {
    blinkLights(1, 0, 0, 6, 350);
    while (1); // Wait here forever. MUST BE CHANGED!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  }
  radio.setPALevel(RF24_PA_MAX);  //adjustable power level
  radio.setDataRate(RF24_250KBPS); //slowest speed for higher range
  radio.setChannel(124);
  radio.setPayloadSize(1);

  radio.openWritingPipe(getPipe(0));
  radio.openReadingPipe(1, getPipe(1));
}

void setupSleep() {
  SMCR |= (1 << 2); //power down mode
  SMCR |= 1;//enable sleep
}
