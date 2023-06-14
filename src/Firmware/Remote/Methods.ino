void sendButton(unsigned char data) {    //send data over radio
  //encode mode
  byte modeBin = currentMode << 3;
  byte encodedMsg = modeBin | data;
  if (radio.write( &encodedMsg, sizeof(byte))) {
    encodedMsg = 0b11111001;              //confirmation message so it doesn't confuse config with normal messages when signal is interrupted
    radio.write( &encodedMsg, sizeof(byte));
  }
}

void isr() {                      //called when interrupt happens which wakes up arduino from sleep
  input = true;
}

void enterSleep() {             //Puts arduino to sleep to save power
  radio.powerDown();
  digitalWrite(1, HIGH);
  digitalWrite(10, HIGH);
  digitalWrite(11, HIGH);
  digitalWrite(12, HIGH);
  digitalWrite(13, HIGH);
  pinMode(rotA, INPUT);
  pinMode(rotB, INPUT);
  digitalWrite(lowVPin, LOW); //disable radio
  attachInterrupt(digitalPinToInterrupt(2), isr, CHANGE);
  attachInterrupt(digitalPinToInterrupt(3), isr, CHANGE);
  ADCSRA &= ~(1 << 7);  //Disable ADC
  //BOD DISABLE - this must be called right before the __asm__ sleep instruction
  MCUCR |= (3 << 5); //set both BODS and BODSE at the same time
  MCUCR = (MCUCR & ~(1 << 5)) | (1 << 6); //then set the BODS bit and clear the BODSE bit at the same time
  __asm__  __volatile__("sleep");//in line assembler to go to sleep
  //after wake up
  detachInterrupt(digitalPinToInterrupt(2));
  detachInterrupt(digitalPinToInterrupt(3));
  ADCSRA |= (1 << 7);
  digitalWrite(lowVPin, HIGH); //enable radio
  delay(2);
  initRadio();                                                  //initialization of radio
  pinMode(rotA, INPUT_PULLUP);
  pinMode(rotB, INPUT_PULLUP);
}




bool isBatteryEmpty() {                 //returns whether battery is too low
  if (analogRead(A6) >= 1000) {
    return true;
  }
  return false;
}

void blinkLights(int red, int green, int blue, int reps, int period) {
  for (int i = 0; i < reps; i++) {
    if (red == 1) digitalWrite(redPin, HIGH);
    if (green == 1) digitalWrite(greenPin, HIGH);
    if (blue == 1) digitalWrite(bluePin, HIGH);
    delay(period);
    if (red == 1) digitalWrite(redPin, LOW);
    if (green == 1) digitalWrite(greenPin, LOW);
    if (blue == 1) digitalWrite(bluePin, LOW);
    delay(period);
  }
}

void turnOnLight(float intensity) {
  if (intensity < 0.0) intensity = 0.0;
  if (intensity > 1.0) intensity = 1.0;
  analogWrite(redPin, Red * Bright * intensity);
  analogWrite(greenPin, Green * Bright * intensity);
  analogWrite(bluePin, Blue * Bright * intensity);
}

void turnOffLight() {

  digitalWrite(redPin, LOW);
  digitalWrite(greenPin, LOW);
  digitalWrite(bluePin, LOW);
}

void changeMode() {
  currentMode ++;
  if (currentMode >= numberOfModes) currentMode = 0;
  Red = modes[currentMode][0];
  Green = modes[currentMode][1];
  Blue = modes[currentMode][2];
  //turnOnLight(1.0);
  fadeInAnim = true;
  fadeInAmount = 0.0;
  fadeInTime = millis();
}

void setMode(byte selection) {
  currentMode == selection;
  if (currentMode >= numberOfModes) currentMode = 0;
  Red = modes[currentMode][0];
  Green = modes[currentMode][1];
  Blue = modes[currentMode][2];
  //turnOnLight(1.0);
  fadeInAnim = true;
  fadeInAmount = 0.0;
  fadeInTime = millis();
}

void addMode(byte r, byte g, byte b) {
  int index;
  if (numberOfModes == 0) index = 0;
  else index = numberOfModes;
  modes[index][0] = r;
  modes[index][1] = g;
  modes[index][2] = b;
  numberOfModes++;
}

void clearModes() {
  for ( int i = 0; i < 32;  ++i ) {
    for (int j = 0; j < 3; j++) {
      modes[i][j] = 0;
    }
  }
  numberOfModes = 0;
  currentMode = 0;
}

void loadConfig() {
  clearModes();
  bool isEmpty = true;
  for ( int i = 0; i < 128;  i += 4 ) {
    isEmpty = true;
    for (int j = 1; j < 4; j++) {
      if (EEPROM.read(i + j) != 0)isEmpty = false;
    }
    if (!isEmpty) addMode(EEPROM.read(i + 1), EEPROM.read(i + 2), EEPROM.read(i + 3));
    else break;
  }
  currentMode = 0;
  Red = modes[currentMode][0];
  Green = modes[currentMode][1];
  Blue = modes[currentMode][2];
}

uint64_t getPipe(byte index) {    //loads pipe(address)from EEPROM memory. Each remote/receiver combo has its own address
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
