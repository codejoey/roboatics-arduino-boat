#include <LSM303.h>
#include <Arduino.h>
#include <Wire.h>
#include <NewPing.h>

//compass pins a4, a5
LSM303 compass;
LSM303::vector<int16_t> running_min = {32767, 32767, 32767}, running_max = { -32768, -32768, -32768};
float initialHeading;

//motors
int motorControlPinLeft = 4;
int motorControlPinRight = 2;
int motorControlPinLeftrev = 5;
int motorControlPinRightrev = 3;
int motorControlPinLeftpwm = 11;
int motorControlPinRightpwm = 10;
int green = 7;
int red = 6;
int maxspeed = 255;
int leftspeed = 0;
int rightspeed = 0;

//sonar
#define TRIGGER_PIN A0
#define ECHO_PIN A1
#define MAX_DISTANCE 2000 //need to define max distance for sonar sensor to sense
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance

//distance
int THRS = 30;
int dist;
int maxv = 350;
int minv = 250;

void setup() {
  blink(6,0);
  pinMode(motorControlPinLeft, OUTPUT);
  pinMode(motorControlPinRight, OUTPUT);
  pinMode(motorControlPinLeftrev, OUTPUT);
  pinMode(motorControlPinRightrev, OUTPUT);
  pinMode(motorControlPinLeftpwm, OUTPUT);
  pinMode(motorControlPinRightpwm, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(red, OUTPUT);
  digitalWrite(motorControlPinLeft, 0);
  digitalWrite(motorControlPinLeftrev, 0);
  analogWrite(motorControlPinLeftpwm, 0);
  digitalWrite(motorControlPinRight, 0);
  digitalWrite(motorControlPinRightrev, 0);
  analogWrite(motorControlPinRightpwm, 0);
  Serial.begin(9600);
  Wire.begin();
  compass.init();
  compass.enableDefault();
  delay(5000);

  compass.read();
  initialHeading = (atan2(compass.m.y, compass.m.x));
  if (initialHeading < 0) {
    initialHeading += 2 * PI;
  }
  //Serial.print("Initial bearing = ");
  //Serial.println(initialHeading);
}

int drivetowall() {
  digitalWrite(motorControlPinLeft, 1);
  digitalWrite(motorControlPinLeftrev, 0);
  digitalWrite(motorControlPinRight, 1);
  digitalWrite(motorControlPinRightrev, 0);
  unsigned int dist = sonar.ping_cm();
  compass.read();
  float heading = (atan2(compass.m.y, compass.m.x));
  // Correct for when signs are reversed.
  if (heading < 0) {
    heading += 2 * PI;
  }
  while (dist == 0 or dist > THRS) {
    compass.read();
    heading = (atan2(compass.m.y, compass.m.x));
    // Correct for when signs are reversed.
    if (heading < 0) {
      heading += 2 * PI;
    }
    float dif = heading - initialHeading;
    if (dif < -PI) {
      dif += 2 * PI;
    }
    else if (dif > PI) {
      dif -= 2 * PI;
    }
    Serial.print("Heading difference: \t");
    Serial.println(dif);
    if (dif < -0.1) {
      // turn right
      leftspeed = (int) (0.4 * maxspeed);
      rightspeed = (int) (max(0.4 + dif,0) * maxspeed);
    }
    else if (dif > 0.1) {
      //turn left
      leftspeed = (int) (max(0.4 - dif,0)* maxspeed);
      rightspeed = (int) (0.4 * maxspeed);
    }
    else {
      leftspeed = (int) (0.4 * maxspeed);
      rightspeed = (int) (0.4 * maxspeed);
    }
    analogWrite(motorControlPinLeftpwm, leftspeed);
    Serial.print("Left motor = ");
    Serial.println(leftspeed);
    analogWrite(motorControlPinRightpwm, rightspeed);
    Serial.print("Right motor = ");
    Serial.println(rightspeed);
    delay(100);
    dist = sonar.ping_cm();
    Serial.print(dist);
    Serial.println("cm");
  }
  analogWrite(motorControlPinLeftpwm, 0);
  analogWrite(motorControlPinRightpwm, 0);
  delay(100);
  
  //stop moving
  leftspeed = (int) (0.4 * maxspeed);
  rightspeed = (int) (0.4 * maxspeed);
  digitalWrite(motorControlPinLeft, 0);
  digitalWrite(motorControlPinLeftrev, 1);
  analogWrite(motorControlPinLeftpwm, leftspeed);
  delay(50);
  digitalWrite(motorControlPinRight, 0);
  digitalWrite(motorControlPinRightrev, 1);
  analogWrite(motorControlPinRightpwm, rightspeed);
  delay(100);
}

void turn() {
  compass.read();
  float heading = (atan2(compass.m.y, compass.m.x));
  // Correct for when signs are reversed.
  if (heading < 0) {
    heading += 2 * PI;
  }
  float dif = heading - initialHeading;
  if (dif < -PI) {
    dif += 2 * PI;
  }
  else if (dif > PI) {
    dif -= 2 * PI;
  }
  leftspeed = (int) (0.2 * maxspeed);
  rightspeed = (int) (0.2 * maxspeed);
  //reverse left motor direction
  digitalWrite(motorControlPinLeft, 0);
  digitalWrite(motorControlPinLeftrev, 1);
  analogWrite(motorControlPinLeftpwm, leftspeed);
  digitalWrite(motorControlPinRight, 1);
  digitalWrite(motorControlPinRightrev, 0);
  analogWrite(motorControlPinRightpwm, rightspeed);
  while (dif > -2.5) {//3.1) {
    compass.read();
    heading = (atan2(compass.m.y, compass.m.x));
    // Correct for when signs are reversed.
    if (heading < 0) {
      heading += 2 * PI;
    }
    dif = heading - initialHeading;
    if (dif < -PI) {
      dif += 2 * PI;
    }
    else if (dif > PI) {
      dif -= 2 * PI;
    }
    Serial.print("Turning at angle: ");
    Serial.println(dif);
    delay(50);
  }
  
  //stop spinning
  digitalWrite(motorControlPinLeft, 1);
  digitalWrite(motorControlPinLeftrev, 0);
  analogWrite(motorControlPinLeftpwm, leftspeed);
  digitalWrite(motorControlPinRight, 0);
  digitalWrite(motorControlPinRightrev, 1);
  analogWrite(motorControlPinRightpwm, rightspeed);
  delay(50);
  
  //switch left motor direction
  analogWrite(motorControlPinLeftpwm, 0);
  analogWrite(motorControlPinRightpwm, 0);
  initialHeading += PI;
  if (initialHeading > 2 * PI) {
    initialHeading -= 2 * PI;
  }
}

void mystop() {
  digitalWrite(motorControlPinLeft, 0);
  digitalWrite(motorControlPinLeftrev, 0);
  analogWrite(motorControlPinLeftpwm, 0);
  digitalWrite(motorControlPinRight, 0);
  digitalWrite(motorControlPinRightrev, 0);
  analogWrite(motorControlPinRightpwm, 0);
}

void blink(int number, int endstate)
{
  for (int i = 0; i < number; i++)
  {
    digitalWrite(green, !digitalRead(green));
    digitalWrite(red, !digitalRead(red));
    delay(100);
  }
  digitalWrite(green, endstate);
  digitalWrite(red, endstate);
}


void loop() {
  drivetowall();
  mystop();
  turn();
  drivetowall();
  mystop();
  Serial.println("done");
  while (1) {}
}
