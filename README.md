# BalanceBot
The Stupid fast Servo powered balance bot

This runs on a Teensy 3.6 with an MPU6050 as the IMU and Odrive 3.6 with custom firmware to increase the serial to 920Ksomething baud

I dont expect anyone to use this code line for line, but if you do:

You will need to modify the PID1 Library, I made a change to the library
that can be used to clear or reduce the integral band's windup. i need to upload my files for that

