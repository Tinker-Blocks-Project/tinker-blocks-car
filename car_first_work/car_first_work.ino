#include <Servo.h>

const int motorPins[] = {22, 23, 24, 25, 30, 31, 32, 33};
const int enablePins[] = {4, 6, 3,8};
const int servoPins[] = {2, 11};
const int SPEED = 100;
const int ref_angle1 = 105;
const int ref_angle2 = 90;
Servo servos[2];

void setup() {
    // Set motor control pins as OUTPUT
    for (int i = 0; i < 8; i++) {
        pinMode(motorPins[i], OUTPUT);
    }
    
    // Set enable pins as OUTPUT
    for (int i = 0; i < 4; i++) {
        pinMode(enablePins[i], OUTPUT);
    }

    // Attach servos
    for (int i = 0; i < 2; i++) {
        servos[i].attach(servoPins[i]);
    }

    moveServo(0,ref_angle1);
  moveServo(1,ref_angle2);
    
  

 
}

void moveMotor(int motorIndex, int speed, bool forward) {
    int pin1 = motorPins[motorIndex * 2];
    int pin2 = motorPins[motorIndex * 2 + 1];
    int enPin = enablePins[motorIndex];

    analogWrite(enPin, speed); // Set motor speed

    if (forward) {
        digitalWrite(pin1, HIGH);
        digitalWrite(pin2, LOW);
    } else {
        digitalWrite(pin1, LOW);
        digitalWrite(pin2, HIGH);
    }
}

void stopMotor(int motorIndex) {
    int pin1 = motorPins[motorIndex * 2];
    int pin2 = motorPins[motorIndex * 2 + 1];
    int enPin = enablePins[motorIndex];
    
    digitalWrite(pin1, LOW);
    digitalWrite(pin2, LOW);
    analogWrite(enPin, 0);
}

void moveServo(int servoIndex, int angle) {
    if (servoIndex >= 0 && servoIndex < 2) {
        servos[servoIndex].write(angle);
    }
}

void loop() {
  /* 
   moveMotor(0,SPEED,false);
  moveMotor(1,SPEED,false);
  moveMotor(2,SPEED,false);
  moveMotor(3,SPEED,true);
  delay(1000);
  moveServo(0,ref_angle1 + 15);
  moveServo(1,ref_angle2 + 15);
  delay(1000);
  moveServo(0,ref_angle1);
  moveServo(1,ref_angle2);*/

}
