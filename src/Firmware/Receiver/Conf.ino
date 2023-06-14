void loadConfig() {
  for ( int i = 0; i < 32;  ++i ) {
    currentConf[i * 4] = i;
  }
  //-------------------------------------------
  currentConf[1] = 255;
  currentConf[6] = 255;
  currentConf[7] = 255;
  currentConf[10] = 255;
  //-------------------------------------------
}

void HIDaction(byte input) {
  byte mode = input >> 3;
  byte button = input & 0b00000111;
  switch (mode) {
    //-------------------------------------------
    case 0:
      switch (button) {
        case 1:
          Keyboard.print("j");
          break;
        case 2:
          Keyboard.print("k");
          break;
        case 3:
          Keyboard.print("l");
          break;
        case 4:
          Keyboard.print("f");
          break;
        case 5:
          Keyboard.print("m");
          break;
        case 6:
          Consumer.write(MEDIA_VOLUME_UP);
          break;
        case 7:
          Consumer.write(MEDIA_VOLUME_DOWN);
          break;
      }
      break;
    //-------------------------------------------
    case 1:
      switch (button) {
        case 1:
          Keyboard.write(KEY_LEFT_ARROW);
          break;
        case 2:
          Keyboard.print(" ");
          break;
        case 3:
          Keyboard.write(KEY_RIGHT_ARROW);
          break;
        case 4:
          Consumer.write(MEDIA_VOLUME_MUTE);
          break;
        case 5:
          Keyboard.write(KEY_RETURN);
          break;
        case 6:
          Consumer.write(MEDIA_VOLUME_UP);
          break;
        case 7:
          Consumer.write(MEDIA_VOLUME_DOWN);
          break;
      }
      break;
    //-------------------------------------------
    case 2:
      switch (button) {
        case 1:
          Consumer.write(MEDIA_PREVIOUS);
          break;
        case 2:
          Consumer.write(MEDIA_PLAY_PAUSE);
          break;
        case 3:
          Consumer.write(MEDIA_NEXT);
          break;
        case 4:
          Consumer.write(MEDIA_VOLUME_MUTE);
          break;
        case 5:
          Consumer.write(MEDIA_STOP);
          break;
        case 6:
          Consumer.write(MEDIA_VOLUME_UP);
          break;
        case 7:
          Consumer.write(MEDIA_VOLUME_DOWN);
          break;
      }
      break;
  }
}
