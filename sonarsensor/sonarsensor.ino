/*
 * Posted on https://randomnerdtutorials.com
 * created by http://playground.arduino.cc/Code/NewPing
*/

#include <NewPing.h> //library for sonar sensor
 
#define TRIGGER_PIN 8 //B8
#define ECHO_PIN 9 //B9
#define MAX_DISTANCE 200 //need to define max distance for sonar sensor to sense

// NewPing setup of pins and maximum distance
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); 
 
unsigned int distance = sonar.ping_cm(); //returns distance cm from sonar sensor

// Use all that ^

void setup() {
   Serial.begin(9600);
}
 
void loop() {
   delay(50);
   unsigned int distance = sonar.ping_cm();
   Serial.print(distance);
   Serial.println("cm");
}
