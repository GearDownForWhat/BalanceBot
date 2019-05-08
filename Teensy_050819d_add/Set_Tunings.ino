

void SetPIDTunings1() {





  currentMillis[0] = millis();


  if (currentMillis[0] - previousMillis[0] >= 300) {
    previousMillis[0] = millis();

    // double p[] = {600, 0.01146} ;
    //double i[] = {200, .026} ;
    //double d[] = {60,0};


    p[0] =  map2(analogRead(A0), 1023, 0, 300, 1240);
    // (((float)map(analogRead(A1), 1023, 0, 20, 100)));
    i[0] =  map2(analogRead(A1), 1023, 0, 300, 1200);

    d[0] =  map2(analogRead(A2), 1023, 0, 30, 60);
    Wheel_Drive_PID.SetTunings( p[0], i[0], d[0]);


    if (1) {
      /*
        Serial.print("  A0: ");
        Serial.print((analogRead(A0), 5));
        Serial.print("  A1: ");
        Serial.print((analogRead(A1), 5));
        Serial.print("  A2: ");
        Serial.println((analogRead(A2), 5));
      */
      //Serial.print("  Command_Angle :");
      //Serial.print(Command_Angle);
      //Serial.print("  Current_Angle :");
      // Serial.print(Current_Angle);
      // Serial.print("  \toutput :");
      // Serial.print(output);
      Serial.print("  \tP_term: ");
      Serial.print(Wheel_Drive_PID.GetKp(), 5);
      Serial.print("  \tI_term: ");
      Serial.print(Wheel_Drive_PID.GetKi(), 5);
      Serial.print("  \tD_term: ");
      Serial.print(Wheel_Drive_PID.GetKd(), 5);
      Serial.println();
      delay(1);

    }
  }
  //Serial.println("Leaving ST");
}


void SetPIDTunings2() {





  currentMillis[0] = millis();


  if (currentMillis[0] - previousMillis[0] >= 1000) {
    previousMillis[0] = millis();

    //d[1] = (((float)map(analogRead(A2), 1023, 0, 0, 3)) / 1000.0);
    //float factor = (((float)map(analogRead(A2), 1023, 0, 0, 2000)) / 1000.0);

    // double p[] = {600, 0.01146} ;
    //double i[] = {200, .026} ;
    //double d[] = {60,0};

    p[1] = (((float)map(analogRead(A0), 1023, 0, 1000, 2000)) / 100000.0);
    // (((float)map(analogRead(A1), 1023, 0, 20, 100)));
    i[1] = (((float)map(analogRead(A1), 1023, 0, 1500, 5000)) / 100000.0);

    CA_Decay = (((float)map(analogRead(A2), 1023, 0, 85000, 99999)) / 100000.0);


    //  double p[] = {161.72,0.01460} ;
    //double i[] = {0, 0.03000} ;
    //double d[] = {12.54000,0};


    Cmd_Angle_PID.SetTunings( p[1], i[1], d[1]);

    if (1) {
      /*
        Serial.print("  A0: ");
        Serial.print((analogRead(A0), 5));
        Serial.print("  A1: ");
        Serial.print((analogRead(A1), 5));
        Serial.print("  A2: ");
        Serial.println((analogRead(A2), 5));
      */
      //  Serial.print("  Command_Angle :");
      // Serial.print(Command_Angle);
      //// Serial.print("  Current_Angle :");
      //Serial.print(Current_Angle);
      //Serial.print("  \toutput :");
      //Serial.print(output);
      Serial.print("  \tP_term: ");
      Serial.print(Cmd_Angle_PID.GetKp(), 5);
      Serial.print("  \tI_term: ");
      Serial.print(Cmd_Angle_PID.GetKi(), 5);
      Serial.print("CA_Decay");
      Serial.print(CA_Decay, 20);
      Serial.println();

    }
  }
  //Serial.println("Leaving ST");
}
void SetPIDTunings3() {





  currentMillis[0] = millis();


  if (currentMillis[0] - previousMillis[0] >= 1000) {
    previousMillis[0] = millis();

    //d[1] = (((float)map(analogRead(A2), 1023, 0, 0, 3)) / 1000.0);
    //float factor = (((float)map(analogRead(A2), 1023, 0, 0, 2000)) / 1000.0);

    // double p[] = {600, 0.01146} ;
    //double i[] = {200, .026} ;
    //double d[] = {60,0};

    p[2] =  map2(analogRead(A0), 1023, 0, 600, 6000);
    // (((float)map(analogRead(A1), 1023, 0, 20, 100)));
    i[2] =  map2(analogRead(A1), 1023, 0, 10, 2200);

    d[2] =  map2(analogRead(A2), 1023, 0, 0, 100);


    //  double p[] = {161.72,0.01460} ;
    //double i[] = {0, 0.03000} ;
    //double d[] = {12.54000,0};


    Rot_Angle_PID.SetTunings( p[2], i[2], d[2]);

    if (1) {
      /*
        Serial.print("  A0: ");
        Serial.print((analogRead(A0), 5));
        Serial.print("  A1: ");
        Serial.print((analogRead(A1), 5));
        Serial.print("  A2: ");
        Serial.println((analogRead(A2), 5));
      */
      //  Serial.print("  Command_Angle :");
      // Serial.print(Command_Angle);
      //// Serial.print("  Current_Angle :");
      //Serial.print(Current_Angle);
      //Serial.print("  \toutput :");
      //Serial.print(output);
      Serial.print("  \tP_term: ");
      Serial.print(Rot_Angle_PID.GetKp(), 5);
      Serial.print("  \tI_term: ");
      Serial.print(Rot_Angle_PID.GetKi(), 5);
      Serial.print("  \tD_term: ");
      Serial.print(Rot_Angle_PID.GetKd(), 5);

      Serial.println();

    }
  }
  //Serial.println("Leaving ST");
}
