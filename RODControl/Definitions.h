// Serial connection type (USB || LAN)
#define USB 0
#define LAN 1
#define WLAN 2
#define CONNECTION LAN

#if CONNECTION == USB
  #define mySerial (Serial)
#else
  #define mySerial (Serial1)
#endif

// Arduino pin connections
#define LED 13
#define ENDSTOP 2
#define RIGHTMOTORA 3
#define RIGHTMOTORB 5
#define LEFTMOTORA 6
#define LEFTMOTORB 11
#define CAMERAX A0
#define CAMERAY A1
#define PULLEYSERVO A2
#define TURNSERVO A3
#define GRIPPERSERVO A4

// Servo angles
#define TURNSERVOMIN 1
#define TURNSERVOMID 86
#define TURNSERVOMAX 173
#define TURNSERVOSTEP 2

#define GRIPPERSERVOMIN 21
#define GRIPPERSERVOMAX 60
#define GRIPPERSERVOSTEP 1

#define PULLEYSERVOUP 0 //??
#define PULLEYSERVOIDLE 88
#define PULLEYSERVODOWN 180  //??

#define CAMERAXMIN 10
#define CAMERAXMAX 125
#define CAMERAXGRIPPER 110
#define CAMERAXFORWARD 80
#define CAMERAYMIN 40
#define CAMERAYMAX 140
#define CAMERAYGRIPPER 55
#define CAMERAYFORWARD 80
#define CAMERASTEP 2

// Function prototypes
void waitAndBlink(unsigned long deltaMilliSec);
void endstopPressed();
void endstopNotPressed();
