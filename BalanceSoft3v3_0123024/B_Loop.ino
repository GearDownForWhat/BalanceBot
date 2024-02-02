


// set the LED on
volatile unsigned long PinHighStartTime[6];
volatile long Channel[6];
double Last_Command_Angle = 0;

void loop() {
  //Serial.print("VbusVoltage: ");
  //Serial.println(VbusVoltage, 6);
  VoltageDetectOdrive();
  scanNum++;
  AxisReset();
  if (Serial.available())
    Serial3.write(Serial.read());
  if (SerialLoopback) {
    SerialLoopTest();
  } else {
    if (RemoteCommCheck) {
      EnableOuterLoop = RemoteCommOK;
    }

    ReadOdrive(500);
    GetDMPdata();
    Tipwatch();
    MainProgram();
    Blink();

    if (DIP[0]) {
      SetPIDTunings1();
    }
    if (DIP[1]) {
      SetPIDTunings2();
    }
    if (VoltageDetectIOEnable) {
      VoltageDetectIO();
    }
    if (VoltageDetectOdriveEnable) {
      VoltageDetectOdrive();
    }
    CheckRemoteComm();  // verify if remote is turned on or not
    CheckGyro();        //this checks the data from the gyro to make sure its sensible
  }
  /*
Serial.print("CH4 :");
Serial.print(Channel[3]);
Serial.print("CH5 :");
Serial.print(Channel[4]);
Serial.print("CH6 :");
Serial.println(Channel[5]);
*/
}



int SteerChannel = 0;
int DriveChannel = 0;
int Ch3 = 0;

bool flag;

unsigned long lastmicros = micros();
int batDead = 0;

void MainProgram() {

  if(UseGripFactor){
    Wheel_Drive_PID.SetTunings(p[0] * GripFactor, i[0] * GripFactor, d[0] * GripFactor);
  }
  else{
    Wheel_Drive_PID.SetTunings(p[0],i[0],d[0]);
  }
 

  float dt = 1e-6 * (micros() - lastmicros);
  lastmicros = micros();

  SteerChannel = Average1(0, Channel[0]);

  DriveChannel = Average1(1, Channel[1]);

  Command_Speed = (map(DriveChannel, 1000, 2000, -500, 500) - 76.0);  //  - 9.5;

  if ((Bat_Low == 1) && abs(Command_Speed) < 50) {
    AMP = 1;
    Serial.println("BattLow");
  } else if (Bat_Low > 1) {
    DisableMotors();
  }


  if (abs(Command_Speed) < 40) {
    Command_Speed = 0;
  }
  double Speed_Error = Command_Speed - Current_Speed;

  Angle_Error = Current_Angle - Command_Angle;


  if (!EnableOuterLoop) {
    Command_Angle = -1;
  }

  //Wheel_Drive_PID.Compute(0);
  Wheel_Drive_PID.Compute(0);
  Speed_Error = (Current_Speed - Command_Speed);


  CA_PID_I = Current_Speed * AMP;
  CA_PID_SP = Command_Speed * AMP;

  if (abs(Speed_Error) <= SpeedDeadBand) {
    //double CA_Decay2 = CA_Decay - map2(Speed_Error, 0, 10, .2, 0);
    double CA_Decay2 = map2(abs(Speed_Error), 0, 1000, CA_Decay, 1);
    if (CA_Decay2 < 0) { CA_Decay2 = 0; }
    if (CA_Decay2 > 1) { CA_Decay2 = 1; }
    if (scanNum % 10 == 0) {
      /*
      Serial.print("CADCY");
      Serial.print(CA_Decay2,10);
      Serial.print("SPD_ER");
      Serial.println(abs(Speed_Error));
      */
    }
    Cmd_Angle_PID.Compute(CA_Decay2);
    //Cmd_Angle_PID.Compute(0);
  } else {
    //Cmd_Angle_PID.Compute(0);
    Cmd_Angle_PID.Compute(0);
  }

  int OutputLimitPos;
  int OutputLimitNeg;


  if (GoFaster) {
    OutputLimitPos = constrain(map2(Current_Speed, 0, 1000, 35, 10), 0, 35);

    OutputLimitNeg = constrain(map2(Current_Speed, -1000, 0, -10, -35), -35, 0);
  } else {
    OutputLimitPos = constrain(map2(Current_Speed, 0, 1000, 20, 0), 0, 20);

    OutputLimitNeg = constrain(map2(Current_Speed, -1000, 0, 0, -20), -20, 0);
  }
  Cmd_Angle_PID.SetOutputLimits(OutputLimitNeg, OutputLimitPos);

  if (LimitIntegralWindup1) {

    Wheel_Drive_PID.SetOutputLimits(-(27600.0 * AMP / 10.0), (27600.0 * AMP / 10));
  }

  if (Command_Angle_Smooth) {
    double togo = abs(Command_Angle - Command_Angle_Out);
    if (millis() - previousMillis[5] >= 5) {
      previousMillis[5] = millis();


      if (abs(Command_Angle) < abs(Command_Angle_Out) && abs(Command_Angle_Out) > 10) {
        if (Command_Angle < Command_Angle_Out) {
          Command_Angle = Command_Angle + .1;
        }
        if (Command_Angle > Command_Angle_Out) {
          Command_Angle = Command_Angle - .1;
        }
      } else {
        if (Command_Angle < Command_Angle_Out) {
          Command_Angle = Command_Angle + (togo * .12);
        }
        if (Command_Angle > Command_Angle_Out) {
          Command_Angle = Command_Angle - (togo * .12);
        }
      }
    }

  } else {
    Command_Angle = Command_Angle_Out;
  }


  if (LimitIntegralWindup2) {
    double limit = MaxCPS / (27600.0);
    //Serial.println(limit);
    Current_Speed = constrain(Current_Speed, -limit, limit);
  } else {
    Current_Speed = constrain(Current_Speed, -MaxCPS, MaxCPS);
  }
  if (ResetTipFlag) {
    ResetTipFlag = 0;
    Current_Speed = 0;
    dt = 0;
    lastmicros = micros();
  } else {
    Current_Speed += (WD_PID_Out_Vector * dt / AMP);
    Drive(Current_Speed, 0);
  }
  //Serial.print((WD_PID_Out_Vector));
  //Serial.print("\t");

  if (debug == 1) {  //&& (scanNum % 1000 == 0)) {
    //Serial.println("DebugData");
    //Serial.print("Drive Channel: ");
    //Serial.print((DriveChannel / 150.0));
    //Serial.print(",Cur Speed: ");
    // Serial.print(",");
    //Serial.print(Current_Speed/150.0);
    //Serial.print(",Cmd Speed: ");
    //Serial.print(",");
    //Serial.print(Command_Speed,5);
    //Serial.print(",Cur Angle :");
    //Serial.print(",");
    //Serial.println(Current_Angle,5);
    //Serial.print(",Cmd Angle: ");
    //Serial.print(",");
    //Serial.print(Command_Angle,5);
    //Serial.println();
  }
}
bool LED_STATE = 0;
unsigned long LastTime3 = 0;
unsigned long LastTime4 = 0;
void Blink() {
  if (millis() - LastTime3 > 50) {
    LastTime3 = millis();
    LED_STATE = !LED_STATE;
    digitalWrite(ledPin, LED_STATE);
  }
}
