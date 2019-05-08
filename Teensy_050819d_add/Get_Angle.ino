

unsigned long Time = 0;
unsigned long PreTime = 0;
unsigned long PostTime = 0;
int16_t ax, ay, az;
int16_t gx, gy, gz;






void UpdatePositionFast() {

 // Time = micros();

 // double ScanTime = (Time - LastTime);
 // LastTime = Time;
  //double dt = ScanTime / 10000.0;
// if (ScanTime < 5000) {
 //   double Factor =  map(ScanTime, 200, 5000 , 19000, 3400);
    //delayMicroseconds(10);
    mpu.getRotation(&gx, &gy, &gz);

   // Serial.print(ScanTime);
  //  Serial.print("\t");
  //  Serial.print(Factor/100);
  //  Serial.print("\t");
    /// Serial.print(angle2);
    // Serial.println();
    Current_Angle -= (gy / 3500.0); //* dt); / 5000 at 177 microseconds 3300 at 312 ms


    //scantime 9765 and 297 factor is 3400




    
    Serial.print(angle);
    Serial.print("\t");
    Serial.print(Current_Angle);
    Serial.println("\t");
 //}
}

unsigned long bla[10];

/*
void ReadGyro(){

  mpu.getRotation(&gx, &gy, &gz);
  Current_Angle2 -= (gy /19000.0);
}
*/

void Safedelay(unsigned long time2) {
  bla[0] = millis();
  //Serial.println("Safedelay");
Blink();
  
  unsigned long StartTime = micros();
  while ( micros() - StartTime  < time2) {



    GCodeControl();

    
    GetDMPdata(0);
    delayMicroseconds(1);
   
  }
  //Serial.print("ReqTime :");
 // Serial.println(time2);
 // Serial.print("ActualTime :");
 //Serial.println( millis() - bla[0]);

}
