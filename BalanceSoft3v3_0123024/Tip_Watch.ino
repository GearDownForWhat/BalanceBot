// watch to see if it's gone too far off setpoint for too long
int Max_Error = 30;
void Tipwatch() {




  /*
    if (!AxisStateCheck(0)) {
    TipRecovery();
    }
  */
  if (abs(Angle_Error) < Max_Error || abs(Current_Angle) < 50) { // if the angle error is less than the max error
    previousMillis[1] = millis(); // reset the timer.
  }
  if (millis() - previousMillis[1] >= 500 || ( abs(Current_Angle) > 60)) {
    TipRecovery();


  }
  // if (( abs(Current_Angle) > 60)) {
  //TipRecovery();
  // }
}

void AxisReset() {
  if ((AxisStates[0] != 8 || AxisStates[1] != 8) && Bat_Low == 0 ) { // chg 0620
    if (EnableAxisStateCheck) {

      Serial.println("Odrive Reboot Command");
      Safedelay(5000);
      SetLowCurrent();
      Serial3.print("\n");
      Serial3.print("sb\n");
      Safedelay(5000000);


      //ReqAxisState(0, 0);
      Safedelay(5000);
      //ReqAxisState(1, 0);
      Serial.println("Fix Axis State In Tip Watch");
      Safedelay(5000);
      SetHighCurrent();


      Serial.print("Axis1State : ");
      Serial.println(AxisStates[0]);
      Serial.print("Axis2State : ");
      Serial.println(AxisStates[1]);
      OdriveInit();


    }
  }
}

void StopNicely() {
  int LastDriveSpeed = 1000;
  Current_Speed = 0;
  while ( abs(LastDriveSpeed) > 30) {
    LastDriveSpeed = Drive(999, 1);
    if (LastDriveSpeed > 0) {
      Drive(LastDriveSpeed * .95, 1);
    } else {
      Drive(LastDriveSpeed * .95, 1);
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
  AxisReset();

  if(!BlockAllDiag) Serial.println("TipRecovery");
  StopNicely();
  Wheel_Drive_PID.SetMode(MANUAL);
  Cmd_Angle_PID.SetMode(MANUAL);
  Rot_Angle_PID.SetMode(MANUAL);
  Current_Speed = 0;
  //AxisReset();
  Safedelay(10);
  //Serial.println("SD7");




  OdriveInit();
  while ( abs(Current_Angle) > 45) {
    //GCodeControl();
    ReadOdrive(100);
    Safedelay(50000);
    if(scanNum % 50 == 0 && !BlockAllDiag){
    Serial.print("Tipped... Angle = ");
    Serial.println(Current_Angle);
    }
  }


  Wheel_Drive_PID.SetMode(AUTOMATIC);
  Cmd_Angle_PID.SetMode(AUTOMATIC);
  Rot_Angle_PID.SetMode(AUTOMATIC);
  Wheel_Drive_PID.Compute(.1);
  Cmd_Angle_PID.Compute(.1);
  //Wheel_Drive_PID.Compute(0);
  //Cmd_Angle_PID.Compute(0);
  previousMillis[1] = millis();
  lastmicros = micros();
  ReqRotationAngle = CurRotationAngle; // set the rotation equal to where it is now.

}
