# BalanceBot
The Stupid fast Servo powered balance bot

This runs on a Teensy 3.6 with an MPU6050 as the IMU and Odrive 3.6 with custom firmware to increase the serial to 921600 baud

I dont expect anyone to use this code line for line, but if you do:

You will need to modify the PID1 Library, I made a change to the library
1. that can be used to clear or reduce the integral band's windup. i need to upload my files for that


2. In order to have the appropate serial speed to tighten your control loop, You will need to install the approprate things to build and load custom firmware on the Odrive, so that you are able to increase the baud rate. Follow Oscars instructions on how to do that found here:
https://docs.odriverobotics.com/developer-guide


Then find this file and open it in text editor:

Firmware/Board/v3/Src/usart.c
and change this line:
huart4.Init.BaudRate = 115200;
To this:
huart4.Init.BaudRate = 921600;
Then build and load the firmware. 



