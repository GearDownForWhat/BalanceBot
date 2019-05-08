// watch to see if it's gone too far off setpoint for too long
int Max_Error = 30;
void Tipwatch() {
  currentMillis[1] = millis();



  if ((currentMillis[1] - previousMillis[1] >= 500 || ( abs(Current_Angle) > 60))) {
    TipRecovery();
  }
  if (!AxisStateCheck(0)) {
    if (!AxisStateCheck(1)) {
      TipRecovery();
    }
  }
  if (abs(Angle_Error) < Max_Error || abs(Current_Angle) < 50) { // if the angle error is less than the max error
    previousMillis[1] = millis(); // reset the timer.
  }
  // if (( abs(Current_Angle) > 60)) {
  //TipRecovery();
  // }
}
bool AxisStateCheck(bool Now) {
  if (Now) {
   if  (ReqAxisState(0, 1) == 8 && ReqAxisState(1, 1) == 8){
    return 1;
   }
    //ReqAxisState(1, 1);
  }
  else if (AxisState[0] == 8 && AxisState[1] == 8) {
    //Serial.print
    return 1;
  }
  return 0;
}

void StopNicely() {
  int LastDriveSpeed = 1000;
  Current_Speed = 0;
  while ( abs(LastDriveSpeed) > 30) {
    LastDriveSpeed = Drive(999, 1);
    if (LastDriveSpeed > 0) {
      Drive(LastDriveSpeed*.95, 1);
    } else {
      Drive(LastDriveSpeed*.95, 1);
    }
    Safedelay(2);
    //Serial.println("SD5");
  }
  Current_Speed = 0;
  lastmicros = micros();
  Drive(0, 1);
  Safedelay(10);
  //Serial.println("SD6");
  ResetTipFlag = 1;
}




void TipRecovery() {
  Serial.println("TipRecovery");
  StopNicely();
  Wheel_Drive_PID.SetMode(MANUAL);
  Cmd_Angle_PID.SetMode(MANUAL);
  Rot_Angle_PID.SetMode(MANUAL);
  Current_Speed = 0;
  AxisReset();
  Safedelay(10);
  //Serial.println("SD7");




  OdriveInit();
  while ( abs(Current_Angle) > 45) {
    GCodeControl();
    Safedelay(100);
    Serial.print("Tipped... Angle = ");
    Serial.println(Current_Angle);
  }


  Wheel_Drive_PID.SetMode(AUTOMATIC);
  Cmd_Angle_PID.SetMode(AUTOMATIC);
  Rot_Angle_PID.SetMode(AUTOMATIC);
  Wheel_Drive_PID.Compute(.1);
  Cmd_Angle_PID.Compute(.1);
  previousMillis[1] = millis();
  lastmicros = micros();
  ReqRotationAngle = CurRotationAngle; // set the rotation equal to where it is now. 

}
