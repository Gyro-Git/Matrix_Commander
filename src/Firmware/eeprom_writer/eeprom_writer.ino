#include <EEPROM.h>

//Serial number - ASCII characters
char n0 = '0';
char n1 = '0';
char n2 = '0';

void setup() {
  //Serial.begin(115200);
  //1st pipe
  EEPROM.write(200, 'G');
  EEPROM.write(201, 'R');
  EEPROM.write(202, n0);
  EEPROM.write(203, n1);
  EEPROM.write(204, n2);
  //Serial.println(long(getPipe(0)),HEX);

  //Second pipe
  EEPROM.write(205, 'G');
  EEPROM.write(206, 'U');
  EEPROM.write(207, n0);
  EEPROM.write(208, n1);
  EEPROM.write(209, n2);
  //Serial.println(long(getPipe(1)),HEX);
}

void loop() {
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
