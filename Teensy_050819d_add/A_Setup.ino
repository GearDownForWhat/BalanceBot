void setup()
{

  Serial.begin(115200);
  Serial1.begin(500000);
  Serial2.begin(921600); //// . BY DEFAULT THE ODrive DOES NO TALK SERIAL AT THIS SPEED, ODRIVE DEFAULT IS 115200, BUT THAT IS NOW FAST ENOUGH, YOU NEED TO CUSTOMIZE FIRMWARE TO USE THIS SPEED

  while (!Serial2) {
    delay(1);
  }
  delay(1000);
  pinMode(ledPin, OUTPUT);
  Blink(1000000);
if (OdriveInitialize){
  OdriveConfigure();
  while(1){
    Serial.println("Odrive Config Done, Turn off this option to use other parts of program");
    delay(1000);
  }
}
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
  Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(4000, true);
#endif
  mpu.initialize();
  pinMode(INTERRUPT_PIN, INPUT);

  devStatus = mpu.dmpInitialize();

  mpu.setXGyroOffset(102);
  mpu.setYGyroOffset(20);
  mpu.setZGyroOffset(-74);
  mpu.setXAccelOffset(1012); // 1688 factory default for my test chip
  mpu.setYAccelOffset(2020);
  mpu.setZAccelOffset(686);

  if (devStatus == 0) {
    mpu.setDMPEnabled(true);
    attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
    mpuIntStatus = mpu.getIntStatus();
    dmpReady = true;
    packetSize = mpu.dmpGetFIFOPacketSize();
  } else {
    Serial.print(F("DMP Initialization failed (code "));
    Serial.print(devStatus);
    Serial.println(F(")"));
  }



  Blink();
  Safedelay(10000);
  //Serial.println("SD8");
  Drive(0, 1);
  Safedelay(10000);
  // Serial.println("SD9");
  Drive(0, 1);

  for (int axis = 0; axis < 2; ++axis) {
    Serial.print("Axis"); Serial.println(axis);
    Serial.print("Velocity"); Serial.println(ReqVel(axis, 1));
    Serial.print("Current"); Serial.println(ReqCurrent(axis, 1));
    Serial.print("State"); Serial.println(ReqAxisState(axis, 1));
    Serial.println();
  }
  ReqVbus(1);
  Serial.print("Vbus :"); Serial.println(VbusVoltage);


  if     (VbusVoltage < MinBatt && VbusVoltage != 0) {
    Serial.println("bat:");
    while (VbusVoltage < MinBatt && VbusVoltage != 0) {
      Serial.print("Battery Voltage is :");

      Serial.print(VbusVoltage);
      Serial.println(" Volts. CHARGE ME");
      //ReqVbus(1);
      Safedelay(1000);
      //Serial.println("SD10");

      Blink();
    }
  }

  Serial.println("OdriveInit");

  OdriveInit();


  // delay(4000);
  previousMillis[1] = millis();
  //lastmicros = micros();


  Wheel_Drive_PID.SetOutputLimits(-10000, 10000);
  Cmd_Angle_PID.SetOutputLimits(-20, 20);
  Rot_Angle_PID.SetOutputLimits(-60000, 60000);
  Wheel_Drive_PID.SetMode(AUTOMATIC);
  Cmd_Angle_PID.SetMode(AUTOMATIC);
  Rot_Angle_PID.SetMode(AUTOMATIC);
  Wheel_Drive_PID.SetSampleTime(5); // was 6
  Cmd_Angle_PID.SetSampleTime(20);// was 6
  Rot_Angle_PID.SetSampleTime(20);// was 6
  Cmd_Angle_PID.SetControllerDirection(REVERSE);
  Rot_Angle_PID.SetControllerDirection(REVERSE);


  //Left.attach(9);
  //Right.attach(8);



  Serial.println("Boot");

  // myTimer.begin(ReadGyro, 10000);
  //noInterrupts();

  // MotorEnable(0);
  //MotorEnable(1);
  //delay(1000);
  pinMode(CH1Pin, INPUT_PULLUP);
  pinMode(CH2Pin, INPUT_PULLUP);
  pinMode(CH3Pin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(CH1Pin), CH1PC, CHANGE);
  attachInterrupt(digitalPinToInterrupt(CH2Pin), CH2PC, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(CH3Pin), CH3PC, CHANGE);

  Safedelay(4000000);
  previousMillis[3] = millis();
  //delay(2000);
}


double MaxCPS = 20000 * AMP;

void OdriveInit() {
  for (int axis = 0; axis < 2; ++axis) {
    Serial2 << "w axis" << axis << ".controller.config.vel_limit " << MaxCPS << '\n';
    Safedelay(1);
    Serial2 << "w axis" << axis << ".motor.config.current_lim " << MAX_CURRENT << '\n';
    Safedelay(1);
    Serial2 << "w axis" << axis << ".motor.config.vel_gain " << .002f << '\n';
    Safedelay(1);
    Serial2 << "w axis" << axis << ".motor.config.vel_integrator_gain " << .5f << '\n';
  }
}

void AxisReset() {

  Serial.print("State"); Serial.println(ReqAxisState(0, 1));
  Serial.print("State"); Serial.println(ReqAxisState(1, 1));

  //AxisStateCheck(0);
  while  (AxisState[0] != 8 || AxisState[1] != 8 ) {
    ReqAxisState(0, 1);
    //Safedelay(10);
    ReqAxisState(1, 1);
    // Safedelay(10);
    //ReqAxisState(
    //AxisStateCheck(0);
    //Timer(0,2000,1);
    Serial.print("Axis_State : ");
    Serial.println(AxisState[0]);
    Safedelay(3000);
    //Serial2.print();
    Serial2.println("sb");

    Safedelay(3000);
    //Serial.println("SD11");
    OdriveInit();
    Safedelay(1000);
    //Serial.println("SD12");

    // Serial.println("loop");
  }
  //Serial.println("loop2");
}
