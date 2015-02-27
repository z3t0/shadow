/*
================================================================================================
Name        : SumoBot.ino
Author      : Rafi Khan
Version     : 1.1
Copyright   : Copyright 2015 Rafi Khan
Description : Arduino sketch for mini sumo bot competition, check README for more info.
=================================================================================================
*/



// Includes
#include <Servo.h>
#include <NewPing.h>

// Servos
byte LEFT_SERVO_PIN = 9;
byte RIGHT_SERVO_PIN = 10;
int SERVO_MIN = 1000;
int SERVO_MAX = 2000;

Servo leftServo;
Servo rightServo;

// Sensors
// Ping Sensor
byte RIGHT_PING_PIN = 12;
byte LEFT_PING_PIN = 11;
byte MAX_DISTANCE = 77;

NewPing rightSonar(RIGHT_PING_PIN, MAX_DISTANCE);
NewPing leftSonar(LEFT_PING_PIN, MAX_DISTANCE);

// Line Sensors
byte LEFT_SENSOR_PIN = A0;	// NEED TO REDEFINE
byte RIGHT_SENSOR_PIN = A1;
byte BACK_SENSOR_PIN = A2;

int SENSOR_THRESHOLD = 400; // NEED TO REDEFINE


void setup(){
        // Attach Servos
        leftServo.attach(LEFT_SERVO_PIN, SERVO_MIN, SERVO_MAX);
        rightServo.attach(RIGHT_SERVO_PIN, SERVO_MIN, SERVO_MAX);
        Serial.begin(9600);	// Initialize Serial Communication
}

void loop(){
        while(checkLineSensor)
                scan();
}

// If we are on the edge return false
bool checkLineSensor(){

        bool leftLine = false;
        bool rightLine = false;
        bool backLine = false;

        // True if on black
        // Get values from line sensors
        if(analogRead(LEFT_SENSOR_PIN) <= SENSOR_THRESHOLD)
                leftLine = false;
        if(analogRead(RIGHT_SENSOR_PIN) <= SENSOR_THRESHOLD)
                rightLine = false;
        if(analogRead(BACK_SENSOR_PIN) <= SENSOR_THRESHOLD)
                backLine = false;

        if(backLine){   // Falling STRAIGHT BACKWARDS
                if(!leftLine && !rightLine)
                        simpleMove(2000, 2000, 300);

                if(rightLine && !leftLine)
                        simpleMove(1800,2000, 300);

                if(leftLine && !rightLine)
                        simpleMove(2000, 1800, 300);

                return false;
        }

        else if(leftLine){   
            if(!rightline)  // Falling LEFT ONLY
                    simpleMove(1200, 1400, 150);
            if(rightLine)   // Falling STRAIGHT FO
                    simpleMove(1000, 1000, 300);
        }

        else if(rightLine){
                simpleMove(1400, 1200, 150);
        }
}

void scan(){
        int left = checkPingSensor(leftSonar);
        int right = checkPingSensor(rightSonar);
        bool neg = false;

        int num = left - right; // -3
        if(num < 0){
                neg = true;
                num = num * -1;
        }

        float val = atan2(num, 2) * 57.30; // Verify accuracy 

        if(neg){
                int mval = (val/90) * 500 + 1500 ; // Left
                int amval = (90 - val/90) * 500 + 1500; // Right
                simpleMove(mval, amval, 0);
        }	
        else{
                int mval = (val/90)	* 500 + 1500;
                int amval = (90 - val/90) * 500 + 1500;
                simpleMove(amval, mval, 0);
        }
}

// Returns distance within defined range
int checkPingSensor(NewPing sonar){
        delay(50);
        unsigned int uS = sonar.ping() / US_ROUNDTRIP_CM;
        return uS;
}

/*
   For direction:
   -3 = backward
   -2 = right backward
   -1 = left backward	
   0 = nothing
   1 = left forward
   2 = right forward
   3 = forward
 */

void simpleMove(int leftSpeed, int rightSpeed, int dtime){
        leftServo.writeMicroseconds(leftSpeed);
        rightServo.writeMicroseconds(rightSpeed);
        delay(dtime);
}
