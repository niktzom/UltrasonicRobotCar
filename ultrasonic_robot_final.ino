#include <Servo.h>

// create servo objects
Servo leftMotor;
Servo rightMotor;

const int serialPeriod = 250;       // only print to the serial console every 1/4 second
unsigned long timeSerialDelay = 0;

const int loopPeriod = 30;          // a period of 20ms = a frequency of 50Hz
unsigned long timeLoopDelay   = 0;

const int SPEED = 20;
const int leftMotorCenter = 94;
const int rightMotorCenter = 92;

int rndNumber = 0;

// specify the trig & echo pins used for the ultrasonic sensors
const int ultrasonic2TrigPin = 4;
const int ultrasonic2EchoPin = 5;

int ultrasonic2Distance;
int ultrasonic2Duration;

// define the states
#define DRIVE_FORWARD     0
#define TURN_LEFT         1
#define TURN_RIGHT        2

int state = DRIVE_FORWARD; // 0 = drive forward (DEFAULT), 1 = turn left

void setup()
{
    Serial.begin(9600);
  
    // ultrasonic sensor pin configurations
    pinMode(ultrasonic2TrigPin, OUTPUT);
    pinMode(ultrasonic2EchoPin, INPUT);
    
    leftMotor.attach(7);
    rightMotor.attach(8);

  // if analog input pin 0 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(0));
}

void loop()
{
    if(digitalRead(2) == HIGH) // if the push button switch was pressed
    {
        while(1)
        {
            stopmotors();
        }

    }
    //debugOutput(); // prints debugging messages to the serial console
    
    if(millis() - timeLoopDelay >= loopPeriod)
    {
        readUltrasonicSensors(); // read and store the measured distances
        
        stateMachine();
        
        timeLoopDelay = millis();
    }
}


void stateMachine()
{
    if(state == DRIVE_FORWARD) // no obstacles detected
    {
        if(ultrasonic2Distance > 15 || ultrasonic2Distance < 0) // if there's nothing in front of us (note: ultrasonicDistance will be negative for some ultrasonics if there's nothing in range)
        {
            driveForward();
        }
        else // there's an object in front of us
        {
            rndNumber = random(10, 20);
            if (rndNumber < 15) {
              state = TURN_LEFT;
            } 
            else {
              state = TURN_RIGHT;
            }
            //state = TURN_LEFT;
            //state = TURN_RIGHT;
        }
    }
    else if(state == TURN_LEFT) // obstacle detected -- turn left
    {
      driveBackwards(750);
        turnLeft(250);
        
        state = DRIVE_FORWARD;
    }

    else if(state == TURN_RIGHT) // obstacle detected -- turn right
    {

        //stopMotorsFor(300);

        driveBackwards(750);
    
      
        turnRight(250);
        
        state = DRIVE_FORWARD;
    }
}

void driveForward(){
   rightMotor.write(rightMotorCenter - SPEED); //30
   leftMotor.write(leftMotorCenter + SPEED); //150
}

void driveBackwards(int time){
  unsigned long timeToBackUp   = time; // back up for 2 seconds
  
  unsigned long backUpStartTime = millis(); // save the time that we started backing up

  while((millis()-backUpStartTime) < timeToBackUp) // stay in this loop until timeToBackUp has elapsed
  {
    // back up (you may have to switch these values depending on how you oriented the motors)
    rightMotor.write(rightMotorCenter + SPEED/2); //150
    leftMotor.write(leftMotorCenter - SPEED/2); //30
  }
}

void turnLeft(int time){
   unsigned long timeToTurnLeft = time; // it takes around 1.1 seconds to turn 90 degrees
        
        unsigned long turnStartTime = millis(); // save the time that we started turning

        while((millis()-turnStartTime) < timeToTurnLeft) // stay in this loop until timeToTurnLeft (1.1 seconds) has elapsed
        {
            // turn left
            rightMotor.write(rightMotorCenter - SPEED); //30
            leftMotor.write(leftMotorCenter - SPEED); //30
        }
}

void turnRight(int time){
  unsigned long timeToTurnRight = time; // it takes around 1.1 seconds to turn 90 degrees
        
        unsigned long turnStartTime = millis(); // save the time that we started turning

        while((millis()-turnStartTime) < timeToTurnRight) // stay in this loop until timeToTurnLeft (1.1 seconds) has elapsed
        {
            // turn right
            rightMotor.write(rightMotorCenter + SPEED); //150
            leftMotor.write(leftMotorCenter + SPEED); //150
        }
}

void stopMotorsFor(int time){
  unsigned long timeToStop  = time; 
  
  unsigned long StopStartTime = millis(); // save the time that we started backing up

  while((millis()-StopStartTime) < timeToStop) // stay in this loop until timeToBackUp has elapsed
  {
    stopmotors();
  }
}

void stopmotors(){
  leftMotor.write(leftMotorCenter);
  rightMotor.write(rightMotorCenter);
}


void readUltrasonicSensors()
{
    // ultrasonic 2
    digitalWrite(ultrasonic2TrigPin, HIGH);
    delayMicroseconds(10);                  // must keep the trig pin high for at least 10us
    digitalWrite(ultrasonic2TrigPin, LOW);
    
    ultrasonic2Duration = pulseIn(ultrasonic2EchoPin, HIGH);
    ultrasonic2Distance = (ultrasonic2Duration/2)/29;
}


void debugOutput()
{
    if((millis() - timeSerialDelay) > serialPeriod)
    {
        Serial.print("ultrasonic2Distance: ");
        Serial.print(ultrasonic2Distance);
        Serial.print("cm");
        Serial.println();
        
        timeSerialDelay = millis();
    }
}
