#define EnDirection 0 //This setting is dumb and buggy, dont use it. 
#define EnableAxisStateCheck 0 //this setting is buggy and causes boot problems, dont use it. 


#define OdriveInitialize 0 // turn this on to help with the initial setup on Odrive
#define EnFreeFallDetect 1 // detect when car is in free fall, and stop PID windup
bool EnableOuterLoop = 0; // Turn this on th enable remote control
#define RemoteCommCheck 1 // disable outer loop when remote is not turned on, this will stop the bootup deathspin.
#define GoFaster 0 // allow machine to lean further
#define Command_Angle_Smooth 1 // Make the command angle change in more controlled of a fashion
#define MAX_CURRENT 70.0f // Max Odrive Current
double AMP = 1; // Speed amplification setting, turn this up to go faster.
bool Bat_Low = 0;
float MinBatt = 19; // minimum battery voltage before AMP gets set to 


#include <PID_v1.h>
#include <ODriveArduino.h>



#define CH1Pin 23
#define CH2Pin 22
#define CH3Pin 21



//(The third PID Loop is for a dumb setting that didnt work right, ignore it)
double p[] = {492, 0.01,600} ; // This is PID settings for the three PID Loops {wheeldrivePID, Command Angle PID, Steering PID}
double i[] = {300, .03782,286} ;
double d[] = {41,0,0};






bool RemoteCommOK = !RemoteCommCheck;

#define MPU6050_DMP_FIFO_RATE_DIVISOR 0x00



template<class T> inline Print& operator <<(Print &obj,     T arg) { obj.print(arg);    return obj; }
template<>        inline Print& operator <<(Print &obj, float arg) { obj.print(arg, 4); return obj; }

ODriveArduino odrive(Serial2);



double Current_Angle,Command_Angle,Command_Angle_Out,Command_Speed = 0,WD_PID_Out_Vector, Speed_Error,Angle_Error,Zero = 0,Current_Speed, CA_PID_I , CA_PID_SP,CA_Decay = .85;
double SpeedDeadBand = 100, CurRotationAngle = 0, ReqRotationAngle = 0, VbusVoltage = 0,GripFactor = 1, SteerRotationAngle = 0,RotationAngleError; // was .3






const int ledPin = 13;
int AxisState[2];
int MotorCurrent[2];
int MotorVelocity[2];
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

MPU6050 mpu;

#define OUTPUT_READABLE_YAWPITCHROLL
#define INTERRUPT_PIN 17  // use pin 2 on Arduino Uno & most boards
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
