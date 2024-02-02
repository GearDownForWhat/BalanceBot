/*
This code was (mostly) written by Ross Rohrer, in Minnesota!
it is for the Rbalance 3V3 and Teensy 4.1, contrilling an Odrive to balance a robot!



*/

#define EnDirection 0  // i think this is steering direction
#define UseGripFactor 0 // use the Z accell to calculate the amount of grip avaliable
#define EnableAxisStateCheck 0 // check axis states when tipped and automatically reboot odrive
#define SerialLoopback 0 // this needs to be off for normal operation
#define BlockAllDiag 0 // turn onn to block most of the standard diagnostics
#define OdriveInitialize 0 // turn this on to help with the initial setup on Odrive.  You need to setup Odrives Baud Rate First, and newer firmware will not work
#define EnFreeFallDetect 1 // detect when car is in free fall, and stop PID windup
bool EnableOuterLoop = 1; // Turn this on th enable remote control
#define RemoteCommCheck 1 // disable outer loop when remote is not turned on, this will stop the bootup deathspin.
#define GoFaster 1 // allow machine to lean further
#define Command_Angle_Smooth 1 // Make the command angle change in more controlled of a fashion
#define debug 1 // show command angle current angle command speed current speed
double MAX_CURRENT = 70.0; // Max Odrive Current / set to 70 normal operation
double AMP = 3; // Speed amplification setting, turn this up to go faster.
double Steer_Amp = 1; // max of like2 probably. this is the steer speed;
int Bat_Low = 0;
#define Reverse 0 // Trying to drive the wrong direction?? Change this!
#define VoltageDetectIOEnable 0 //broken122323
#define VoltageDetectOdriveEnable 1
#define MinBatt 19 // for both batteries in series 19
#define CriticalBatt 18 // motor shutdown voltage 18
#define LimitIntegralWindup1 1
#define LimitIntegralWindup2 0
#define Odrive_Serial_Timeout 1000       // Timeout in milliseconds



double SpeedDeadBand = 20;
// DIP SWITCHES READ IN LOOP
// DIP 1 = Wheel Drive PID Tunings By Pot
// DIP 2 = Wheel Drive PID Sed Default Tunings


//#include <PID_v1.h> // 2020
#include <PID_v1_bc.h>  // 2023
#include <ODriveArduino.h>

//double p[] = {782, 0.0152, 600} ; // This is PID settings for the three PID Loops {wheeldrivePID, Command Angle PID, Steering PID}
//double i[] = {813, .0358, 286} ;
//double d[] = {48, 0, 0};
double p[] = {782, 0.0152, 600} ; // This is PID settings for the three PID Loops {wheeldrivePID, Command Angle PID, Steering PID}
double i[] = {1185, .0358, 286} ;
double d[] = {50, 0, 0};
double CA_Decay = .48; // this decays the command angle by this factor when the car reaches set speed. the car doesnt have to lean anymore if its are the setpoint



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

ODriveArduino odrive(Serial3);

double Current_Angle, Command_Angle, Command_Angle_Out, Command_Speed = 0, WD_PID_Out_Vector, Speed_Error, Angle_Error, Zero = 0, Current_Speed, CA_PID_I , CA_PID_SP;
double CurRotationAngle = 0, ReqRotationAngle = 0, VbusVoltage = 0, GripFactor = 1, SteerRotationAngle = 0, RotationAngleError, GyroLast = 0,BattVoltage[2]; // was .3
int scanNum = 0;

//int AxisState[2];AnglTempCode
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
//#define INTERRUPT_PIN 20 ONBOARD MPU6050
#define INTERRUPT_PIN 17
#define LEDPIN1 20
#define LEDPIN2 21
#define LEDPIN3 22
#define LEDPIN4 23
#define  ledPin 13 // this is the built in LED on the TEENSY

MPU6050 mpu(0x69);
const int MPU6050_ADDRESS = 0x69; 
// MPU6050 [-1573,-1572] --> [-11,5]	[-641,-640] --> [-13,7]	[913,914] --> [16358,16390]	[124,124] --> [0,6]	[-56,-55] --> [0,2]	[-14,-13] --> [0,4][-1573,-1572] --> [-11,5]	[-641,-640] --> [-13,7]	[913,914] --> [16358,16390]	[124,124] --> [0,6]	[-56,-55] --> [0,2]	[-14,-13] --> [0,4][-1573,-1572] --> [-11,5]	[-641,-640] --> [-13,7]	[913,914] --> [16358,16390]	[124,124] --> [0,6]	[-56,-55] --> [0,2]	[-14,-13] --> [0,4][-1573,-1572] --> [-11,5]	[-641,-640] --> [-13,7]	[913,914] --> [16358,16390]	[124,124] --> [0,6]	[-56,-55] --> [0,2]	[-14,-13] --> [0,4][-1573,-1572] --> [-11,5]	[-641,-640] --> [-13,7]	[913,914] --> [16358,16390]	[124,124] --> [0,6]	[-56,-55] --> [0,2]	[-14,-13] --> [0,4][-1573,-1572] --> [-11,5]	[-641,-640] --> [-13,7]	[913,914] --> [16358,16390]	[124,124] --> [0,6]	[-56,-55] --> [0,2]	[-14,-13] -    device address




  bool DIP[4];


bool blinkState = false;

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
