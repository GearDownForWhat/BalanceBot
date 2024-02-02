void setup() {
  pinMode(LEDPIN1, OUTPUT);
  pinMode(LEDPIN2, OUTPUT);
  pinMode(LEDPIN3, OUTPUT);
  pinMode(LEDPIN4, OUTPUT);

  digitalWrite(LEDPIN1, HIGH);
  Safedelay(100);
  
  Serial.begin(250000);
  Serial3.begin(921600);  //// . BY DEFAULT THE ODrive DOES NOT TALK SERIAL AT THIS SPEED, ODRIVE DEFAULT IS 115200, BUT THAT IS NOW FAST ENOUGH, YOU NEED TO CUSTOMIZE FIRMWARE TO USE THIS SPEED
  if (!SerialLoopback) {
    while (!Serial3) {
      delay(1);
    }
    delay(1000);
    pinMode(ledPin, OUTPUT);
    Blink();
    if (OdriveInitialize) {
      OdriveConfigure();
      while (1) {
        Serial.println("Odrive Config Done, Turn off this option to use other parts of program");
        delay(1000);
      }
    }
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin();
    Wire.setClock(400000);  // 400kHz I2C clock. Comment this line if having compilation difficulties
#endif

    mpu.initialize();
    pinMode(INTERRUPT_PIN, INPUT);
    //mpu.setRate(9);
    devStatus = mpu.dmpInitialize();


    //You may need to calibrate the MPU6050 twice, the first time is a rough calibration, and then the second time make sure your MPU6050 is solidly mounted,
    //Lay the Car on its side and get it to show exactly 90 degrees within .3degs and then run the IMU zero a second time. make sure to select the address 0X69

    //	XAccel			           YAccel				         ZAccel			                 XGyro		             	YGyro			        ZGyro
    // [983,984] --> [-17,13]	[837,838] --> [-10,6]	[617,618] --> [16357,16386]	[53,54] --> [-1,3]	[-36,-35] --> [-2,1]	[-26,-25] --> [0,3]
    //Serial.println("Here2");
    digitalWrite(LEDPIN2, HIGH);
    Safedelay(100);
    // Horizontal Calibtation
    mpu.setXAccelOffset(983);  // 1688 factory default for my test chip
    mpu.setYAccelOffset(837);
    mpu.setZAccelOffset(617);
    mpu.setXGyroOffset(53);
    mpu.setYGyroOffset(-36);
    mpu.setZGyroOffset(-26);


    if (devStatus == 0) {
      mpu.setDMPEnabled(true);
      attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
      mpuIntStatus = mpu.getIntStatus();
      dmpReady = true;
      packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
      if (!BlockAllDiag) {
        Serial.print(F("DMP Initialization failed (code "));
        Serial.print(devStatus);
        Serial.println(F(")"));
      }
    }


    digitalWrite(LEDPIN3, HIGH);
    Safedelay(100);
    Blink();
    Safedelay(3000);
    Drive(0, 1);
    Safedelay(3000);
    Drive(0, 1);

    Safedelay(1000);  // this will read data from the Odrive see if motors are ready
    if (!BlockAllDiag) {
      Serial.print("Battery Voltage is :");
      Serial.println(VbusVoltage);
      if (VbusVoltage < MinBatt && VbusVoltage != 0) {
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
    }
    digitalWrite(LEDPIN4, HIGH);
    delay(10);
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
    Wheel_Drive_PID.SetSampleTime(5);  // was 6
    Cmd_Angle_PID.SetSampleTime(20);   // was 6
    Rot_Angle_PID.SetSampleTime(20);   // was 6
    Cmd_Angle_PID.SetControllerDirection(REVERSE);
    Rot_Angle_PID.SetControllerDirection(REVERSE);
    Cmd_Angle_PID.SetTunings(p[0], i[0], d[0]);
    Cmd_Angle_PID.SetTunings(p[1], i[1], d[1]);

    Serial.println("Boot");

    pinMode(CH1Pin, INPUT_PULLUP);
    pinMode(CH2Pin, INPUT_PULLUP);
    pinMode(CH3Pin, INPUT_PULLUP);
    pinMode(CH4Pin, INPUT_PULLUP);
    pinMode(CH5Pin, INPUT_PULLUP);
    pinMode(CH6Pin, INPUT_PULLUP);
    pinMode(DIP1Pin, INPUT_PULLUP);
    pinMode(DIP2Pin, INPUT_PULLUP);
    pinMode(DIP3Pin, INPUT_PULLUP);
    pinMode(DIP4Pin, INPUT_PULLUP);




    attachInterrupt(digitalPinToInterrupt(CH1Pin), CH1PC, CHANGE);
    attachInterrupt(digitalPinToInterrupt(CH2Pin), CH2PC, CHANGE);
    attachInterrupt(digitalPinToInterrupt(CH3Pin), CH3PC, CHANGE);
    attachInterrupt(digitalPinToInterrupt(CH4Pin), CH4PC, CHANGE);
    attachInterrupt(digitalPinToInterrupt(CH5Pin), CH5PC, CHANGE);
    attachInterrupt(digitalPinToInterrupt(CH6Pin), CH6PC, CHANGE);

    attachInterrupt(digitalPinToInterrupt(DIP1Pin), DIP1PC, CHANGE);
    attachInterrupt(digitalPinToInterrupt(DIP2Pin), DIP2PC, CHANGE);
    attachInterrupt(digitalPinToInterrupt(DIP3Pin), DIP3PC, CHANGE);
    attachInterrupt(digitalPinToInterrupt(DIP4Pin), DIP4PC, CHANGE);


    DIP1PC();
    DIP2PC();
    DIP3PC();
    DIP4PC();
    Serial.print("DIP : ");
    Serial.print(DIP[0]);
    Serial.print(",");
    Serial.print(DIP[1]);
    Serial.print(",");
    Serial.print(DIP[2]);
    Serial.print(",");
    Serial.println(DIP[3]);
    digitalWrite(LEDPIN1, LOW);
    digitalWrite(LEDPIN2, LOW);
    digitalWrite(LEDPIN3, LOW);
    digitalWrite(LEDPIN4, LOW);



    Safedelay(4000000);
    previousMillis[3] = millis();
    GyroLast = Current_Angle;
  }
  //delay(1000);
  //delay(1000);
  Safedelay(10);
  delay(1000);
  Safedelay(10);
  if (!BlockAllDiag) Serial.println("Boot");
  CheckAxis(0);
  CheckAxis(1);
}
double MaxCPS = 25000 * AMP;
