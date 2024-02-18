/*
This code was (mostly) written by Ross Rohrer, in Minnesota!
it is for the Rbalance 3V3 and Teensy 4.1, contrilling an Odrive to balance a robot!

*/

#define EnDirection 0  // i think this is steering direction
#define UseGripFactor 1 // use the Z accell to calculate the amount of grip avaliable, and adjust how much torque to apply to the wheels
#define EnableAxisStateCheck 0 // check axis states when tipped and automatically reboot odrive
#define SerialLoopback 0 // this needs to be off for normal operation
#define BlockAllDiag 0 // turn onn to block most of the standard diagnostics
#define OdriveInitialize 0 // turn this on to help with the initial setup on Odrive.  You need to setup Odrives Baud Rate First, and newer firmware will not work
#define EnFreeFallDetect 1 // detect when car is in free fall, and stop PID windup
bool EnableOuterLoop = 1; // Turn this on th enable remote control
#define RemoteCommCheck 1 // disable outer loop when remote is not turned on, this will stop the bootup deathspin.
#define MaxLeanAngle 48
#define Command_Angle_Smooth 0 // Make the command angle change in more controlled of a fashion
double MAX_CURRENT = 70.0; // Max Odrive Current / set to 70 normal operation
double AMP = 7; // Speed amplification setting, turn this up to go faster.
double MaxCPS = 25000 * AMP; // moved here from below setup 2/4/24. this belongs in the loop but will also have to be written to Odrive when changed
double Steer_Amp = 1; // max of like 4 probably. this is the steer speed. currently setting this from the remote
int Battery_Status =0; // 0 is unknown, 1 is good, 2 is unknown, 3 is critical.
#define Reverse 0 // Trying to drive the wrong direction?? Change this!
bool VoltageDetectOdriveEnable = 1;
#define MinBatt 20 // for both batteries in series 19
#define CriticalBatt 19.6 // motor shutdown voltage 18
#define Odrive_Serial_Timeout 1000       // Timeout in milliseconds
#define Balance_Angle 0
unsigned int mode = 0;
float vel_gain = .002;
float vel_integrator_gain = .70;
float encoder_bandwidth = 2000;
const double rate_limit = .21; // sets how quickly the car can attempt to lean away from vertical, in degrees per scan
const float no_slow_zone = 32.0; // How many degrees can the car quickly tip away from vertical before slowing the speed listed above
double Stability = 100; // the current estimated stability of the car, used to try prevent falling down;



double SpeedDeadBand = 30;

/*
 * Each combination of the DIP switches corresponds to a different operational mode, affecting how the car behaves or interacts with debugging tools.
 * 
 * How to Set the DIP Switches:
 * The DIP switches are binary switches where the ON position represents a '1' and the OFF position represents a '0'. Combining the states of these four switches forms a binary number, converted to decimal to select the operational mode.
 * 
 * - Switch 1 is the least significant bit (LSB).
 * - Switch 4 is the most significant bit (MSB).
 * 
 * Modes and Their Descriptions with Bit Positions (Switch 1 to Switch 4):
 * 0 - Standard Mode: Normal operation of the car without any special tuning or debugging. [0,0,0,0]
 * 1 - Tune Speed and Steering: Adjust the car's speed and steering using remote channels 5 and 6. [1,0,0,0]
 * 5 - Tune Inner Loop (Balance) PID: Fine-tune the PID settings for balancing the car using channels 5 and 6. Ch3 to switch between P and D [1,0,1,0]
 * 6 - Tune Outer Loop (Balance Angle) PID: Adjust the PID settings for the car's balance angle using channels 5 and 6. Ch3 to switch between P and D [0,1,1,0]
 * 7 - Tune Odrive Servo PID and Encoder Rate: 5 and 6. Ch3 to switch between P and Encoder rate [1,1,1,0]
 * 10 - MPU6050 Debug Mode: Debug the MPU6050 sensor to ensure accurate readings and performance. [0,1,0,1]
 * 11 - Odrive Debug Mode: Debug the Odrive motor controller for optimal performance. [1,1,0,1]
 * 12 - Remote Debug Mode: Debug remote control communication and responsiveness. [0,0,1,1]
 * 14 - Odrive Serial Loopback Mode: Test the serial communication with the Odrive controller. [0,1,1,1]
 * 15 - Odrive Encoder Calibration Mode: Calibrate the Odrive motor encoders and other sensors for accurate positioning and movement. [1,1,1,1]
 * 
 * To select a mode, set the DIP switches to match the corresponding bit positions. For example, to enter 'MPU6050 Debug Mode', set the switches to [0,1,0,1], which means Switches 2 and 4 should be ON and Switches 1 and 3 should be OFF.
 * 
 * Please refer to the user manual of the RC car and the hardware components for more detailed instructions on how to perform each tuning or debugging operation. This program is designed to provide a versatile platform for both standard operation and advanced adjustments or diagnostics as needed.
 */

#include <PID_v1_bc.h>  // 2023
//#include <ODriveArduino.h>

double p[] = {467, 0.133, 600} ; // This is PID settings for the three PID Loops {wheeldrivePID, Command Angle PID, Steering PID}
double i[] = {180, .28, 286} ;
double d[] = {81, 0, 0};
double CA_Decay = .44; // this decays the command angle by this factor when the car reaches set speed. the car doesnt have to lean anymore if its are the setpoint
bool RemoteCommOK = !RemoteCommCheck;
#define MPU6050_DMP_FIFO_RATE_DIVISOR 0x00 // how fast to check the MPU6050

template<class T> inline Print& operator <<(Print &obj,     T arg) {
  obj.print(arg);
  return obj;
}
template<>        inline Print& operator <<(Print &obj, float arg) {
  obj.print(arg, 4);
  return obj;
}

double Current_Angle, Command_Angle, Command_Angle_Out, Command_Speed = 0, WD_PID_Out_Vector, Speed_Error, Angle_Error, Zero = 0, Current_Speed, CA_PID_I , CA_PID_SP, Current_Speed_Acc = 0;
double CurRotationAngle = 0, ReqRotationAngle = 0, VbusVoltage = 0, GripFactor = 1, SteerRotationAngle = 0, RotationAngleError, GyroLast = 0,BattVoltage[2]; // was .3
int scanNum = 0;

int AxisStates[2];
double MotorCurrent[2];
double MotorVelocity[2];
volatile long RemoteComm = 0;

#define NumTimers 10
unsigned long previousMillis[NumTimers];
unsigned long currentMillis[NumTimers];

PID Wheel_Drive_PID(&Current_Angle, &WD_PID_Out_Vector, &Command_Angle, p[0], i[0], d[0], DIRECT);
PID Cmd_Angle_PID(&CA_PID_I, &Command_Angle_Out, &CA_PID_SP, p[1], i[1], d[1], DIRECT);
PID Rot_Angle_PID(&RotationAngleError, &SteerRotationAngle, &Zero, p[2], i[2], d[2], DIRECT);
int ResetTipFlag = 0;

#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

#define CH1Pin 38
#define CH2Pin 36
#define CH3Pin 34
#define CH4Pin 37
#define CH5Pin 35
#define CH6Pin 33

#define DIP1Pin 32
#define DIP2Pin 31
#define DIP3Pin 30
#define DIP4Pin 29

#define Servo1Pin 2
#define Servo2Pin 3
#define Batt1Pin (A17)
#define Batt2Pin (A16)
#define INTERRUPT_PIN 17
#define LEDPIN1 20
#define LEDPIN2 21
#define LEDPIN3 22
#define LEDPIN4 23
#define  ledPin 13 // this is the built in LED on the TEENSY

MPU6050 mpu(0x69);
bool DIP[4];
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements

Quaternion q;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
  mpuInterrupt = true;
}
