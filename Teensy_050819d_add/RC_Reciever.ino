
void CH1PC() {
  RemoteComm ++;
  if (digitalRead(CH1Pin)) {
    PinHighStartTime[0] = micros();
  } else {
    Channel[0] = micros() - PinHighStartTime[0];
  }
  if (!RemoteCommOK || abs(Channel[0]-1500) > 750) {
    Channel[0] = 1500;
  }
}
void CH2PC() {
  RemoteComm ++;
  if (digitalRead(CH2Pin)) {
    PinHighStartTime[1] = micros();
  } else {
    Channel[1] = micros() - PinHighStartTime[1];
  }
 if (!RemoteCommOK || abs(Channel[1]-1500) > 750) {
    Channel[1] = 1500;
  }
}
void CH3PC() {
  RemoteComm ++;
  if (digitalRead(CH3Pin)) {
    PinHighStartTime[2] = micros();
  } else {
    Channel[2] = micros() - PinHighStartTime[2];
  }
 if (!RemoteCommOK || abs(Channel[2]-1500) > 750) {
    Channel[2] = 1500;
  }
}
void CheckRemoteComm() {

if(RemoteCommCheck){
  if (millis() - previousMillis[4] >= 100) {
    previousMillis[4] = millis();

    if ((RemoteComm < 1)) {
      RemoteCommOK = 0;
      Cmd_Angle_PID.SetMode(MANUAL);
      RemoteComm = 0;
    }
    else {
      RemoteCommOK = 1;
      RemoteComm = 0;
      
      Cmd_Angle_PID.SetMode(AUTOMATIC);
      
    }
  }
  }
}
