#ifndef SMOOTHPATH_H
#define SMOOTHPATH_H
#include "constants.h"
//#include <math.h>

/*parameter*/
const int a_max = 50;//最大加速度
const int alpha_max = 50;//最大角加速度
const float sampling_time = 0.025;
const float c_x = 0.1;//landing coefficient
const float d_w = 103;//輪距
const float r_w = 16.5;//輪子半徑

class Car{
    public:
        float x;//x座標
        float y;//y座標
        float theta;//角度
        float v;//速度
        float w;//角速度
        Car(); 
        Car(float tx, float ty, float tt, float tv, float tw);
};

class SmoothPath{
    public:
        /*mediator*/
        float e_x;
        float e_y;
        float e_theta;
        float e_xdot;
        float e_ydot;
        float v_s;
        float a_c;
        float theta_p;
        float w_p;
        float w_s;
        float alpha_c;
        /*cmd*/
        float wr_cmd;//右輪指令
        float wl_cmd;//左輪指令

        Car present;
        Car next;
        Car target;
        /*function*/
        float abs_val(float);
        int sgn_fun(float);
        float clamp(float,float);
        void dead_reckoning(void);
        void relative_path_error(void);
        void velocity_control(void);
        void wheel_velocity(void);
        SmoothPath();
};


#endif
