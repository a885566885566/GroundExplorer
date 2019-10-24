#include "PID_Controller.h"
#include <Arduino.h>
int PID_Controller::limit_output(double input){
    return (int)(int(input)>OUTPUT_MAX) ? OUTPUT_MAX : ((int(input)<OUTPUT_MIN) ? OUTPUT_MIN : int(input));
}

PID_Controller::PID_Controller(double p, double i, double d){
    KP = p;
    KI = i;
    KD = d;
    ERR = 0;
}

int PID_Controller::update(double vcmd, double current){
    //static double G = 0, In = 0, D = 0, T = 0, preERR = ERR;
    //static int pwm = 0;
    ERR = vcmd - current;
    G = ERR * KP;
    In = In + ERR * INTERRUPT_INTERVAL * (KI - T);
    D = KD * (ERR - preERR) / INTERRUPT_INTERVAL;
    preERR = ERR;
    pwm=G;
    pwm = limit_output(G) + limit_output(In) + limit_output(D);
    pwm = limit_output(pwm);
    /*
    if( abs(pwm) < INTERGRAL_SAT ) T = 0;
    else{
        T = ( pwm - INTERGRAL_SAT) * INTERGRAL_SAT_RATIO;
        pwm = INTERGRAL_SAT;
    }*/
    //if( pwm < 0 ) pwm = 0;
    return pwm;
}
