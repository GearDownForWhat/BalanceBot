
void CH1PC() {
  
  RemoteComm ++;
  if (digitalRead(CH1Pin)) {
    PinHighStartTime[0] = micros();
  } else {
    Channel[0] = micros() - PinHighStartTime[0];
    //Serial.print("CH1 :");
    //Serial.print(Channel[0]);
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
    //Serial.print("CH2 :");
    //Serial.println(Channel[1]);
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
void CH4PC() {
 
  if (digitalRead(CH4Pin)) {
    PinHighStartTime[3] = micros();
  } else {
    Channel[3] = micros() - PinHighStartTime[3];
  }
// if (!RemoteCommOK || abs(Channel[3]-1500) > 750) {
 //   Channel[2] = 1500;
 // }

}
void CH5PC() {
 
  if (digitalRead(CH5Pin)) {
    PinHighStartTime[4] = micros();
  } else {
    Channel[4] = micros() - PinHighStartTime[4];
  }
 //if (!RemoteCommOK || abs(Channel[4]-1500) > 750) {
 //   Channel[4] = 1500;
 // }
  
}
unsigned long remoteWarning = 0;
void CH6PC() {
 
  if (digitalRead(CH6Pin)) {
    PinHighStartTime[5] = micros();
    //Serial.println("CH6_HIGH");
  } else {
    //Serial.println("CH6_LOW");
    Channel[5] = micros() - PinHighStartTime[5];
  }
 //if (!RemoteCommOK || abs(Channel[5]-1500) > 750) {
 //   Channel[5] = 1500;
  //}

}
void CheckRemoteComm() {

if(RemoteCommCheck){
  if (millis() - previousMillis[4] >= 100) {
    previousMillis[4] = millis();

    if ((RemoteComm < 1)) {
      RemoteCommOK = 0;
      Cmd_Angle_PID.SetMode(MANUAL);
      RemoteComm = 0;
      if(!BlockAllDiag){
         if (millis() - remoteWarning > 10000){
         Serial.println("No Remote");
        remoteWarning = millis();
        }
     
    }
    }
    else {
      RemoteCommOK = 1;
      RemoteComm = 0;
      
      Cmd_Angle_PID.SetMode(AUTOMATIC);
      
    }
  }
  }
}
