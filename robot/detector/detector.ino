#include "ServerConnect.h"

#define ERROR( msg ) Serial.println(msg);


void setup() {
    Serial.begin(115200);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    wifi_init();
}
void loop() {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH);
    int distance = duration*0.34/2;
    Serial.println(distance);
    send_request(distance);
    send_request(distance);
}
