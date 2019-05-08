int LastWriteSpeed = 0;
int SpeedSave = 0;
double writespeed;




int Drive(double writespeedin, bool STOP) {
  double RangeVal = 27600 * AMP;


  if (writespeedin != 999) {
    LastWriteSpeed = SpeedSave;
    SpeedSave = writespeedin;

    writespeed = writespeedin;

    int Steer;
    if (!EnableOuterLoop) {
      Steer = 0;
    }
    else {

      if (EnDirection) {

        if (millis() - previousMillis[2] > 5) {
          ReqRotationAngle += map2(SteerChannel, 1000, 2000, 5, -5 );
          RotationAngleError = angleBetween(CurRotationAngle, ReqRotationAngle);
          Rot_Angle_PID.Compute(0);
          previousMillis[2] = millis();
          Steer = -SteerRotationAngle;

          if (0) {
            Serial.print("\t");
            Serial.print(RotationAngleError);
            Serial.print("\t");
            Serial.print(CurRotationAngle);
            Serial.print("\t");
            Serial.print(ReqRotationAngle);
            Serial.print("\t");
            Serial.print(Steer);
            Serial.println();

          }
          Serial.print(Steer);
            Serial.println();


        }

      }
      else {
        Steer = round(map(SteerChannel, 1000, 2000, 15000, -15000));

      }
    }

    if (abs(Steer) < 100 || STOP)
    {
      Steer = 0;
    }


    double LeftWriteSpeed = round(map2(writespeed, 1000, -1000, RangeVal, -RangeVal)) - Steer;
    double RightWriteSpeed = round(map2(writespeed, 1000, -1000, -RangeVal, RangeVal)) - Steer;

    Serial2.print("v 0 ");
    Serial2.println(LeftWriteSpeed);
    Serial2.print("v 1 ");
    Serial2.println(RightWriteSpeed);

    if (0) {
      Serial.print("\t");
      Serial.print(LeftWriteSpeed);
      //Serial.print("\t");
      //Serial.println(RightWriteSpeed);
      //Serial.print("\t");
      //Serial.println((RightWriteSpeed));
    }
  }

  return LastWriteSpeed;

}

double angleBetween(double angleA, double angleB) {
  angleA = (angleA * 100.0);
  angleB = (angleB * 100.0);
  int PITAU = 36000 + 18000; // for readablility
  int signed_diff = ( (int)angleA - (int)angleB + PITAU ) % 36000 - 18000;
  return signed_diff / 100.0;
}
