// watch to see if it's gone too far off setpoint for too long
int Max_Error = 20;
void Tipwatch() {

  if (abs(Angle_Error) < Max_Error || abs(Current_Angle) < (MaxLeanAngle+2.0)) { // if the angle error is less than the max error
    previousMillis[1] = millis(); // reset the timer.
  }
  if (millis() - previousMillis[1] >= 300 || ( abs(Current_Angle) > (MaxLeanAngle+10.0))) {

    TipRecovery();


  }
}
void AxisReset() {
  if ((AxisStates[0] != 8 || AxisStates[1] != 8) && Battery_Status < 2 ) { // chg 0620
    if (EnableAxisStateCheck) {
      Serial.println("Odrive Reboot Command");
      Safedelay(5000);
      SetLowCurrent();
      Serial3.print("\n");
      Serial3.print("sb\n");
      Safedelay(5000000);
      Safedelay(5000);
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
  ResetTipFlag = 1;
}




void TipRecovery() {
  
  Serial.print(" TipRecovery:  ");
  AxisReset();

  if(!BlockAllDiag) Serial.println("TipRecovery");
  StopNicely();
  Wheel_Drive_PID.SetMode(MANUAL);
  Cmd_Angle_PID.SetMode(MANUAL);
  Rot_Angle_PID.SetMode(MANUAL);
  Current_Speed = 0;
  Safedelay(10);
  OdriveInit();
  while ( abs(Current_Angle) > 5) {
     Safedelay(50000);
    digitalWrite(LEDPIN2, HIGH);
    digitalWrite(LEDPIN4, HIGH);
    ReadOdrive(10);
    Safedelay(50000);
    if(scanNum % 50 == 0){
    Serial.print(" Tipped... Angle = ");
    Serial.print(Current_Angle);
    Serial.print(" mode: ");
    Serial.print(mode);
    Serial.print(" AMP: "); // this is a good time to print some data to the serial port
    Serial.print(AMP);
    Serial.print(" Steer_AMP: ");
    Serial.print(Steer_Amp);
    Serial.print("  Battery Voltage: ");
    Serial.println(VbusVoltage);
    }
    digitalWrite(LEDPIN2, LOW);
    digitalWrite(LEDPIN4, LOW);
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
