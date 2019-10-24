#ifndef _PID_CONTROLLER_H
#define _PID_CONTROLLER_H

#define MAX_VELOCITY 60
#define WHEEL_WIDTH 0.48


#define INTERRUPT_INTERVAL 0.0025
#define OUTPUT_MAX 250
#define OUTPUT_MIN -250
#define INTERGRAL_SAT 200
#define INTERGRAL_SAT_RATIO 0.5

class PID_Controller{
    public: 
        double KP;
        double KI;
        double KD;
        double ERR;
        PID_Controller(double p, double i, double d);
        static int limit_output(double input);
        int update(double vcmd, double current);
    private:
        double G = 0, In = 0, D = 0, T = 0, preERR = ERR;
        int pwm = 0;
};

#endif
