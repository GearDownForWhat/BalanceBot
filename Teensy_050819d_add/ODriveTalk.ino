#define BUF_SIZE (32)
char buff0[BUF_SIZE];
int buf0Length = 0;
bool absoluteMode = 1;

unsigned long LastTime2 = 0;
int incriment = 0;



void ReadOdrive() {


  int max = 6;
  if ( millis() - LastTime2 > 7 && incriment <= 6 ) {


    //Serial.println("Timerr");
    incriment ++;
    LastTime2 = millis();

    if (incriment == 0) {
      //Serial.println("Vel A0");
      ReqVel(0, 0);
    }
    if (incriment == 1) {
      ReqCurrent(0, 0);
    }
    if (incriment == 2) {
      ReqAxisState(0, 0);
    }
    if (incriment == 3) {
      ReqVbus(0);
    }
    if (incriment == 4) {
      ReqVel(1, 0);
    }
    if (incriment == 5) {
      ReqCurrent(1, 0);
    }
    if (incriment == 6) {
      ReqAxisState(1, 0);
    }

  }
  else if (incriment >= 6) {
    incriment = 0;
  }

}



bool NewSerial = 0;
int ReqAxisState(int Motor, bool Now) {
  if (EnableAxisStateCheck) {
    Safedelay(100);
    buf0Length = 0;
    PreloadBuff('A');
    if (Motor == 0) {
      PreloadBuff('0');
    }
    if (Motor == 1) {
      PreloadBuff('1');
    }
    PreloadBuff(' ');
    PreloadBuff('S');
    //Serial2.println();
    Serial2 << "r axis" << Motor << ".current_state\n";
    //Serial << "r axis" << Motor << ".current_state\n";

    if (Now) {
      NewSerial = 0;

      //Serial.print("Req State Motor :");
      //Serial.println(Motor);

      int comm = 0;
      while (!NewSerial) {
        comm ++;

        Safedelay(100000);
        //Serial.println("SD1");
        if (comm > 10) {
          break;
        }
      }
      return AxisState[Motor];
    }
    return 0;
  }
  else {
    AxisState[Motor] = 8;
    return 8;

  }
}

float ReqVbus(int Now) {
  PreloadBuff('V');
  Serial2.print("r vbus_voltage\n");
  int comm = 0;
  while (!NewSerial) {
    comm ++;

    Safedelay(1);
    //Serial.println("SD2");
    if (comm > 100) {
      break;
    }
    return  VbusVoltage;
  }
  return 0;
}

int ReqCurrent(int Motor, bool Now) {
  PreloadBuff('A');
  if (Motor == 0) {
    PreloadBuff('0');
  }
  if (Motor == 1) {
    PreloadBuff('1');
  }
  PreloadBuff(' ');
  PreloadBuff('C');

  Serial2 << "r axis" << Motor << ".motor.current_meas_phB\n";

  if (Now) {
    NewSerial = 0;
    int comm = 0;
    while (!NewSerial) {
      comm ++;

      Safedelay(1);
      // Serial.println("SD3");
      if (comm > 100) {
        break;
      }
    }
    return  MotorCurrent[Motor];
  }
  // odrv0.axis0.motor.current_meas_phB

  return 0;
}

int ReqVel(int Motor, bool Now) {

  PreloadBuff('A');
  if (Motor == 0) {
    PreloadBuff('0');
  }
  if (Motor == 1) {
    PreloadBuff('1');
  }
  PreloadBuff(' ');
  PreloadBuff('C');
  Serial2 << "r axis" << Motor << ".encoder.vel_estimate\n";


  if (Now) {
    NewSerial = 0;
    int comm = 0;
    while (!NewSerial) {
      comm ++;

      Safedelay(1);
      //Serial.println("SD4");
      if (comm > 100) {
        break;
      }
    }
    return  MotorVelocity[Motor];
  }
  return 0;


}


// odrv0.axis0.motor.current_meas_phB

//odrv0.axis0.encoder.vel_estimate

unsigned TIME[10];
bool Timer(int Number, int Time, bool Start) {
  if (Start) {
    TIME[Number] = millis();
  }
  if (millis() - TIME[Number] > Time) {
    Serial.println("TimerBreak");
    delay(100);
    return 1;

  }
  return 0 ;
}
