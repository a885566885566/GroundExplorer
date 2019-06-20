#ifndef SMOOTHPATH_H
#define SMOOTHPATH_H
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
typedef struct car
{
	float x;//x座標
	float y;//y座標
	float theta;//角度
	float v;//速度
	float w;//角速度
}car;
/*car state*/
car present;
car next;
car target;

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

/*parameter*/
const int a_max;//最大加速度
const int alpha_max;//最大角加速度
const float sampling_time;
const float c_x;//landing coefficient
const float d_w;//輪距
const float r_w;//輪子半徑

/*cmd*/
float wr_cmd;//右輪指令
float wl_cmd;//左輪指令

/*function*/
float abs_val(float);
int sgn_fun(float);
float clamp(float,float);
void dead_reckoning(void);
void relative_path_error(void);
void velocity_control(void);
void wheel_velocity(void);
#endif
