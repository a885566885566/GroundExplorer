#include "ServerConnect.h"


void setup() {
    Serial.begin(115200);
    wifi_init();
    pinMode(SONAR_TRIG_PIN, OUTPUT);
    pinMode(SONAR_ECHO_PIN, INPUT);
}
void loop() {
    digitalWrite(SONAR_TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(SONAR_TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(SONAR_TRIG_PIN, LOW);

    long duration = pulseIn(SONAR_ECHO_PIN, HIGH);
    //long duration = millis();
    //int distance = duration;
    int distance = (double)duration*0.34/2;
    Serial.println(distance);
    send_request(distance);
}
