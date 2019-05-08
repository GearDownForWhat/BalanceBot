


// set the LED on
volatile unsigned long PinHighStartTime[3];
volatile long Channel[3];
double Last_Command_Angle = 0;



//double RangeVal = 27.6 * AMP;
//double DeRangeVal = AMP / 27.6;
int loopcount = 0;
void loop() {
  if (RemoteCommCheck) {
    EnableOuterLoop = RemoteCommOK;
  }

  if (loopcount >= 2) {
    GetDMPdata(1);
  }
  else {
    GetDMPdata(0);
    loopcount = 0;
  }
  loopcount++;

  GCodeControl();
  MainProgram();
  Tipwatch();
  ReadOdrive();
  Blink();
  //SetPIDTunings1();
  //SetPIDTunings2();
  //SetPIDTunings3();
  VoltageDetect();
  CheckRemoteComm();
  //Serial.println
}



int SteerChannel = 0;
int DriveChannel = 0;
int Ch3 = 0;

bool flag;

unsigned long lastmicros = micros();
int batDead = 0;
void MainProgram() {



  Wheel_Drive_PID.SetTunings( p[0]*GripFactor, i[0]*GripFactor, d[0]*GripFactor);





  float dt = 1e-6 * (micros() - lastmicros);
  lastmicros = micros();

  SteerChannel = Average1(0, Channel[0]);

  DriveChannel  = Average1(1, Channel[1]);

  Command_Speed = (map(DriveChannel, 1000, 2000, -500, 500) - 76.0); //  - 9.5;

  if (Bat_Low && abs(Command_Speed)) {
    AMP = 1;
    Serial.println("BattLow");
  }


  if (abs(Command_Speed ) < 40)
  {
    Command_Speed  = 0;
  }
  double Speed_Error = Command_Speed - Current_Speed;

  Angle_Error = Current_Angle - Command_Angle;


  if (!EnableOuterLoop) {
    Command_Angle = -1;
  }

  Wheel_Drive_PID.Compute(0);
  Speed_Error =  (Current_Speed -  Command_Speed);


  CA_PID_I = Current_Speed * AMP;
  CA_PID_SP = Command_Speed * AMP;

  if (abs(Speed_Error) <= SpeedDeadBand) {
    double CA_Decay2 = CA_Decay - map2(Speed_Error, 0, 100, .2, 0);
    Cmd_Angle_PID.Compute(CA_Decay2);
  } else {
    Cmd_Angle_PID.Compute(0);
  }

  int OutputLimitPos;
  int OutputLimitNeg;


  if (GoFaster) {
    OutputLimitPos = constrain(map2(Current_Speed, 0, 1000, 30, 10), 0, 25);

    OutputLimitNeg = constrain(map2(Current_Speed, -1000, 0, -10, -30), -25, 0);
  }
  else {
    OutputLimitPos = constrain(map2(Current_Speed, 0, 1000, 20, 0), 0, 20);

    OutputLimitNeg = constrain(map2(Current_Speed, -1000, 0, 0, -20), -20, 0);

  }
  Cmd_Angle_PID.SetOutputLimits(OutputLimitNeg, OutputLimitPos);





  if (Command_Angle_Smooth) {
    double togo = abs(Command_Angle - Command_Angle_Out);
    if (millis() - previousMillis[5] >= 5) {
      previousMillis[5] = millis();


      if (abs(Command_Angle) < abs(Command_Angle_Out)&& abs(Command_Angle_Out)>10) {
        if (Command_Angle < Command_Angle_Out) {
          Command_Angle = Command_Angle +.1;
        }
        if (Command_Angle > Command_Angle_Out) {
          Command_Angle = Command_Angle -.1;
        }
      }
      else {
        if (Command_Angle < Command_Angle_Out) {
          Command_Angle = Command_Angle + (togo*.12);
        }
        if (Command_Angle > Command_Angle_Out) {
          Command_Angle = Command_Angle - (togo*.12);
        }
      }
    }

  }
  else {
    Command_Angle = Command_Angle_Out - 1;
  }

  Current_Speed = constrain(Current_Speed, -MaxCPS, MaxCPS);

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

  if (0) {
    Serial.print((DriveChannel / 150.0));
    Serial.print("\t");
    Serial.print(Current_Speed);
    Serial.print("\t");
    Serial.print(Command_Speed);
    Serial.print("\t");
    Serial.print(Current_Angle);
    Serial.print("\t");
    Serial.print(Command_Angle, 10);
    Serial.println();
  }

}
bool LED_STATE = 0;
unsigned long LastTime3 = 0;
unsigned long LastTime4 = 0;
void Blink() {
  if ( millis() - LastTime3 > 50) {
    LastTime3 = millis();
    LED_STATE = !LED_STATE;
    digitalWrite(ledPin, LED_STATE);

  }

}
void Blink(int time) {
  LastTime4 =  millis();
  while (millis() - LastTime4 > time) {
    if ( millis() - LastTime3 > 50) {
      LastTime3 = millis();
      LED_STATE = !LED_STATE;
      digitalWrite(ledPin, LED_STATE);

    }

  }
}
