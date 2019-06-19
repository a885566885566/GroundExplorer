#ifndef CORE_H
#define CORE_H

#include <Arduino.h>

#define ssid "e94066157-X550VQ"
#define password "bpbfXOHT"

#define port 10418
#define host "10.42.0.1"


#define SONAR_TRIG_PIN 2 //D4
#define SONAR_ECHO_PIN 0 //D3

#define ERROR( msg ) Serial.println(msg);
#define LOG( msg ) Serial.println(msg);

typedef struct{
    double x;
    double y;
}Point2f;


#endif
