# BalanceBot

Experience unparalleled speed with our servo-powered balance bot, engineered for performance and precision.

## Hardware Components

- **Teensy 4.1:** The brain of the bot. Available [here](https://amzn.to/48WA4g9).
- **Rbalance 3v3 Board:** My custom design that seamlessly integrates essential components for balance bot projects, including:
  - Direct support for Teensy 4.1.
  - 4 LED indicators.
  - An onboard MPU6050 accelerometer/gyro with optimal orientation (I2C address 0X69).
  - Three potentiometers for fine-tuning PID settings.
  - Four DIP switches for quick configuration changes.
  - Comprehensive IO output options.
  - A 2.2K-ohm resistor-protected serial 3 port to safeguard against potential Odrive and Teensy damage.
  - Voltage divider analog inputs for monitoring battery levels (up to 12V and 24V).
  - Two outputs for R/C servos.
  - Six-channel input for voltage-divided R/C receiver signals (allowing direct receiver connection).
  - Purchase link for Rbalance 3v3 will be provided soon.
- **Odrive 3.6 24V:** For precise motor control.
- **Motors:** 2x 5065 Dual shaft Brushless Outrunners. Available [here](https://odriverobotics.com/shop/odrive-custom-motor-d5065).
- **Encoders:** 2x with cables for accurate position feedback. Available [here](https://odriverobotics.com/shop/cui-amt-102).
- **Batteries:** 2x 2200mAh 3S LiPo for reliable power. Available [here](https://amzn.to/499Znei).
- **Connectors:** Deans Connectors for secure electrical connections. Available [here](https://amzn.to/2YnCuje).
- **R/C System:** Transmitter and Receiver with channels 5 and 6 as potentiometers (also can be used for tuning PID). Available [here](https://amzn.to/42nNXkN).
- **Wheels:** Durable and smooth-running. Available [here](https://amzn.to/3uswwTA).
- **3D Printed Parts:** Custom-designed components from Fusion 360.

## Rbalance 3v3
![BalanceBot Image](IMG_2786.jpg)

## Connections

The wiring for this project is intricate, involving numerous specific connections. For ease of assembly, I recommend contacting me to obtain a prototype board that simplifies this process.

## Software Guidelines

While the provided code is not meant for direct replication in every project, here are key adjustments and installations necessary for optimal performance:

1. **PID Library Modification:** Replace your existing PID library with mine to leverage enhancements for managing integral windup in the control loop.
2. **MPU6050 Library:** Install the necessary library to utilize DMP mode, available [here](https://github.com/jrowberg/i2cdevlib).
3. **Odrive Firmware Customization:** To achieve the desired serial speed and tighten the control loop, follow the instructions [here](https://docs.odriverobotics.com/developer-guide) to build and load custom firmware, adjusting the baud rate as specified.
4. **DMP Calibration:** Use the provided example code for calibrating the MPU6050, substituting the calibration values with those specific to your setup.
5. **Firmware Compatibility:** The code is tailored for an Odrive 3.6 with firmware from late 2019, without plans for updates to accommodate newer firmware versions due to potential compatibility issues.

For those looking to contribute to the software or replicate the setup accurately, ensuring hardware configuration matches mine is crucial for success.


## Amazon Be Happy
We are a participant in the Amazon Services LLC Associates Program, an affiliate advertising program designed to provide a means for us to earn fees by linking to Amazon.com and affiliated sites.
