void receiveUpdate() {
  radio.startListening();
  int count = 0;
  unsigned long startedWaitingAt = millis();
  while (1) {
    if ( radio.available()) {
      while (radio.available()) {
        radio.read( &tempModes[count / 4][count % 4], sizeof(byte));
        EEPROM.write(count+1, tempModes[count / 4][count % 4]);
        count++;
      }
    }

    if (count == 127) {
      clearModes();
      bool isEmpty = true;
      for ( int i = 0; i < 32; i++) {
        isEmpty = true;
        for (int j = 0; j < 3; j++) {
          if (tempModes[i][j] != 0)isEmpty = false;
        }
        if (!isEmpty) addMode(tempModes[i][0], tempModes[i][1], tempModes[i][2]);
      }
      currentMode = 0;
      Red = modes[currentMode][0];
      Green = modes[currentMode][1];
      Blue = modes[currentMode][2];
      turnOnLight(1.0);

      break;
    }

    if (millis() - startedWaitingAt > 200) {
      updateFailed();
      radio.stopListening();
      return;
    }
  }
  UpdateAnimmation();
}


void checkForUpdate() {
  int currentProgram = 0;
  int totalPrograms = 0;
  byte dataRx;
  byte dataTx = 8; //sending 8 means the remote is about to check the current configuration
  if (!radio.write( &dataTx, sizeof(byte))) //if communication is interrupted, check for update will stop
  {
    return;
  }
  byte Tx;
  for ( int i = 0; i < 32;  ++i ) {     //Sending current configuration to recevier
    Tx = i;
    if (!radio.write( &Tx, sizeof(byte)))
    {
      return;
    }
    for (int j = 0; j < 3; j++) {
      Tx = modes[i][j];
      if (!radio.write( &Tx, sizeof(byte)))
      {
        return;
      }
    }
  }
  Tx = 16;
  if (!radio.write( &Tx, sizeof(byte)))   //Confirmation that the configuration has ben sent
  {
    return;
  }
  radio.startListening();  //Listen to receiver wheter any update is available
  unsigned long startedWaitingAt = millis();
  while (1) {
    while (!radio.available()) {
      if (millis() - startedWaitingAt > 200 ) {   //time out detection
        radio.stopListening();
        return;
      }
      if (input) {                               //if interrupted by user input - Leave immediately
        Awaken = true;
        radio.stopListening();
        return;
      }
    }
    if ( radio.available()) {                   //happens when message is received
      while (radio.available()) {
        radio.read( &dataRx, sizeof(byte));
        if (dataRx == 16) return;                         //Confirmation the cofiguration is up to date
        else {
          receiveUpdate();                              //Updates configuration
        }
      }
    }
  }
}

void UpdateAnimmation() {
  byte waitTime = 5;
  int light = 0;
  turnOffLight();
  delay(500);
  while (light < 160) {
    analogWrite(greenPin, light * Bright);
    light ++;
    delay(waitTime);
  }
  while (light > 0) {
    analogWrite(greenPin, light * Bright);
    light--;
    delay(waitTime);
  }
  while (light < 160) {
    analogWrite(greenPin, light * Bright);
    light++;
    delay(waitTime);
  }
  while (light > 0) {
    analogWrite(greenPin, light * Bright);
    light--;
    delay(waitTime);
  }

  turnOffLight();
  delay(500);
  turnOnLight(1.0);
  delay(700);
  int looping = 0;
  while (looping < numberOfModes - 1) {
    changeMode();
    turnOnLight(1.0);
    delay(700);
    looping++;
  }
  changeMode();
  turnOffLight();
}

void updateFailed() {

}
