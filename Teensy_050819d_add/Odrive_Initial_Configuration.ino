


void OdriveConfigure() {

  Serial.println("ODriveArduino");
  Serial.println("Setting parameters...");

  for (int axis = 0; axis < 2; ++axis) {
    Serial2 << "w axis" << axis << ".controller.config.vel_limit " << 22000.0f << '\n';
    delay(1);
    Serial2 << "w axis" << axis << ".requested_state " << 4 << '\n'; // state 4 is motor calibration

    Serial.println("The Motor Should now beep");
    delay(5000);


    Serial2 << "w axis" << axis << ".motor.config.pre_calibrated " << 1 << '\n';
    delay(1);
    Serial2 << "w axis" << axis << ".encoder.config.use_index " << 1 << '\n';
    delay(1);
    Serial.println("make sure the wheel is able to move freely so the encoder can calibrate the index position");
    delay(500);
    Serial2 << "w axis" << axis << ".requested_state " << 7 << '\n'; // state 7 . is encoder offset calibration
    delay(5000);
    Serial2 << "w axis" << axis << ".encoder.config.pre_calibrated " << 1 << '\n';
    delay(1);
    Serial2 << "w axis" << axis << ".config.startup_encoder_index_search " << 1 << '\n';
    delay(1);

    Serial2 << "w axis" << axis << ".config.startup_closed_loop_control " << 1 << '\n';
    delay(1);
    Serial2 << "w axis" << axis << ".controller.config.vel_limit_tolerance  " << 20 << '\n';
    delay(1);
    Serial2 << "w axis" << axis << ".encoder.config.bandwidth  " << 2000 << '\n';
    delay(1);
    Serial2 << "w axis" << axis << ".motor.config.calibration_current   " << 20 << '\n';

    delay(1);
    Serial.println("The motor should now enable");
    delay(100);
    Serial2 << "w axis" << axis << ".motor.config.current_lim " << 30.0f << '\n';
    delay(1);

    Serial2 << "w axis" << axis << ".requested_state " << 8 << '\n'; // state 8 closed loop control



  }
  Serial.println("Now you need to use Odrivetool on your computer to to save this configuration before you power the Odrive down");
  Serial.println("enter the commands:");
  Serial.println("odrv0.save_configuration()");
  Serial.println("odrv0.reboot()");


}
