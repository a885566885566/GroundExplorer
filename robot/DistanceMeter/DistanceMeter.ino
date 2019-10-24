#include <Servo.h>
Servo myservo;
const int servoPin = 4;
const int trigPin = 23;
const int echoPin = 22;
// defines variables
long duration;
int distance;
void setup() {
    pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin, INPUT); // Sets the echoPin as an Input
    myservo.attach(servoPin);
    Serial.begin(115200); // Starts the serial communication
}
int offset = 1;
int pos = 1;
void loop() {
    while(pos>0 && pos <170) { // goes from 0 degrees to 180 degrees
      // in steps of 1 degree
      myservo.write(pos);              // tell servo to go to position in variable 'pos'
      delay(15);                       // waits 15ms for the servo to reach the position
      Serial.print(pos);
      Serial.print(",");
      Serial.println(get_distance());
      pos += offset;
    }
    offset *= -1;
    pos += offset;
}
double get_distance(){
    // Clears the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    // Calculating the distance
    return duration*0.034/2;
}
