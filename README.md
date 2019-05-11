# BalanceBot
The Stupid fast Servo powered balance bot

## Hardware
 - Teensy 3.6 https://amzn.to/2Q0A1rL
 - MPU-6050 (you need GY-521 with the internal low dropout regulator) https://amzn.to/2JEKfgB
 - Odrive 3.6 24V
 - 2x 5065 Dual shaft Brushless Outrunner https://odriverobotics.com/shop/odrive-custom-motor-d5065
 - 2X Encoders https://odriverobotics.com/shop/cui-amt-102
 - 2 or 4 2200 3S mAh Lipo batteries https://amzn.to/2LxU3vc
 - Deans Connectors https://amzn.to/2YnCuje
 - R/C Transmitter and Reciever 
 - Assorted Resistors https://amzn.to/2Vhlzgb
## Connections
  - We will be using resistors in several places to protect the serial bus on the Odrive, and we will be creating a voltage divider to drop the voltage of the R/C Reciever to 3.3V





## Software

I dont expect anyone to use this code line for line, but if you do:

1. You will need to replace your PID Library with mine, I made a change that can be used to clear or reduce the integral band's windup. This is used in the "outer loop"

2. You need to Install this library for the MPU6050 to use DMP Mode: https://github.com/jrowberg/i2cdevlib


3. In order to have the appropate serial speed to tighten your control loop, You will need to install the approprate things to build and load custom firmware on the Odrive, so that you are able to increase the baud rate. Follow Oscars instructions on how to do that found here:
https://docs.odriverobotics.com/developer-guide Then find this file and open it in text editor: Firmware/Board/v3/Src/usart.c and change this line:
huart4.Init.BaudRate = 115200;
To this:
huart4.Init.BaudRate = 921600;
Then build and load the firmware. 


4. You need to use DMP calibration example code and instructions and swap my calibration values with yours
  I.E.
  mpu.setXGyroOffset(102);
  mpu.setYGyroOffset(20);
  mpu.setZGyroOffset(-74);
  mpu.setXAccelOffset(1012); 
  mpu.setYAccelOffset(2020);
  mpu.setZAccelOffset(686);



## Amazon Be Happy
We are a participant in the Amazon Services LLC Associates Program, an affiliate advertising program designed to provide a means for us to earn fees by linking to Amazon.com and affiliated sites.
