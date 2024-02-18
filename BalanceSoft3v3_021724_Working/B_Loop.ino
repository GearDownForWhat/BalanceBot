
volatile unsigned long PinHighStartTime[6];
volatile long Channel[6];
double Last_Command_Angle = 0;


void loop() {
  Serial.println("Mode Search ");
  delay(500);

  switch (mode) {
    case 0:  // Standard Mode
      while (mode == 0) {
        MainProgram();
      }
      break;
    case 1:  // Tune Speed and Steering Mode
      while (mode == 1) {
        MainProgram();
      }
      break;
    case 5:  // Tune Inner Loop (Balance) PID Mode
      while (mode == 5) {
        SetPIDTunings1();
        EnableOuterLoop = 0;
        MainProgram();
         
      }
      break;
    case 6:  // Tune Outer Loop (Balance Angle) PID Mode
      while (mode == 6) {
        SetPIDTunings2();
        MainProgram();
      }
      break;
    case 7:  // Tune Odrive Settings
      while (mode == 7) {
        VoltageDetectOdriveEnable = 0;
        SetOdriveTunings();
        MainProgram();
      }
      break;

    case 10:  // MPU6050 Debug Mode
      while (mode == 10) {
        MainProgram();
        Serial.print(",Cur Speed: ");
        Serial.print(",");
        Serial.print(Current_Speed);
        Serial.print(",Cmd Speed: ");
        Serial.print(",");
        Serial.print(Command_Speed, 5);
        Serial.print(",Cur Angle :");
        Serial.print(",");
        Serial.print(Current_Angle, 5);
        Serial.print(",Cmd Angle: ");
        Serial.print(",");
        Serial.print(Command_Angle, 5);
        Serial.println();
      }
      break;
    case 11:  // Odrive Debug Mode
      while (mode == 11) {
        ReadOdrive(100);
        Serial.print("VbusVoltage: ");
        Serial.print(VbusVoltage, 6);
        Serial.println("DebugData");

      }
      break;
    case 12:  // Remote Debug Mode
      while (mode == 12) {
        CheckRemoteComm();
        Serial.print(Channel[0]);  // Print the value of CH1
        Serial.print(",");
        Serial.print(Channel[1]);  // Print the value of CH2
        Serial.print(",");
        Serial.print(Channel[2]);  // Print the value of CH3
        Serial.print(",");
        Serial.print(Channel[3]);  // Print the value of CH4
        Serial.print(",");
        Serial.print(Channel[4]);  // Print the value of CH5
        Serial.print(",");
        Serial.println(Channel[5]);  // Print the value of CH6 and start a new line
        delay(100);
        // Check for mode change
      }
      break;
    case 14:  // Odrive Serial Loopback Mode
      while (mode == 14) {
        SetHighCurrent();
        SerialLoopTest();
        // Check for mode change
      }
      break;
    case 15:  // Odrive Encoder and other things Calibration Mode
      while (mode == 15) {
        OdriveConfigure();
        Serial.println("Odrive Config Done, Turn off this option to use other parts of program");
        delay(1000);
        // Here, you would need a method to exit this loop, such as an interrupt or a specific input
      }
      break;
    default:  // Unknown Mode

      Serial.print("Mode ");
      Serial.print(mode);
      Serial.println(" is Unknown - Please debug");
      delay(1000);

      break;
  }
}

int SteerChannel = 0;
int DriveChannel = 0;
unsigned long lastmicros = micros();

void MainProgram() {  // Mode 0 Program
  if (RemoteCommCheck && mode != 5) {
    EnableOuterLoop = RemoteCommOK;
  }
  VoltageDetectOdrive();
  CheckRemoteComm();  // verify if remote is turned on or not
  CheckGyro();        //this checks the data from the gyro to make sure its sensible
  ReadOdrive(500);
  GetDMPdata();
  Tipwatch();
  Set_Speeds();  // set the steering_AMP and Drive AMP settings
  Blink();
  VoltageDetectOdrive();
  scanNum++;
  AxisReset();

  if (UseGripFactor) {
    Wheel_Drive_PID.SetTunings(p[0] * GripFactor, i[0] * GripFactor, d[0] * GripFactor);
  } else {
    Wheel_Drive_PID.SetTunings(p[0], i[0], d[0]);
  }


  float dt = 1e-6 * (micros() - lastmicros);
  lastmicros = micros();

  SteerChannel = Average1(0, Channel[0]);
  if(SteerChannel< 1520 && SteerChannel > 1480){ SteerChannel = 1500;} // deadband steering channel
  DriveChannel = Average1(1, Channel[1]);
  if(DriveChannel < 1580 && DriveChannel > 1420){ DriveChannel = 1500;} // deadband steering channel
  Command_Speed = map(DriveChannel, 1000, 2000, -500, 500);  // is this a speed offset factor

  if ((Battery_Status != 1) && abs(Command_Speed) < 100) {
    AMP = 1;
    Serial.print("BattStatus :");
    Serial.println(Battery_Status);
  } else if (Battery_Status > 2) {
    DisableMotors();
  }

  if (abs(Command_Speed) < SpeedDeadBand) {  // this is a deadband for command speed
    Command_Speed = 0;
  }

  double Speed_Error = Command_Speed - Current_Speed;
  Angle_Error = Current_Angle - Command_Angle;
  Stability = calculateStability(Angle_Error);
    //map2(constrain(abs(Angle_Error),0,35),0,45,100,00); // adjusting this value on the fly should help the car to avoid precarious situations


  //Serial.print("  test11 :");
  //Serial.println(test11);

  if (!EnableOuterLoop) {
    Command_Angle = Balance_Angle;
  }
 


  Wheel_Drive_PID.Compute(0); // the output of this PID is WD_PID_Out_Vector
  Speed_Error = (Current_Speed - Command_Speed);

  CA_PID_I = Current_Speed;  // this is the command angle PID Input
  CA_PID_SP = Command_Speed; // This is the Command Angle PID Setpoint

  //if (abs(Speed_Error) >= SpeedDeadBand && (((abs(Current_Speed)*1.2) > abs(Command_Speed)))) {  // is absolute speed error is less than the deadband
  if (abs(Current_Speed) > abs(Command_Speed)) {
    // the below line needs, work, when you set the number to .78 the command angle is better, but it doesnt slow down as quickly as it should.
    // something should be done to make it not decay the command angle when the car is slowing down.
    //double CA_Decay2 = map2(abs(Speed_Error), 0, 1000, map2(CA_PID_I, 0, 10000, CA_Decay, 1), 1);  //was .78 Adjusts command angle integral decay based on speed error to enhance robot's upright recovery at low speeds
    double CA_Decay2 = map2(abs(Speed_Error), 0, 40000, CA_Decay, 1);
    if (CA_Decay2 < 0) {CA_Decay2 = 0;}
    if (CA_Decay2 > 1) {CA_Decay2 = 1;}
    //Serial.print(" Ca_Decay :");
    //Serial.println(CA_Decay2);
    Cmd_Angle_PID.Compute(CA_Decay2);
    //Cmd_Angle_PID.Compute(0); // The output of this PID is Command_Angle_Out
  } else {
    Cmd_Angle_PID.Compute(0);
  }

  Cmd_Angle_PID.SetOutputLimits(map2(Stability,0,100,0,-MaxLeanAngle),map2(Stability,0,100,0, MaxLeanAngle));
  Wheel_Drive_PID.SetOutputLimits(-(27600.0), (27600.0));

  Command_Angle = moderateCommandAngle(Command_Angle_Out - Balance_Angle);
 
  Current_Speed_Acc = constrain(Current_Speed_Acc,-27600*AMP,27600*AMP);

  if (ResetTipFlag) {
    ResetTipFlag = 0;
    Current_Speed = 0;
    Current_Speed_Acc = 0;
    dt = 0;
    lastmicros = micros();
  } else {
    //Serial.print("WD_PID_Out_Vector: ");
    //Serial.println(WD_PID_Out_Vector);
    //Serial.print("Current_Speed_Acc: ");
    //Serial.println(Current_Speed_Acc);
    double AMP_Correction=map2(AMP,1,20,1,1.6); // this is a bodge, for some reason increasing AMP screws up the inner loop and overtunes it, but nothing is tied to AMP in the inner loop so im confused
    Current_Speed_Acc += (WD_PID_Out_Vector/AMP_Correction) * dt;
    Drive(Current_Speed_Acc, 0);
    Current_Speed = Current_Speed_Acc / AMP; //convert command speed back to an unscaled value by speed
  }
}


double calculateStability(double Angle_Error) {
    // Constrain the angle error to a specific range
    double constrainedError = constrain(abs(Angle_Error), 0, 35);

    // Map the constrained error to a stability value
    double rawStability = map(constrainedError, 0, 35, 100, 0);

    // Implement a dampening factor for when stability increases (moving towards 100)
    static double lastStability = 100; // Assume starting at 100% stability
    double dampeningFactor = 0.01; // Adjust this value to control the rate of increase

    // If the new raw stability is greater than the last stability (increasing confidence),
    // apply the dampening factor to slow down the increase
    if (rawStability > lastStability) {
        lastStability += (rawStability - lastStability) * dampeningFactor;
    } else {
        // If the new raw stability is less than or equal to the last stability (decreasing confidence),
        // allow it to move freely without dampening
        lastStability = rawStability;
    }

    // Ensure the final stability value is within the bounds of 0 to 100
    double finalStability = constrain(lastStability, 0, 100);

    return finalStability;
}






bool LED_STATE = 0;
unsigned long LastTime3 = 0;

void Blink() {
  if (millis() - LastTime3 > 50) {
    LastTime3 = millis();
    LED_STATE = !LED_STATE;
    digitalWrite(ledPin, LED_STATE);
  }
}
double moderateCommandAngle(float inputCommandAngle) {

  if(abs(inputCommandAngle)< no_slow_zone){
    Last_Command_Angle = inputCommandAngle;
    return inputCommandAngle;
    }
  double change = inputCommandAngle - Last_Command_Angle; // Calculate change in angle

  // Determine if movement is towards MaxLeanAngle and needs moderation
  if ((change > 0 && inputCommandAngle > Last_Command_Angle && inputCommandAngle > 0) || 
      (change < 0 && inputCommandAngle < Last_Command_Angle && inputCommandAngle < 0)) {
    if (abs(change) > rate_limit) { // If change is faster than rate_limit
      // Moderate change towards MaxLeanAngle
      inputCommandAngle = Last_Command_Angle + ((change > 0) ? rate_limit : -rate_limit);
    }
  } // If moving towards 0 degrees (vertical), no moderation is applied

  Last_Command_Angle = inputCommandAngle; // Update last command angle for next call

  return inputCommandAngle; // Return the moderated or unchanged command angle
}