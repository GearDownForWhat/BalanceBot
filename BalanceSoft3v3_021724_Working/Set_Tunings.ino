float P0_Center = p[0];
float I0_Center = i[0];
float D0_Center = d[0];

void SetPIDTunings1() {
digitalWrite(LEDPIN1, HIGH);
  digitalWrite(LEDPIN2, HIGH);

  currentMillis[0] = millis();
  if (currentMillis[0] - previousMillis[0] >= 100) {
    previousMillis[0] = millis();
    if (Channel[3] > 1500) {
      d[0] = map2(Channel[4], 875, 1500, (D0_Center*.3), D0_Center);
      //p[0] = P0_Center;

    } else {
      p[0] = map2(Channel[4], 875, 1500, (P0_Center*.3), P0_Center);
      //d[0] = D0_Center;
      
    }

    // (((float)map(analogRead(A1), 1023, 0, 20, 100)));
    i[0] = map2(Channel[5], 875, 1500, (I0_Center*.3), I0_Center);


    Wheel_Drive_PID.SetTunings(p[0], i[0], d[0]);


    if (1 && !BlockAllDiag) {

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

float CA_Center = CA_Decay;
float P1_Center = p[1];
float I1_Center = i[1];

void SetPIDTunings2() {
  digitalWrite(LEDPIN2, HIGH);
  digitalWrite(LEDPIN3, HIGH);
  currentMillis[0] = millis();
  if (currentMillis[0] - previousMillis[0] >= 100) {



    previousMillis[0] = millis();
    if (Channel[3] > 1500) {
      p[1] = map2(Channel[4], 875, 1500, (P1_Center*.2), P1_Center); // try center 
      //CA_Decay = CA_Center;
    } else {
      CA_Decay = map2(Channel[4], 875, 1500, .1, .9);
      if(CA_Decay > 1){CA_Decay = 1;}

      //p[1] = P1_Center;
    }
    // (((float)map(analogRead(A1), 1023, 0, 20, 100)));
    i[1] = map2(Channel[5], 875, 1500, (I1_Center*.2), I1_Center);

    Serial.print("Cal Outer Loop");
    Serial.print("P :");
    Serial.print(p[1],7);
    Serial.print(" I :");
    Serial.print(i[1],7);
    Serial.print(" CA :");
    Serial.println(CA_Decay,8);

    //d[2] =  map2(analogRead(A2), 1023, 0, 30, 60);



    Cmd_Angle_PID.SetTunings(p[1], i[1], d[1]);

    if (0 && !!BlockAllDiag) {

      Serial.print("  \tP_term: ");
      Serial.print(Cmd_Angle_PID.GetKp(), 5);
      Serial.print("  \tI_term: ");
      Serial.print(Cmd_Angle_PID.GetKi(), 5);
      //Serial.print("CA_Decay");
      //Serial.print(CA_Decay, 20);
      Serial.println();
    }
  }
  //Serial.println("Leaving ST");
}
void SetPIDTunings3() {
  digitalWrite(LEDPIN3, HIGH);
  digitalWrite(LEDPIN4, HIGH);
  Serial.println("SetTunings3");





  currentMillis[0] = millis();


  if (currentMillis[0] - previousMillis[0] >= 1000) {
    previousMillis[0] = millis();

    //d[1] = (((float)map(analogRead(A2), 1023, 0, 0, 3)) / 1000.0);
    //float factor = (((float)map(analogRead(A2), 1023, 0, 0, 2000)) / 1000.0);

    // double p[] = {600, 0.01146} ;
    //double i[] = {200, .026} ;
    //double d[] = {60,0};

    p[2] = map2(analogRead(A0), 1023, 0, 600, 6000);
    // (((float)map(analogRead(A1), 1023, 0, 20, 100)));
    i[2] = map2(analogRead(A1), 1023, 0, 10, 2200);

    d[2] = map2(analogRead(A2), 1023, 0, 0, 100);


    //  double p[] = {161.72,0.01460} ;
    //double i[] = {0, 0.03000} ;
    //double d[] = {12.54000,0};


    Rot_Angle_PID.SetTunings(p[2], i[2], d[2]);

    if (1 && !BlockAllDiag) {
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

float vel_gain_Center = vel_gain;
float vel_integrator_gain_Center = vel_integrator_gain;
float encoder_bandwidth_Center = encoder_bandwidth;

void SetOdriveTunings() {
      currentMillis[0] = millis();
  if (currentMillis[0] - previousMillis[0] >= 1000) {
    Serial.print("Cal Odrive");
    previousMillis[0] = millis();
    if (Channel[2] > 1500) {
      vel_gain = map2(Channel[4], 875, 1500, (vel_gain_Center*.5), vel_gain_Center); // try center 
       Serial.print(" vel_gain :");
      Serial.print(vel_gain,7);
    } else {
      encoder_bandwidth = map2(Channel[4], 875, 1500, (encoder_bandwidth_Center*.5), encoder_bandwidth_Center);
      Serial.print(" encoder_bandwidth :");
      Serial.print(encoder_bandwidth,8);
    }
    // (((float)map(analogRead(A1), 1023, 0, 20, 100)));
    vel_integrator_gain = map2(Channel[5], 875, 1500, (vel_integrator_gain_Center*.5), vel_integrator_gain_Center);

    
   
    Serial.print("vel_integrator_gain :");
    Serial.println(vel_integrator_gain,7);
    
     MainProgram();
    SetHighCurrent();
    MainProgram();
    Safedelay(100);
    Serial3 << "w axis" << 0 << ".encoder.config.bandwidth  " << encoder_bandwidth << '\n';
    Safedelay(100);
    Serial3 << "w axis" << 1 << ".encoder.config.bandwidth  " << encoder_bandwidth << '\n';
    Safedelay(100);

  }
  //Serial.println("Leaving ST");
}
