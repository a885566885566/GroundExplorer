#ifndef _MY_MOTOR
#define _MY_MOTOR

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define GPIO1 8
#define MOTOR1 9
#define GPIO2 12
#define MOTOR2 11
#define ENCODER1 2
#define ENCODER2 3

#define reso 9
extern const double rad;
extern char mode;

// ------pi control------
extern const double KP_1 , KI_1 , KP_2 , KI_2;

// -----vcmd-----
extern double vcmd1,vcmd2;
extern volatile int PWM1 , PWM2;

void EncoderInit();
void EncoderDisable();
int pi_control(int,double,double);
void change_vcmd(char);

#endif
