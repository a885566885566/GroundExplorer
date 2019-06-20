#include"smoothpath.c"
void loop()
{
	//step 1 : encoder read left and right wheel's speed
	float wr;
	float wl;
	present.w=r_w/d_w*(wr-wl);
	present.v=r_w/2*(wr+wl);
	//	present.x=0;
	//	present.y=0;
	//	present.theta=0;

	//step 2 : tracking control algorithm
	dead_reckoning();
	relative_path_error();
	velocity_control();
	wheel_velocity();

	//step 3 : update car's data 
	present=next;

	//step 4 : pi cotrollor speed control
	//put wr_cmd and wl_cmd into pi controllor
	





}
