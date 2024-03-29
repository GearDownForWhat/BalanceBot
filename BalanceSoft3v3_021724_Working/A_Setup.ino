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
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin();
    Wire.setClock(400000);  // 400kHz I2C clock. Comment this line if having compilation difficulties
#endif
    mpu.initialize();
    pinMode(INTERRUPT_PIN, INPUT);
    mpu.setRate(9);
    devStatus = mpu.dmpInitialize();
    digitalWrite(LEDPIN2, HIGH);
    Safedelay(100);
    mpu.setXAccelOffset(879);  // 1688 factory default for my test chip
    mpu.setYAccelOffset(794);
    mpu.setZAccelOffset(621);
    mpu.setXGyroOffset(48);
    mpu.setYGyroOffset(-29);
    mpu.setZGyroOffset(-25);

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
          Safedelay(1000);
          Blink();
        }
      }
      Serial.println("OdriveInit");
    }
    digitalWrite(LEDPIN4, HIGH);
    delay(15);
    OdriveInit();
    previousMillis[1] = millis();

    SetHighCurrent();

    Wheel_Drive_PID.SetOutputLimits(-10000, 10000);
    Cmd_Angle_PID.SetOutputLimits(-20, 20);
    //Rot_Angle_PID.SetOutputLimits(-60000, 60000);
    Wheel_Drive_PID.SetMode(AUTOMATIC);
    Cmd_Angle_PID.SetMode(AUTOMATIC);
    //Rot_Angle_PID.SetMode(AUTOMATIC);
    Wheel_Drive_PID.SetSampleTime(5);  // was 5
    Cmd_Angle_PID.SetSampleTime(20);   // was 20
    //Rot_Angle_PID.SetSampleTime(20);   // was 20
    Cmd_Angle_PID.SetControllerDirection(REVERSE);
    Rot_Angle_PID.SetControllerDirection(REVERSE);
    Wheel_Drive_PID.SetTunings(p[0], i[0], d[0]); // fixed 02/06/24
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
  Safedelay(10);
  delay(1000);
  Safedelay(10);
  Serial.println("Boot");
  CheckAxis(0);
  CheckAxis(1);
  //delay(10);
  //Serial3 << "w axis" << 0 << ".controller.config.vel_limit " << (220000.0 * AMP) << '\n';
  //delay(10);
  //Serial3 << "w axis" << 1 << ".controller.config.vel_limit " << (220000.0 * AMP) << '\n';
  //delay(10);

 // Serial3.print("v 0 ");
 // Serial3.println(8300);
 // delay(1000000);
}


