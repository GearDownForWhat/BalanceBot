# BalanceBot
The Stupid fast Servo powered balance bot

## Hardware
 - Teensy 4.1 https://amzn.to/48WA4g9
 - Rbalance 3v3 (designed by me) Link to come soon!
 - Odrive 3.6 24V
 - 2x 5065 Dual shaft Brushless Outrunner https://odriverobotics.com/shop/odrive-custom-motor-d5065
 - 2X Encoders https://odriverobotics.com/shop/cui-amt-102
 - 2 or 4 2200 3S mAh Lipo batteries https://amzn.to/499Znei
 - Deans Connectors https://amzn.to/2YnCuje
 - R/C Transmitter and Reciever https://amzn.to/42nNXkN use one with channels 5 and 6 as potentiometers, and make sure the reciever doesnt stick out when plugged into Rbalance 3v3 https://amzn.to/42nNXkN
 - Wheels https://amzn.to/3uswwTA
 - 3D Printed Parts from Fusion 360 design
## Connections
 - The wiring for this project is very specific, and there is a lot of it. I would highly suggest you send me an email and i can send you a prototype board that i've been working on to make this project easier.




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


4. You need to use DMP calibration example code included with the MPU6050 Library, use the address 0X69 and instructions and swap my calibration values with yours
  I.E.
  mpu.setXGyroOffset(102);
  mpu.setYGyroOffset(20);
  mpu.setZGyroOffset(-74);
  mpu.setXAccelOffset(1012); 
  mpu.setYAccelOffset(2020);
  mpu.setZAccelOffset(686);

5. All of this code is talking to my 5 year old Odrive 3.6 of which i REFUSE to update because SCARY!. this Odrive has taken a beating, ane due to my lazyness this code will probaably not talk to an Odrive with new firmware. I believe oscar has changed the communicaiton style with the never firmware also, so if you want this to work without any issues maybe look for an old version of Odrive firmware from late 2019??  So sorry for this. 




## Amazon Be Happy
We are a participant in the Amazon Services LLC Associates Program, an affiliate advertising program designed to provide a means for us to earn fees by linking to Amazon.com and affiliated sites.
