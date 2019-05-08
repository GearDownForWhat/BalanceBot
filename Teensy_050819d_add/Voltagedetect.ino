void VoltageDetect(){


 

  if (millis() - previousMillis[3] >= 60000) {
    Bat_Low = 1;
    Serial.println("BattLow");
  }

  

  if (VbusVoltage > MinBatt ) { // if the angle error is less than the max error
    previousMillis[3] = millis(); // reset the timer.
  }
}
