const int triggerpin = 8;
const int echopin = 9;
long duration = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(triggerpin, OUTPUT); //set as output
  pinMode(echopin, INPUT); //set as input
  Serial.begin(9600); //set baud rate of serial output
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(triggerpin, LOW);                   
  delayMicroseconds(2);
  digitalWrite(triggerpin, HIGH);          // Trigger pin to HIGH
  delayMicroseconds(10);                   // 10us high 
  digitalWrite(triggerpin, LOW);           // Trigger pin to HIGH
 
  duration = pulseIn(echopin,HIGH);        // Waits for the echo pin to get high
                                           // returns the Duration in microseconds
  long Distance_cm = Distance(duration);   // Use function to calculate the distance
 
  Serial.print("Distance = ");             // Output to serial
  Serial.print(Distance_cm);
  Serial.println(" cm");
 
  delay(1000);                             // Wait to do next measurement
} 
long Distance(long time)
{
  long DistanceCalc;                      // Calculation variable
   
  DistanceCalc = ((time * 0.034) / 2);     // Actual calculation in cm
    //DistanceCalc = time / 74 / 2;         // Actual calculation in inches
  return DistanceCalc;                    // return calculated value
}
