#include"smoothpath.h"
float abs_val(float x)
{
	if(x<0)
	{
		return -x;
	}
	else return x;
}

int sgn_fun(float x)
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

float clamp(float x,float y)
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
void dead_reckoning()
{
	if(present.w_c!=0)
	{
		next.theta=present.theta+present.w*sampling_time;
		next.x=present.x+present.v/present.w*(sin(next.theta)-sin(present.theta));
		next.y=present.y-present.v/present.w*(cos(next.theta)-cos(present.theta));
	}
	else
	{
		next.theta=present.theta+present.w*sampling_time;
		next.x=present.x+present.v*sampling_time*cos(present.theta);
		next.y=present.y+present.v*sampling_time*sin(present.theta);
	}
}

void relative_path_error()
{
	e_x=(target.x-present.x)*cos(target.theta)+(target.y-present.y)*sin(target.theta);
	e_y=(targer.x-present.x)*sin(target.theta)+(target.y-present.y)*cos(target.theta);
	e_theta=target.theta-present.theta;
}

void velocity_control()
{
	e_xdot=target.v-present.v*cos(e_theta)+target.w*e_y;
	v_s=e_xdot+sqrt(2*a_max*abs_val(e_x))*sgn_fun(e_x);
	a_c=clamp(v_s/sampling_time,a_max);
	next.v=present.v+a_c*sampling_time;
}

void wheel_velocity()
{
	theta_p=target.theta+atan(3*c_x*pow(e_y/c_x,2/3)*sgn_fun(e_y));
	e_ydot=present.v*sin(e_theta)-target.w*e_x;
	w_p=target_w+(2*pow(e_y/c_x,-1/3)/(1+tan(theta_p-target.theta)*tan(theta_p-target.theta))*e_ydot*sgn_fun(e_y);
	w_s=w_p+sqrt(2*alpha_max*abs_val(theta_p-present.theta))*sgn_fun(theta_p-present.theta);
	alpha_c=clamp(w_s/sampling_time,alpha_max);
	next.w=present.w+alpha_c*sampling_time;
	wr_cmd=next.v/r_w+0.5*d_w*next.w/r_w;
	wl_cmd=next.v/r_w-0.5*d_w*next.w/r_w;
}
