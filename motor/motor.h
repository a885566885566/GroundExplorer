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
#define MOTOR2 5
#define ENCODER1 2
#define ENCODER2 4

#define MAX 200 //積分飽和
#define MAX_VELOCITY 0.6
#define WHEEL_WIDTH 0.48

extern const double rad;//一格
extern volatile double speed1, speed2;
extern double vcmd1, vcmd2;

extern const double KP_1, KI_1;
extern const double KP_2, KI_2;
extern volatile int PWM1, PWM2;

/*-----Interrupt-----*/
//void EncoderInit();
//void EncoderDisable();
//ISR(TIMER2_COMPA_vect);

int pi_control(int, double, double);

#endif