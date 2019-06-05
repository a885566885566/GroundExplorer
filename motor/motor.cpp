#include "motor.h"

const double rad = 0.3490658504;//一格
volatile double speed1 = 0 , speed2 = 0;
double vcmd1 = 0 , vcmd2 = 0;

const double KP_1 = 20 , KI_1 = 30 , KD_1 = 0;
const double KP_2 = 22 , KI_2 = 30 , KD_2 = 0 ;
volatile int PWM1 = 0 , PWM2 = 0;

int pi_control(int num , double vcmd , double sp){
    static int PWM = 0;
    static double G_1 , G_2 , in_1 , in_2 , d_1 , d_2;
    static double ERR = 0 , G = 0 , in = 0 , PRE_ERR_1 = 0 , PRE_ERR_2 = 0 , D = 0;
    static double T_1 , T_2;

    ERR = vcmd - sp;
    if(num == 1){//motor1
        G_1 = ERR * KP_1;
        in_1 = in_1 + ERR * 0.1 * (KI_1 - T_1);
        d_1 = (ERR - PRE_ERR_1) / 0.1 * KD_1;
        PRE_ERR_1 = ERR;
        G = G_1; in = in_1; D = d_1;
    }
    else{//motor2
        G_2 = ERR * KP_2;
        in_2 = in_2 + ERR * 0.1 * (KI_2 - T_2);
        d_2 = (ERR - PRE_ERR_2) / 0.1 * KD_2;
        PRE_ERR_2 = ERR;
        G = G_2; in = in_2; D = d_2;
    }
    if(G > 255)
        G = 255;
    else if (G < -255)
        G = -255;

    if(in > 255)
        in = 255;
    else if(in < -255)
        in = -255;

    if(D > 255)
        D = 255;
    else if(D < -255)
        D = -255;

    PWM = (G + in + D) * 10;
    if(PWM < MAX){
        if(num == 1)
            T_1 = 0;
        else
            T_2 = 0;

        if(PWM < 0)
            PWM = 0;
    }
    else if(PWM > MAX){
        if(num == 1)
            T_1 = (PWM - MAX) * 0.5;
        else
            T_2 = (PWM - MAX) * 0.5;
        PWM = MAX;
    }
    return PWM;
}