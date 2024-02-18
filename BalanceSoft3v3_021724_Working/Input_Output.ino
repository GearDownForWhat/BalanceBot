

void DIP1PC() {
  DIP[0] = !digitalRead(DIP1Pin);
  //Serial.println("I1");
  updateMode();
}
void DIP2PC() {
  DIP[1] = !digitalRead(DIP2Pin);
  //Serial.println("I2");
  updateMode();
}
void DIP3PC() {
  DIP[2] = !digitalRead(DIP3Pin);
  //Serial.println("I3");
  updateMode();
}
void DIP4PC() {
  DIP[3] = !digitalRead(DIP4Pin);
  //Serial.println("I4");
  updateMode();
}
void updateMode() {
  mode = 0;  // Reset mode to 0
  for (int i = 0; i < 4; i++) {
    if (DIP[i]) {
      mode |= (1 << i);  // Set the corresponding bit in mode if the DIP switch is on
    }
  }
  Serial.print(" Mode Changed to: ");
  Serial.println(mode);
}

/*
bool VoltageDetectIO() {

  analogReadResolution(13);
  BattVoltage[0] = map2(analogRead(Batt1Pin), 0, 7640.0, 0, 12.0);
  //Serial.print("batt: ");
  // Serial.print(BattVoltage[0]);
  //Serial.print(",");
  analogReadResolution(13);
  double TotalVoltage = map2(analogRead(Batt2Pin), 0, 7275.0, 0, 24.0);
  BattVoltage[1] = TotalVoltage - BattVoltage[0];
  //Serial.print(BattVoltage[1]);
  //Serial.print(",");
  //Serial.println(VbusVoltage);

  if (BattVoltage[0] < (MinBatt / 2.0) || BattVoltage[1] < (MinBatt / 2.0)) {
    Bat_Low = 1;
    Serial.println("BattLowIO");
    digitalWrite(LEDPIN4, !digitalRead(LEDPIN4));
  }

  if (BattVoltage[0] < (CriticalBatt / 2.0) || BattVoltage[1] < (CriticalBatt / 2.0)) {
    Bat_Low = 2;
  }
}
*/
//unsigned long vbusWarning = 0;                 // Timer for low battery voltage check
//unsigned long previousCriticalBattMillis = 0;  // Timer for critical battery voltage check
//unsigned long previousLowBattMillis = 0;       // Timer for critical battery voltage check
const long lowBattInterval = 3000;      // Interval for checking low battery voltage (60 seconds) was 40000
const long criticalBattInterval = 200;  // Your desired interval for critical battery check was 30000

//unsigned long  vbusWarning = 0;
/*
void VoltageDetectOdrive() {
  unsigned long currentMillis = millis();  // Current time
  if (VbusVoltage != 0) {
    // Check if the voltage is critically low
    if (VbusVoltage < CriticalBatt) {
      if (currentMillis - previousCriticalBattMillis >= criticalBattInterval) {
        digitalWrite(LEDPIN3, HIGH);
        digitalWrite(LEDPIN4, HIGH);
        Battery_Status = 3;  // Set the battery flag to critical
        Serial.print("Battery Critical: ");
        Serial.println(VbusVoltage);
        DisableMotors();                             // Disable motors due to critical battery level
        previousCriticalBattMillis = currentMillis;  // Reset the critical battery timer
      }
    }

    // Check if the voltage is below the low threshold but not critical
    else if (VbusVoltage < MinBatt) {
      
      if (currentMillis - previousLowBattMillis >= lowBattInterval) {
        Battery_Status = 2;  // Set the battery flag to low
        if (!BlockAllDiag) {
          Serial.print("BattLowOdrive: ");
          Serial.println(VbusVoltage);
        }
        previousLowBattMillis = currentMillis;  // Reset the low battery timer
        AMP = 1;
        Steer_Amp = 1;
      }
    }

    else {
        if (millis() - vbusWarning > 5000){

        if(Battery_Status == 1) {Battery_Status = 0;}
        Serial.println("Vbus_Voltage Not Reading");
        vbusWarning = millis();
        }
    }
  }

}
*/

void VoltageDetectOdrive() {
  static unsigned long lowBattStartMillis = 0;       // Timestamp when the voltage first went below the low threshold
  static unsigned long criticalBattStartMillis = 0;  // Timestamp when the voltage first went below the critical threshold
  unsigned long currentMillis = millis();            // Current time

  if (VbusVoltage == 0) {
    // Handle case where VbusVoltage is not being read correctly
    HandleNoReadVbus(currentMillis);
    return;  // Exit the function early
  }

  // Handle critical battery level
  if (VbusVoltage < CriticalBatt) {
    if (criticalBattStartMillis == 0) {  // First time voltage is below critical threshold
      criticalBattStartMillis = currentMillis;
    } else if (currentMillis - criticalBattStartMillis >= criticalBattInterval) {
      // Voltage has been below critical threshold for enough time
      HandleCriticalBattery();
      criticalBattStartMillis = 0;  // Reset the timer
    }
  } else {
    criticalBattStartMillis = 0;  // Reset since voltage is not below critical threshold
  }

  // Handle low but not critical battery level
  if (VbusVoltage >= CriticalBatt && VbusVoltage < MinBatt) {
    if (lowBattStartMillis == 0) {  // First time voltage is below the low threshold
      lowBattStartMillis = currentMillis;
    } else if (currentMillis - lowBattStartMillis >= lowBattInterval) {
      // Voltage has been below low threshold for enough time
      HandleLowBattery();
      lowBattStartMillis = 0;  // Reset the timer
    }
  } else {
    lowBattStartMillis = 0;  // Reset since voltage is not below low threshold
  }

  if(VbusVoltage > MinBatt) {
    if(Battery_Status < 2){ Battery_Status = 1;}
  }

}

void HandleCriticalBattery() {
  digitalWrite(LEDPIN3, HIGH);
  digitalWrite(LEDPIN4, HIGH);
  Battery_Status = 3;  // Set the battery flag to critical
  Serial.print("Battery Critical: ");
  Serial.println(VbusVoltage);
  DisableMotors();  // Disable motors due to critical battery level
}

void HandleLowBattery() {
  Battery_Status = 2;  // Set the battery flag to low
  Serial.print("BattLowOdrive: ");
  Serial.println(VbusVoltage);
  AMP = 1;
  Steer_Amp = 1;
}

void HandleNoReadVbus(unsigned long currentMillis) {
  static unsigned long lastWarningTime = 0;
  if (currentMillis - lastWarningTime > 5000) {  // Only warn every 5 seconds
    if (Battery_Status == 1) {
      Battery_Status = 0;  // Consider setting this to a new status code for "Not Reading"
    }
    Serial.println("Vbus_Voltage Not Reading");
    lastWarningTime = currentMillis;
  }
}
