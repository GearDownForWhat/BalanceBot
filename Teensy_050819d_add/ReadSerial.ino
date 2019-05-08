void SerialLoopTest() {

  if (Serial1.available())
    Serial.write(Serial1.read());
  if (Serial.available())
    Serial1.write(Serial.read());
}




float GetVal(int BN, char letter, float NA_val) {
  char *bufPtr0 = buff0;


  while (bufPtr0  > (char*) 1 && *bufPtr0 && bufPtr0 < buff0 + buf0Length) {
    if (*bufPtr0 == letter) {
      return atof(bufPtr0 + 1 );
    }
    bufPtr0 = strchr(bufPtr0, ' ') + 1;
  }

  return NA_val;
}

void ParseGCode(int BN) {
  // Serial.println("ParseGCode");

  //ResetBuffer[BN] = 1;
int Motor = GetVal(BN, 'A', -1);
if (Motor == 0 || Motor == 1){

    if (GetVal(BN, 'S', -99) != -99) {

      AxisState[Motor] = GetVal(BN, 'S', -99);
      //Serial.println("rcvd");
    }
    if (GetVal(BN, 'C', -99) != -99) {
      MotorCurrent[Motor] = GetVal(BN, 'C', -99);
      // Serial.println("iasdfasd1");
    }
    if (GetVal(BN, 'R', -99) != -99) {
      MotorVelocity[Motor] = GetVal(BN, 'R', -99);
      // Serial.println("iasdfasd1");
    }
}

  if (GetVal(BN, 'V', -1) != -1) {
    VbusVoltage = GetVal(BN, 'V', -1);
  }

  buf0Length = 0;

}

void PreloadBuff(char char_) {

  if (buf0Length < BUF_SIZE - 1) {
    buff0[buf0Length++] = char_; // store it
  }
}

void GCodeControl() {
  // listen for serial commands
  while (Serial2.available() > 0) { // if something is available
    char c = Serial2.read(); // get it
    //Serial.print(c);  // repeat it back so I know you got the message
    if (buf0Length < BUF_SIZE - 1) {
      buff0[buf0Length++] = c; // store it
    }

    if (c == '\n') {
      buff0[buf0Length] = 0; // end the buffer so string functions work right
      //Serial.print("Odrive Sent: ");
      //Serial.println(buff0);
      ParseGCode(0);  // do something with the command
      NewSerial = 1;
      //Serial.println("END");
      // use "both NL & CR" in serial monitor
    }
  }

}
