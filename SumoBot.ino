/*
 ============================================================================
 Name        : Robot.ino
 Author      : Rafi Khan
 Version     : 1.0
 Copyright   : Copyright 2015 Rafi Khan
 Description : Arduino sketch for sumo bot
 ============================================================================
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

	int SENSOR_THRESHOLD = 400;


void setup(){
	// Attach Servos
  		leftServo.attach(LEFT_SERVO_PIN, SERVO_MIN, SERVO_MAX);
  		rightServo.attach(RIGHT_SERVO_PIN, SERVO_MIN, SERVO_MAX);
  	
	Serial.begin(9600);	// Initialize Serial Communication

}

void loop(){
	while(checkLineSensor){
		scan();
	}
}
	

// If we are on the edge return false
bool checkLineSensor(){
	int leftLine = analogRead(LEFT_SENSOR_PIN);
	int rightLine = analogRead(RIGHT_SENSOR_PIN);
	int backLine = analogRead(BACK_SENSOR_PIN);

	if(backLine <= SENSOR_THRESHOLD){
		simpleMove(2000, 2000, 300);
		return false;
	}
	else if(leftLine <= SENSOR_THRESHOLD && rightLine <= SENSOR_THRESHOLD){
		simpleMove(1000, 1000, 300);
		return false;
	}
	else if(leftLine <= SENSOR_THRESHOLD){
		simpleMove(1000, 1300, 300);
		return false;
	}
	else if(rightLine <= SENSOR_THRESHOLD){
		simpleMove(1300, 1000, 300);
		return false;
	}
	else{
		return true;
	}

}

void scan(){
	int left = checkPingSensor(leftSonar);
	int right = checkPingSensor(rightSonar);

	if (right == left)
	simpleMove(2000, 2000, 0);
	else if (right < left)
	simpleMove(1600, 2000, 0);
	else if (left < right)
	simpleMove(2000, 1600, 0);	
}

// Returns true if there is something in front of us within 'limit' cm
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

