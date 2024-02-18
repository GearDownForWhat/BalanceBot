double angle = 0;
unsigned long LastTime = 0;
bool ReadMain = 0;
unsigned long mpuUpdateRatems = 0;
unsigned long mpuLastUpdate = 0;
double lastAngle = 0;

void Safedelay(unsigned long time2) {
  scanNum++;
  unsigned long StartTime = micros();
  while (micros() - StartTime < time2) {
    ReadOdrive(500);
    Blink();
    ReadOdrive(50);
    GetDMPdata();
    delayMicroseconds(1);
  }
}



unsigned long LastTime12 = 0;

double gyroFixPos = 1;
double gyroFixNeg = -1;
void GetDMPdata() {
  //Serial.println("GEtDMPDATA");

  // if programming failed, don't try to do anything
  if (!dmpReady) return;

  // wait for MPU interrupt or extra packet(s) available
  while (!mpuInterrupt && fifoCount < packetSize) {
    if (mpuInterrupt && fifoCount < packetSize) {
      // try to get out of the infinite loop
      fifoCount = mpu.getFIFOCount();
    }
  }

  // reset interrupt flag and get INT_STATUS byte
  mpuInterrupt = false;
  mpuIntStatus = mpu.getIntStatus();

  // get current FIFO count
  fifoCount = mpu.getFIFOCount();

  // check for overflow (this should never happen unless our code is too inefficient)
  if ((mpuIntStatus & _BV(MPU6050_INTERRUPT_FIFO_OFLOW_BIT)) || fifoCount >= 1024) { 
    // reset so we can continue cleanly
    mpu.resetFIFO();
    fifoCount = mpu.getFIFOCount();
    if (!BlockAllDiag) {
      Serial.println(F("FIFO overflow!"));
    }
    // otherwise, check for DMP data ready interrupt (this should happen frequently)
  } else if (mpuIntStatus & _BV(MPU6050_INTERRUPT_DMP_INT_BIT)) {
    // wait for correct available data length, should be a VERY short wait
    while (fifoCount < packetSize ) fifoCount = mpu.getFIFOCount();
    mpu.getFIFOBytes(fifoBuffer, packetSize);

    fifoCount -= packetSize;


    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);



    //mpu.dmpGetQuaternion(&q, fifoBuffer);
    if (EnFreeFallDetect) {
      // Serial.println(millis() - LastTime12);
      LastTime12 = millis();
      mpu.dmpGetAccel(&aa, fifoBuffer);
      //mpu.dmpGetGravity(&gravity, &q);
      mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
      mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
      // if( aaWorld.z > -300) {aaWorld.z = 0;}



      aaWorld.z = constrain(aaWorld.z, -5000, -300);
      GripFactor = map2(aaWorld.z, -1000, -5000, 1, .2);
      GripFactor = constrain(GripFactor, 0, 1);

      //Serial.println(GripFactor)
    }



    angle = (ypr[1] * 57.2957795) -2.5;//+ 90.0;

    

/*
if(angle != lastAngle){ // calculate how often the MPU Updates
  
  mpuUpdateRatems = millis() - mpuLastUpdate;
  mpuLastUpdate = millis();
  lastAngle = angle;
  Serial.print(" MPUUpdatems: ");
  Serial.println(mpuUpdateRatems);
}
*/

    //Serial.println(angle);
    //PA();

    //Serial.println(angle);
    //delay(5);


    double Angle2 = ypr[0] * 57.2957795;
    

    if (Angle2 < 0) {
      CurRotationAngle = (ypr[0] * 57.2957795 + 360.0);
    } else {
      CurRotationAngle = Angle2;
    }


    Current_Angle += angle;
    Current_Angle = Current_Angle/2.0;
  }
}
// This Function will assure the gyro data isnt crazy, we assume the car cannot possible move too much between readings.
float GyroFailures = 0;
void CheckGyro() {


  if (millis() - previousMillis[6] >= 5) {
    previousMillis[6] = millis();
    double Change = (GyroLast - Current_Angle);

    if (Change > 20) {
      GyroFailures++;
    }
    if (GyroFailures > 5) {
      Serial.println("Gyro Failed");
      mpu.setDMPEnabled(false);
      delay(1);
      devStatus = mpu.dmpInitialize();
      delay(1);
      mpu.resetFIFO();
      delay(1);
      mpu.setDMPEnabled(true);
      Serial.println("Gyro Reset");
      TipRecovery();
    }

    GyroLast = Current_Angle;
  }

  if (millis() - previousMillis[7] >= 1000) {

    previousMillis[7] = millis();
    GyroFailures *= .5;
  }
}
