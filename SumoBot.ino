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
#include <IRremote.h>
#include <IRremoteInt.h>

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
byte MAX_DISTANCE = 30;

NewPing rightSonar(RIGHT_PING_PIN, RIGHT_PING_PIN, MAX_DISTANCE);
NewPing leftSonar(LEFT_PING_PIN, LEFT_PING_PIN, MAX_DISTANCE);

// Line Sensors
byte LEFT_SENSOR_PIN = A0;	// NEED TO REDEFINE
byte RIGHT_SENSOR_PIN = A1;
byte BACK_SENSOR_PIN = A2;

int SENSOR_THRESHOLD = 300; // NEED TO REDEFINE

// IR Receiver
byte IR_RECV_PIN = 3;
IRrecv irrecv(IR_RECV_PIN);

decode_results results;

// Debugging
byte RED_LED_PIN = 6;
byte GRN_LED_PIN = 7;
byte BLU_LED_PIN = 8;

boolean active = false;



void setup() {
  // Attach Servos
  leftServo.attach(LEFT_SERVO_PIN, SERVO_MIN, SERVO_MAX);
  rightServo.attach(RIGHT_SERVO_PIN, SERVO_MIN, SERVO_MAX);
  Serial.begin(115200);	// Initialize Serial Communication
  irrecv.enableIRIn();
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GRN_LED_PIN, OUTPUT);
  pinMode(BLU_LED_PIN, OUTPUT);
}

void loop() {
        results.value = 0;
        if(!active){ 
                digitalWrite(GRN_LED_PIN, LOW);
                digitalWrite(BLU_LED_PIN, HIGH);
                if (irrecv.decode(&results)) {
                        Serial.println(results.value);
                        if (results.value == 16753245)
                                active = true;
 
                        if (results.value == 16718055)
                            simpleMove(1800, 1800);
                        else if (results.value == 16730805)
                                simpleMove(1200, 1200);
                        else if (results.value == 16716015)
                                simpleMove(1200, 1800);
                        else if (results.value == 16734885)
                                simpleMove(1800, 1200);
                        else{
                                simpleMove(1500, 1500);
                        }
                        irrecv.resume(); // Receive the next value
                        
                        delay(100);

                }
                
                else{
                        simpleMove(1500, 1500);
                }

        }

        while(active){
                digitalWrite(GRN_LED_PIN, HIGH);
                digitalWrite(BLU_LED_PIN, LOW);
 
                while (checkLineSensor){
                if (irrecv.decode(&results)) {
                        Serial.println(results.value);
                        if (results.value == 16753245){
                                active = false;
                        }
                        irrecv.resume(); // Receive the next value
                }
                delay(100);
                scan();
                }
        }
}

// If we are on the edge return false
bool checkLineSensor() {

        bool leftLine = false;
        bool rightLine = false;
        bool backLine = false;

        Serial.println(backLine);

        // True if on black
        // Get values from line sensors
        if (analogRead(LEFT_SENSOR_PIN) >= SENSOR_THRESHOLD)
                leftLine = false;
        if (analogRead(RIGHT_SENSOR_PIN) >= SENSOR_THRESHOLD)
                rightLine = false;
        if (analogRead(BACK_SENSOR_PIN) >= SENSOR_THRESHOLD)
                backLine = false;

        if (backLine) { // Falling STRAIGHT BACKWARDS
                simpleMove(2000, 2000);
                return false; 
        }

        else if (leftLine) {
                if (!rightLine) // Falling LEFT ONLY
                        simpleMove(1200, 1400);
                if (rightLine)  // Falling STRAIGHT FO
                        simpleMove(1000, 1000);
                return false;
        }

        else if (rightLine) {
                simpleMove(1400, 1200);
                return false;
        }
        else{
                return true;
        }
}

void scan() {
        int left = checkPingSensor(leftSonar);
        int right = checkPingSensor(rightSonar);
        if(right == 0 && left == 0){
                simpleMove(1500, 1500);
        }

        int value = left - right;
        if (value < 0)
                value = value * -1;

        if (value <= 5){
                simpleMove(1800, 1800);
        }

        else if (left == 0 && right > 0){
                simpleMove(1800, 1200);
        }

        else if(right == 0 && left > 0){
                simpleMove(1200, 1800);  
        }

        else{
                simpleMove(1500, 1500);
        }


}

// Returns distance within defined range
int checkPingSensor(NewPing sonar) {
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

void simpleMove(int leftSpeed, int rightSpeed) {
        leftServo.writeMicroseconds(leftSpeed);
        rightServo.writeMicroseconds(rightSpeed);
}
