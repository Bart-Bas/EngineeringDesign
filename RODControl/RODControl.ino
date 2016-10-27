#include "Definitions.h"
#include <Servo.h>

// Global variables
Servo servoCameraX;
Servo servoCameraY;
Servo servoPulley;
Servo servoTurn;
Servo servoGripper;

int cameraX = CAMERAXFORWARD;
int cameraY = CAMERAYFORWARD;
int turnServo = TURNSERVOMID;
int gripperServo = GRIPPERSERVOMIN;
int pulleyServoIdle = PULLEYSERVOIDLE;
int pulleyServo = pulleyServoIdle;

int pulleyState = PULLEYIDLE;
unsigned long lastTimePulleyChanged;

void setup()
{
  // Set all the PWM pins as output
  pinMode(RIGHTMOTORA, OUTPUT);
  pinMode(RIGHTMOTORB, OUTPUT);
  pinMode(LEFTMOTORA, OUTPUT);
  pinMode(LEFTMOTORB, OUTPUT);
  
  // All DC motors to zero
  analogWrite(RIGHTMOTORA,0);
  analogWrite(RIGHTMOTORB,0);
  analogWrite(LEFTMOTORA,0);
  analogWrite(LEFTMOTORB,0);

  // Attach all the servos
  servoCameraX.attach(CAMERAX);
  servoCameraY.attach(CAMERAY);
  servoPulley.attach(PULLEYSERVO);
  servoTurn.attach(TURNSERVO);
  servoGripper.attach(GRIPPERSERVO);

  // Initialize the endstop switch
  pinMode(ENDSTOP, INPUT);
  digitalWrite(ENDSTOP, HIGH); // Enable internal pullup resistor
  attachInterrupt(digitalPinToInterrupt(ENDSTOP), endstopChange, CHANGE);

  // Set all the servos in their intitial positions
  servoCameraX.write(cameraX);
  servoCameraY.write(cameraY);
  servoPulley.writeMicroseconds(pulleyServo);
  servoTurn.write(turnServo);
  servoGripper.write(gripperServo);
  
  // Start up the serial connection at the right data rate
  #if CONNECTION == USB
    mySerial.begin(9600);
  #else
    mySerial.begin(115200);
  #endif
  
  // Wait, give enough time for the communications processor to wake up
  #if CONNECTION == WLAN
    waitAndBlink(60000);
  #endif
   
  // Clear the input buffer
  while (mySerial.available())
     mySerial.read();
}

void loop() {
  if(mySerial.available() > 0)
  {
    char command = mySerial.read();
    
    switch(command)
    {
      // Driving control
      // Forward
      case '8':
        analogWrite(RIGHTMOTORA,0);
        analogWrite(RIGHTMOTORB,255);
        analogWrite(LEFTMOTORA,0);
        analogWrite(LEFTMOTORB,255);
      break;
      
      // Backward
      case '2':
        analogWrite(RIGHTMOTORA,255);
        analogWrite(RIGHTMOTORB,0);
        analogWrite(LEFTMOTORA,255);
        analogWrite(LEFTMOTORB,0);
      break;

      // Left
      case '4':
        analogWrite(RIGHTMOTORA,0);
        analogWrite(RIGHTMOTORB,255);
        analogWrite(LEFTMOTORA,255);
        analogWrite(LEFTMOTORB,0);
      break;

      // Right
      case '6':
        analogWrite(RIGHTMOTORA,255);
        analogWrite(RIGHTMOTORB,0);
        analogWrite(LEFTMOTORA,0);
        analogWrite(LEFTMOTORB,255);
      break;
      
      // Stop
      case '5':
        analogWrite(RIGHTMOTORA,0);
        analogWrite(RIGHTMOTORB,0);
        analogWrite(LEFTMOTORA,0);
        analogWrite(LEFTMOTORB,0);
      break;

      // Gripper control (open / close)
      // Open
      case 'o':
        gripperServo += GRIPPERSERVOSTEP;
        if(gripperServo > GRIPPERSERVOMAX)
          gripperServo = GRIPPERSERVOMAX;
        servoGripper.write(gripperServo);
      break;

      // Close
      case 'p':
        gripperServo -= GRIPPERSERVOSTEP;
        if(gripperServo < GRIPPERSERVOMIN)
          gripperServo = GRIPPERSERVOMIN;
        servoGripper.write(gripperServo);
      break;

      // Gripper control (turn)
      // Turn parallel to front of the ROD
      case '.':
        turnServo = TURNSERVOMID;
        servoTurn.write(turnServo);
      break;

      // Turn perpendicular to front of the ROD (take the shortest turn)
      case ',':
        if(turnServo >= TURNSERVOMID)
          turnServo = TURNSERVOMAX;
        else
          turnServo = TURNSERVOMIN;
        servoTurn.write(turnServo);
      break;

      // Slightly turn clockwise
      case '>':
        turnServo += TURNSERVOSTEP;
        if(turnServo > TURNSERVOMAX)
          turnServo = TURNSERVOMAX;
        servoTurn.write(turnServo);
      break;

      // Slightly turn anty-clockwise
      case '<':
        turnServo -= TURNSERVOSTEP;
        if(turnServo < TURNSERVOMIN)
          turnServo = TURNSERVOMIN;
        servoTurn.write(turnServo);
      break;

      // Pulley control
      // Pulley down
      case '-':
        pulleyState = PULLEYDOWN;
        lastTimePulleyChanged = millis();
      break;

      // Pulley up
      case '=':
        // Endstop not pressed
        if(digitalRead(ENDSTOP))
        {
          pulleyState = PULLEYUP;
          lastTimePulleyChanged = millis();
        }
      break;
      
      // Tune pulley idle down
      case '(':
        pulleyServoIdle += 1;
        if(pulleyServoIdle > (PULLEYSERVOIDLE + PULLEYSERVOMAXTUNE))
          pulleyServoIdle = (PULLEYSERVOIDLE + PULLEYSERVOMAXTUNE);
      break;

      // Tune pulley idle down
      case ')':
          pulleyServoIdle -= 1;
        if(pulleyServoIdle < (PULLEYSERVOIDLE - PULLEYSERVOMAXTUNE))
          pulleyServoIdle = (PULLEYSERVOIDLE - PULLEYSERVOMAXTUNE);
      break;

      // Camera control
      // Move image up
      case 'w':
        cameraY += CAMERASTEP;
        if(cameraY > CAMERAYMAX)
          cameraY = CAMERAYMAX;
        servoCameraY.write(cameraY);
      break;

      // Move image down
      case 's':
        cameraY -= CAMERASTEP;
        if(cameraY < CAMERAYMIN)
          cameraY = CAMERAYMIN;
        servoCameraY.write(cameraY);
      break;

      // Move image left
      case 'a':
        cameraX += CAMERASTEP;
        if(cameraX > CAMERAXMAX)
          cameraX = CAMERAXMAX;
        servoCameraX.write(cameraX);
      break;

      // Move image right
      case 'd':
        cameraX -= CAMERASTEP;
        if(cameraX < CAMERAXMIN)
          cameraX = CAMERAXMIN;
        servoCameraX.write(cameraX);
      break;

      // Look forward
      case 'f':
        cameraX = CAMERAXFORWARD;
        cameraY = CAMERAYFORWARD;
        servoCameraX.write(cameraX);
        servoCameraY.write(cameraY);
      break;

      // Look at the gripper
      case 'g':
        cameraX = CAMERAXGRIPPER;
        cameraY = CAMERAYGRIPPER;
        servoCameraX.write(cameraX);
        servoCameraY.write(cameraY);
      break;
     }
  }

  if(pulleyState == PULLEYIDLE)
    servoPulley.writeMicroseconds(pulleyServoIdle);
  else if(pulleyState == PULLEYDOWN)
  {
     servoPulley.writeMicroseconds(pulleyServoIdle + PULLEYSERVOTURN);

     if((millis() - lastTimePulleyChanged) > PULLEYSERVOTURNTIME)
      pulleyState = PULLEYIDLE;
  }
  else if(pulleyState == PULLEYUP)
  {
     servoPulley.writeMicroseconds(pulleyServoIdle - PULLEYSERVOTURN);

     if((millis() - lastTimePulleyChanged) > PULLEYSERVOTURNTIME)
      pulleyState = PULLEYIDLE;
  }
}

#if CONNECTION == LAN
  // Function for the LED boot sequence to make sure the wireless connection is stable
  void waitAndBlink(unsigned long deltaMilliSec)
  {
    // Wait deltaMilliSec milliseconds, LED blinks as a sign of life
    // As time passes frequency inreases
    unsigned long deltaT = 0;
    unsigned long tZero = millis(); // Read start time
    
    while(deltaT<deltaMilliSec)
    {
      deltaT = millis() - tZero; // Compute elapsed time
      delay(500 - 400 * deltaT / deltaMilliSec);
      digitalWrite(LED, LOW);
      delay(500 - 400 * deltaT / deltaMilliSec);
      digitalWrite(LED, HIGH);
    } 
  }
#endif

// Function that is called when the endstop gets pressed or released
void endstopChange()
{
  // Endstop pressed
  if(!digitalRead(ENDSTOP))
  {
    pulleyState = PULLEYIDLE;
  }
}

