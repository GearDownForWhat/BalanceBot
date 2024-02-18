int LastWriteSpeed = 0;
int SpeedSave = 0;
double writespeed;
// a write speed of 8300counts is equal to 1 rotation per second

int Save[2];

void Set_Speeds() {
  //if (Bat_Low == 0) { // if batteries not low
    if (mode == 1) { //  if no special configurations enabled.
      Steer_Amp = map2(Channel[4], 875, 1500, .5, 2); //set steering speed by Channel 5
      AMP = map2(Channel[5], 875, 1500, 1.2, 10);  // set drive amp settings by channel 6
    }
  //}
}



int Drive(double writespeedin, bool STOP) {


  double RangeVal = 27600;


  if (writespeedin != 999) {
    LastWriteSpeed = SpeedSave;
    SpeedSave = writespeedin;

    writespeed = writespeedin;

    int Steer;
    if (!EnableOuterLoop) {
      Steer = 0;
    } else {

      if (EnDirection) {

        if (millis() - previousMillis[2] > 5) {
          ReqRotationAngle += map2(SteerChannel, 1000, 2000, 5, -5);
          RotationAngleError = angleBetween(CurRotationAngle, ReqRotationAngle);
          //Rot_Angle_PID.Compute(0);
          Rot_Angle_PID.Compute(0);
          previousMillis[2] = millis();
          Steer = -SteerRotationAngle;
          //Serial.print(Steer);
          //Serial.println();
        }

      } else {
        Steer = round(map(SteerChannel, 1000, 2000, (15000 * Steer_Amp* (Stability/100)), (-15000 * Steer_Amp* (Stability/100))));
      }
    }

    if (abs(Steer) < 100 || STOP) {
      Steer = 0;
    }
    double Factor = 1;
    if (Reverse) {
      Factor = -1;
    }
    int LeftWriteSpeed = (round(map2(writespeed, 1000, -1000, RangeVal, -RangeVal)) - Steer) * Factor;
    int RightWriteSpeed = (round(map2(writespeed, 1000, -1000, -RangeVal, RangeVal)) - Steer) * Factor;



    if (Save[0] != LeftWriteSpeed) {
      Serial3.print("v 0 ");
      Serial3.println(LeftWriteSpeed);
      //Serial.print("v ");
      //Serial.println(LeftWriteSpeed);
    }
    if (Save[1] != RightWriteSpeed) {
      Serial3.print("v 1 ");
      Serial3.println(RightWriteSpeed);
      
      
    }
    Save[0] = LeftWriteSpeed;
    Save[1] = RightWriteSpeed;
  }

  return LastWriteSpeed;
}


double angleBetween(double angleA, double angleB) {
  angleA = (angleA * 100.0);
  angleB = (angleB * 100.0);
  int PITAU = 36000 + 18000;  // for readablility
  int signed_diff = ((int)angleA - (int)angleB + PITAU) % 36000 - 18000;
  return signed_diff / 100.0;
}
