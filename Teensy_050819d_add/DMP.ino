double angle = 0;
unsigned long LastTime = 0;

bool ReadMain = 0;



  // UpdatePositionFast();
  // delay(1);
  // }
  //int Time = micros();
  // double ScanTime = (Time - LastTime);
  // LastTime = Time;
  //   Serial.print(ScanTime);
  //// Serial.print("\t");
  //delay(30);
  //Serial2.print("v 0 ");
  //Serial2.println(-angle * 800.0);
  //Serial2.print(" ");
  //Serial2.println(FF);
  //Serial2.print("v 1 ");
  //Serial2.println(angle * 800.0);

  //Serial2.print("v 0 ");
  //Serial.println(angle);


  //  unsigned long StartTime2 = micros();
  // while ( micros() - StartTime2 < 450) {
  //   UpdatePositionFast();
  





unsigned long LastTime12 = 0;

void GetDMPdata(bool GetAccell) {

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
    Serial.println(F("FIFO overflow!"));

    // otherwise, check for DMP data ready interrupt (this should happen frequently)
  } else if (mpuIntStatus & _BV(MPU6050_INTERRUPT_DMP_INT_BIT)) {
    // wait for correct available data length, should be a VERY short wait
    while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
    mpu.getFIFOBytes(fifoBuffer, packetSize);

    fifoCount -= packetSize;


    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
    


     //mpu.dmpGetQuaternion(&q, fifoBuffer);
     if(EnFreeFallDetect){
     // Serial.println(millis() - LastTime12);
            LastTime12 = millis();
            mpu.dmpGetAccel(&aa, fifoBuffer);
            //mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetLinearAccel(&aaReal, &aa, &gravity);
            mpu.dmpGetLinearAccelInWorld(&aaWorld, &aaReal, &q);
          // if( aaWorld.z > -300) {aaWorld.z = 0;}


          
           aaWorld.z = constrain(aaWorld.z,-5000,-300);
           GripFactor = map2(aaWorld.z,-1000,-5000,1,.2);
           GripFactor = constrain(GripFactor,0,1);

           //Serial.println(GripFactor)
     }


    angle = ypr[1] * 57.2957795;

    double Angle2 = ypr[0] * 57.2957795;

    if (Angle2 < 0){
      CurRotationAngle = (ypr[0] * 57.2957795 +360.0);
    } else{
      CurRotationAngle = Angle2;
    }

   
    Current_Angle = angle;

  }
}
