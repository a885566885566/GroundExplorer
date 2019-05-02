#ifndef CORE_H
#define CORE_H

#include <Arduino.h>

#define SONAR_TRIG_PIN 2 //D4
#define SONAR_ECHO_PIN 0 //D3

#define ERROR( msg ) Serial.println(msg);

typedef struct{
    double x;
    double y;
}Point2f;


#endif
