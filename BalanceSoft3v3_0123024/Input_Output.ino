

void DIP1PC() {
  DIP[0] = !digitalRead(DIP1Pin);
  //Serial.println("I1");
}
void DIP2PC() {
  DIP[1] = !digitalRead(DIP2Pin);
  //Serial.println("I2");
}
void DIP3PC() {
  DIP[2] = !digitalRead(DIP3Pin);
  //Serial.println("I3");
}
void DIP4PC() {
  DIP[3] = !digitalRead(DIP4Pin);
  //Serial.println("I4");
}



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

unsigned long previousLowBattMillis = 0;       // Timer for low battery voltage check
unsigned long previousCriticalBattMillis = 0;  // Timer for critical battery voltage check
const long lowBattInterval = 40000;            // Interval for checking low battery voltage (60 seconds)
const long criticalBattInterval = 30000 /* Your desired interval for critical battery check */;

unsigned long  vbusWarning = 0;

void VoltageDetectOdrive() {
  unsigned long currentMillis = millis();  // Current time
  if (VbusVoltage != 0) {


    // Check if the voltage is critically low
    if (VbusVoltage < CriticalBatt) {
      if (currentMillis - previousCriticalBattMillis >= criticalBattInterval) {
        digitalWrite(LEDPIN3, HIGH);
        digitalWrite(LEDPIN4, HIGH);
        Bat_Low = true;  // Set the low battery flag
        Serial.print("Battery Critical: ");
        Serial.println(VbusVoltage);
        DisableMotors();                             // Disable motors due to critical battery level
        previousCriticalBattMillis = currentMillis;  // Reset the critical battery timer
      }
    }

    // Check if the voltage is below the low threshold but not critical
    else if (VbusVoltage < MinBatt) {
      
      if (currentMillis - previousLowBattMillis >= lowBattInterval) {
        Bat_Low = true;  // Set the low battery flag
        if (!BlockAllDiag) {
          Serial.print("BattLowOdrive: ");
          Serial.println(VbusVoltage);
        }
        previousLowBattMillis = currentMillis;  // Reset the low battery timer
        AMP = 1;
      }
    }

    else {
      if (!BlockAllDiag) {
        if (millis() - vbusWarning > 5000){
        Serial.println("Vbus_Voltage Not Reading");
        vbusWarning = millis();
        }
      }
    }
  }
}
