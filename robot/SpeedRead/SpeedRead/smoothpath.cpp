#include"smoothpath.h"
#include <Arduino.h>
Car::Car(){
    x = y = theta = v = w = 0;
}        
Car::Car(float tx, float ty, float tt, float tv, float tw){
    x = tx; y = ty; theta = tt; v = tv; w = tw;
}

SmoothPath::SmoothPath(){
  e_x = e_y = e_theta = e_xdot = e_ydot = v_s = a_c
   = theta_p = w_p = w_s = alpha_c = wr_cmd = wl_cmd = 0;//左輪指令
}
float SmoothPath::abs_val(float x)
{
    if(x<0)
    {
        return -x;
    }
    else return x;
}

int SmoothPath::sgn_fun(float x)
{
    if(x>0)
    {
        return 1;
    }
    else if(x==0)
    {
        return 0;
    }
    else return -1;
}

float SmoothPath::clamp(float x,float y)
{
    if(abs_val(x)<y)
    {
        return x;
    }
    else if(abs_val(x)>=y)
    {
        return y;
    }
}
void SmoothPath::dead_reckoning()
{
    if(abs(present.w) > 0.0001)
    {
        next.theta=present.theta+present.w*sampling_time;
        next.x=present.x+present.v*(sin(next.theta)-sin(present.theta))/present.w;
        next.y=present.y-present.v*(cos(next.theta)-cos(present.theta))/present.w;
    }
    else
    {
        next.theta=present.theta;
        next.x=present.x+present.v*sampling_time*cos(present.theta);
        next.y=present.y+present.v*sampling_time*sin(present.theta);
    }
}

void SmoothPath::relative_path_error()
{
    e_x=(target.x-present.x)*cos(target.theta)+(target.y-present.y)*sin(target.theta);
    e_y=(target.x-present.x)*sin(target.theta)+(target.y-present.y)*cos(target.theta);
    e_theta=target.theta-present.theta;
}

void SmoothPath::velocity_control()
{
    e_xdot=target.v-present.v*cos(e_theta)+target.w*e_y;
    v_s=e_xdot+sqrt(2*a_max*abs_val(e_x))*sgn_fun(e_x);
    a_c=clamp(v_s/sampling_time,a_max);
    next.v=present.v+a_c*sampling_time;
}

void SmoothPath::wheel_velocity()
{
    theta_p=target.theta+atan(3*c_x*pow(e_y/c_x,2/3)*sgn_fun(e_y));
    e_ydot=present.v*sin(e_theta)-target.w*e_x;
    w_p=target.w+(2*pow(e_y/c_x,-1/3)/(1+tan(theta_p-target.theta)*tan(theta_p-target.theta)))*e_ydot*sgn_fun(e_y);
    w_s=w_p+sqrt(2*alpha_max*abs_val(theta_p-present.theta))*sgn_fun(theta_p-present.theta);
    alpha_c=clamp(w_s/sampling_time,alpha_max);
    next.w=present.w+alpha_c*sampling_time;
    wr_cmd=next.v/r_w+0.5*d_w*next.w/r_w;
    wl_cmd=next.v/r_w-0.5*d_w*next.w/r_w;
}
