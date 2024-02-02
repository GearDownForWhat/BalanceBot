/*
#define BUF_SIZE (32)
char buff0[BUF_SIZE];
char Clear[BUF_SIZE];
int buf0Length = 0;
bool absoluteMode = 1;

unsigned long LastTime2 = 0;
int incriment = 0;
*/
void SerialLoopTest() {

  if (Serial3.available())
    Serial.write(Serial3.read());
  if (Serial.available())
    Serial3.write(Serial.read());
}

unsigned long oDriveComWarning = 0;
void ReadOdrive(int Odrive_Request_Interval) {
  static unsigned long lastRequestTime = 0;
  static unsigned long lastResponseTime = 0;
  static int motor = 0;  // Initialize with axis0
  static const char* commands[] = {
    "r axis0.current_state\n",
    "r axis0.motor.current_meas_phB\n",
    "r axis0.encoder.vel_estimate\n",
    "r vbus_voltage\n",
    "r axis1.current_state\n",
    "r axis1.motor.current_meas_phB\n",
    "r axis1.encoder.vel_estimate\n"
  };
  static int currentCommandIndex = 0;
  static String receivedNumberString = "";
  static bool waitingForResponse = false;

  unsigned long currentTime = millis();

  // Send a new command if the interval has passed and we are not waiting for a response
  if (!waitingForResponse && (currentTime - lastRequestTime >= Odrive_Request_Interval)) {
    Serial3.print(commands[currentCommandIndex]);
    lastRequestTime = currentTime;
    lastResponseTime = currentTime;
    waitingForResponse = true;
  }

  // Check for timeout
  if (waitingForResponse && (currentTime - lastResponseTime >= Odrive_Serial_Timeout)) {

    if (!BlockAllDiag) {
      if (millis() - oDriveComWarning > 7000){
      Serial.println("Timeout waiting for ODrive response.");
      oDriveComWarning = millis();
      }
      }
    waitingForResponse = false;
    currentCommandIndex = (currentCommandIndex + 1) % 7;
    return;  // Skip further processing and wait for the next request
  }

  // Check if there is data available from Serial3
  while (Serial3.available()) {
    char c = Serial3.read();

    if ((c >= '0' && c <= '9') || c == '.' || c == '-') {
      receivedNumberString += c;
    } else if (c == '\n') {
      // Debug print for the received raw string
      //Serial.print("Received String: ");
      //Serial.println(receivedNumberString);

      double receivedNumber = atof(receivedNumberString.c_str());

      // Debug print for the parsed double value
      //Serial.print("Parsed Double: ");
      //Serial.println(receivedNumber, 6);  // Print the parsed number for debugging

      switch (currentCommandIndex) {
        case 0:
          AxisStates[motor] = (int)receivedNumber;
          //Serial.print("AxisStates[");
          //Serial.print(motor);
          //Serial.print("]: ");
          //Serial.println(AxisStates[motor]);
          break;
        case 1:
          MotorCurrent[motor] = receivedNumber;
          //Serial.print("MotorCurrent[");
          //Serial.print(motor);
          //Serial.print("]: ");
          //Serial.println(MotorCurrent[motor], 6);  // Assuming double precision is required
          break;
        case 2:
          MotorVelocity[motor] = receivedNumber;
          //Serial.print("MotorVelocity[");
          //Serial.print(motor);
          //Serial.print("]: ");
          //Serial.println(MotorVelocity[motor], 6);
          break;
        case 3:
          VbusVoltage = receivedNumber;
          //Serial.print("VbusVoltage: ");
          //Serial.println(VbusVoltage, 6);
          break;
        case 4:
          AxisStates[motor] = (int)receivedNumber;
          //Serial.print("AxisStates[");
          //Serial.print(motor);
          //Serial.print("]: ");
          //Serial.println(AxisStates[motor]);
          break;
        case 5:
          MotorCurrent[motor] = receivedNumber;
          //Serial.print("MotorCurrent[");
          //Serial.print(motor);
          //Serial.print("]: ");
          //Serial.println(MotorCurrent[motor], 6);
          break;
        case 6:
          MotorVelocity[motor] = receivedNumber;
          //Serial.print("MotorVelocity[");
          //Serial.print(motor);
          //Serial.print("]: ");
          //Serial.println(MotorVelocity[motor], 6);
          break;
      }

      receivedNumberString = "";
      waitingForResponse = false;
      currentCommandIndex = (currentCommandIndex + 1) % 7;

      if (currentCommandIndex == 0 || currentCommandIndex == 4) {
        motor = (motor + 1) % 2;  // Switch between axis0 and axis1
      }
    }
  }
}

void OdriveInit() {
  for (int axis = 0; axis < 2; ++axis) {
    Safedelay(1000);
    Serial3 << "w axis" << axis << ".controller.config.vel_limit " << MaxCPS << '\n';
    Safedelay(1000);
  }
  SetHighCurrent();
}


void SetLowCurrent() {
  for (int axis = 0; axis < 2; ++axis) {
    Serial3 << "w axis" << axis << ".motor.config.current_lim " << (MAX_CURRENT / 10) << '\n';
    Safedelay(1000);
    Serial3 << "w axis" << axis << ".controller.config.vel_gain " << .00002f << '\n';  // set to .002
    Safedelay(1000);
    Serial3 << "w axis" << axis << ".controller.config.vel_integrator_gain " << .007f << '\n';  // set to .7
    Safedelay(1000);
  }
}
void SetHighCurrent() {
  for (int axis = 0; axis < 2; ++axis) {
    Serial3 << "w axis" << axis << ".motor.config.current_lim " << MAX_CURRENT << '\n';
    Safedelay(1000);
    Serial3 << "w axis" << axis << ".controller.config.vel_gain " << .002f << '\n';  // set to .002
    Safedelay(1000);
    Serial3 << "w axis" << axis << ".controller.config.vel_integrator_gain " << .7f << '\n';  // set to .7
    Safedelay(1000);
  }
}

void DisableMotors() {

  Serial3 << "w axis" << 0 << ".requested_state " << 1 << '\n';  // state 7 . is encoder offset calibration
  delay(500);
  Serial3 << "w axis" << 1 << ".requested_state " << 1 << '\n';  // state 7 . is encoder offset calibration
  delay(100);
  Safedelay(1000);
  Serial.println("Motors Disabled");

  while (1) {
    digitalWrite(LEDPIN3, !digitalRead(LEDPIN3));
    digitalWrite(LEDPIN4, !digitalRead(LEDPIN4));
    Serial.println("Servo Operation has been halted");
    Serial.println("please check the battery voltages and reboot to continue");
    Serial.print("VbusVoltage: ");
    Serial.println(VbusVoltage, 6);
    Safedelay(10000);
  }
}
void CheckAxis(int Axis) {
  //Serial3 << "w axis" << Axis << ".requested_state " << 1 << '\n';  // state 8 . is Enabled
  if(AxisStates[0] !=8){
    Serial.print("Axis Disabled");
    Serial.print(Axis);
    Serial.println("Fixing");

    //Serial3 << "w axis" << Axis << ".requested_state " << 7 << '\n';  // state 8 . is Enabled
    //Serial << "SENT: w axis" << Axis << ".requested_state " << 7 << '\n';  // state 8 . is Enabled
    //Safedelay(3000);
    //Serial3 << "w axis" << Axis << ".requested_state " << 8 << '\n';  // state 8 . is Enabled
    //Serial << "SENT: w axis" << Axis << ".requested_state " << 8 << '\n';  // state 8 . is Enabled
    Serial3 << "sb" << '\n';  // sb is reboot
    Safedelay(2000);
    


  }
  }


// odrv0.axis0.motor.current_meas_phB

//odrv0.axis0.encoder.vel_estimate

unsigned TIME[10];
bool Timer(int Number, int Time, bool Start) {
  if (Start) {
    TIME[Number] = millis();
  }
  if (millis() - TIME[Number] > Time) {
    Serial.println("TimerBrokennn?");
    delay(100);
    return 1;
  }
  return 0;
}
