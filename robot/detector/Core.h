#ifndef CORE_H
#define CORE_H

#include <Arduino.h>

#define SONAR_TRIG_PIN D5
#define SONAR_ECHO_PIN D6

#define ERROR( msg ) Serial.println(msg);

typedef struct{
    double x;
    double y;
}Point2f;


#endif
