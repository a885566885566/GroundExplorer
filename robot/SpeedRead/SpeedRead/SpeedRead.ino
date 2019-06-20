#include "constants.h"
//#include"smoothpath.h"
#include"PID_Controller.h"
#include "QueueArray/QueueArray.h"
#include "ServerConnection.h"
// pointer of timer
hw_timer_t *encoder_timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;
#define ENABLE_WIFI
#define d_t 0.025          // 25ms

volatile double Car_rotary_offset[2] = {0};
volatile double Car_position[2] = {0};
volatile double Car_theta = 0;

volatile double central_v = 0;
volatile double central_w = 0;

volatile double right_pwm = 0;
volatile double left_pwm = 0;

typedef struct Car{
    volatile double x;//x座標
    volatile double y;//y座標
    volatile double theta;//角度
    volatile double v;//速度
    volatile double w;//角速度
}Car;

inline void rest_car(volatile Car *t, double tx, double ty, double tt, double tv, double tw){
    t->x = tx; t->y = ty; t->theta = tt; t->v = tv; t->w = tw;
}

inline void rest_car(volatile Car *a, Car *b){
    a->x = b->x;
    a->y = b->y;
    a->theta = b->theta;
    a->v = b->v;
    a->w = b->w;
}
inline void set_car(volatile Car *a, volatile Car *b){
    a->x = b->x;
    a->y = b->y;
    a->theta = b->theta;
    a->v = b->v;
    a->w = b->w;
}
volatile double e_x;
volatile double e_y;
volatile double e_theta;
volatile double e_xdot;
volatile double e_ydot;
volatile double v_s;
volatile double a_c;
volatile double theta_p;
volatile double w_p;
volatile double w_s;
volatile double alpha_c;
/*cmd*/
volatile double wr_cmd;//右輪指令
volatile double wl_cmd;//左輪指令

Car present;
Car next;
Car target;

const int a_max = 50;//最大加速度
const int alpha_max = 50;//最大角加速度
const double sampling_time = 0.025;
const double c_x = 0.1;//landing coefficient
const double d_w = 103;//輪距
const double r_w = 16.5;//輪子半徑

inline double abs_val(double x){
    if(x<0) return -x;
    else    return x;
}

inline int sgn_fun(double x){
    if(x>0)       return 1;
    else if(x==0) return 0;
    else          return -1;
}

inline double clamp(double x,double y){
    if(abs_val(x)<y)        return x;
    else if(abs_val(x)>=y)  return y;
}

inline void dead_reckoning()
{
    if(abs(present.w) > 0.0001)
    {
        next.theta=present.theta+present.w*d_t;
        next.x=present.x+present.v*(sin(next.theta)-sin(present.theta))/present.w;
        next.y=present.y-present.v*(cos(next.theta)-cos(present.theta))/present.w;
    }
    else
    {
        next.theta=present.theta;
        next.x=present.x+present.v*d_t*cos(present.theta);
        next.y=present.y+present.v*d_t*sin(present.theta);
    }
}

inline void relative_path_error()
{
    e_x=(target.x-present.x)*cos(target.theta)+(target.y-present.y)*sin(target.theta);
    e_y=(target.x-present.x)*sin(target.theta)+(target.y-present.y)*cos(target.theta);
    e_theta=target.theta-present.theta;
}

void velocity_control()
{
    e_xdot=target.v-present.v*cos(e_theta)+target.w*e_y;
    v_s=e_xdot+sqrt(2*a_max*abs_val(e_x))*sgn_fun(e_x);
    a_c=clamp(v_s/d_t,a_max);
    next.v=present.v+a_c*d_t;
}

inline void wheel_velocity()
{
    theta_p=target.theta+atan(3*c_x*pow(e_y/c_x,2/3)*sgn_fun(e_y));
    e_ydot=present.v*sin(e_theta)-target.w*e_x;
    w_p=target.w+(2*pow(e_y/c_x,-1/3)/(1+tan(theta_p-target.theta)*tan(theta_p-target.theta)))*e_ydot*sgn_fun(e_y);
    w_s=w_p+sqrt(2*alpha_max*abs_val(theta_p-present.theta))*sgn_fun(theta_p-present.theta);
    alpha_c=clamp(w_s/d_t,alpha_max);
    next.w=present.w+alpha_c*d_t;
    wr_cmd=next.v/r_w+0.5*d_w*next.w/r_w;
    wl_cmd=next.v/r_w-0.5*d_w*next.w/r_w;
}
// frame size= 100
#define ENCODER_SUB_INTERVAL 10
#define NUM_IN_FRAME 100

//SmoothPath sp;
void IRAM_ATTR onEncoderTimer() {
    static volatile int global_counter = 0;

    static volatile bool right_status[2]={0};
    static volatile int right_counter = 0;
    static volatile byte right_offset = 0;
    static volatile byte right_binary[2] = {0};

    static volatile bool left_status[2]={0};
    static volatile int left_counter = 0;
    static volatile byte left_offset = 0;
    static volatile byte left_binary[2] = {0};

    static volatile byte right_sub_count[ENCODER_SUB_INTERVAL] = {0};
    static volatile short right_sub_head = 1;
    static volatile int right_accum_count = 0;
    
    static volatile byte left_sub_count[ENCODER_SUB_INTERVAL] = {0};
    static volatile short left_sub_head = 1;
    static volatile int left_accum_count = 0;

    /* =============Right Motor Encoder============= */
    // Obtain A,B Phase
    right_status[ENCODER_PHASE_A] = digitalRead(right_encoder_A);
    right_status[ENCODER_PHASE_B] = digitalRead(right_encoder_B);
    // Convert from Gray code to binary
    right_binary[BINARY_NOW_STATE] = (right_status[ENCODER_PHASE_A] << 1) 
        | (right_status[ENCODER_PHASE_A] ^ right_status[ENCODER_PHASE_B]);
    // State change
    if(right_binary[BINARY_NOW_STATE] != right_binary[BINARY_PRE_STATE]){
        right_offset = (right_binary[BINARY_NOW_STATE] - right_binary[BINARY_PRE_STATE]);
        // Determine direction
        if(right_offset == 255 || right_offset == 3)  right_counter++;
        else                                          right_counter--;
        right_binary[BINARY_PRE_STATE] = right_binary[BINARY_NOW_STATE];
    }
    /* =============Left Motor Encoder============= */
    // Obtain A,B Phase
    left_status[ENCODER_PHASE_A] = digitalRead(left_encoder_A);
    left_status[ENCODER_PHASE_B] = digitalRead(left_encoder_B);
    // Convert from Gray code to binary
    left_binary[BINARY_NOW_STATE] = (left_status[ENCODER_PHASE_A] << 1) 
        | (left_status[ENCODER_PHASE_A] ^ left_status[ENCODER_PHASE_B]);
    // State change
    if(left_binary[BINARY_NOW_STATE] != left_binary[BINARY_PRE_STATE]){
        left_offset = (left_binary[BINARY_NOW_STATE] - left_binary[BINARY_PRE_STATE]);
        // Determine direction
        if(left_offset == 255 || left_offset == 3)  left_counter--;
        else                                        left_counter++;
        left_binary[BINARY_PRE_STATE] = left_binary[BINARY_NOW_STATE];
    }
    /* Moving Speed Calculating Model */
    //
    // |_|_|_|_|_|_|_|_|_|_|
    //  ^
    // head
    // 
    // accum will collect the last 1000 value, with frame size 100, that is, repeat again after 10 times
    if(global_counter % NUM_IN_FRAME == 0){     // 1/10, 100
        right_sub_head = (right_sub_head + 1) % ENCODER_SUB_INTERVAL;
        right_accum_count -= right_sub_count[right_sub_head];       // Minus prevous value
        right_sub_count[right_sub_head] = right_counter;            // Push new value
        right_accum_count += right_sub_count[right_sub_head];       // Add new value
        right_counter = 0;
        Car_rotary_offset[RIGHT_MOTOR] = right_accum_count * WHEEL_ANGULAR_RATIO;
        
        left_sub_head = (left_sub_head + 1) % ENCODER_SUB_INTERVAL;
        left_accum_count -= left_sub_count[left_sub_head];
        left_sub_count[left_sub_head] = left_counter;
        left_accum_count += left_sub_count[left_sub_head];
        left_counter = 0;
        Car_rotary_offset[LEFT_MOTOR] = left_accum_count * WHEEL_ANGULAR_RATIO;
    }
    /* =============Global============= */
    if(global_counter >= SPEED_SAMPLE_COUNT){
/*        Car_rotary_offset[RIGHT_MOTOR] = right_counter * WHEEL_ANGULAR_RATIO;   // Already consider dt
        Car_rotary_offset[LEFT_MOTOR]  = left_counter * WHEEL_ANGULAR_RATIO;*/

        /* =======Calculate Absolute Position======= */  
        central_v = (Car_rotary_offset[LEFT_MOTOR] + Car_rotary_offset[RIGHT_MOTOR]) / 2;
        central_w = (Car_rotary_offset[LEFT_MOTOR] - Car_rotary_offset[RIGHT_MOTOR]) / Car_TRACK;

        /*
           double new_Car_theta = Car_theta + central_w * d_t;

           if( abs(central_w) > 0.00001){
           Car_position[X_AXIS] += central_v*(sin(new_Car_theta) - sin(Car_theta)) / central_w;
           Car_position[Y_AXIS] -= central_v*(cos(new_Car_theta) - cos(Car_theta)) / central_w;
           }
           else{
           Car_position[X_AXIS] += central_v*cos(Car_theta)*d_t;
           Car_position[Y_AXIS] += central_v*sin(Car_theta)*d_t;
           }
           Car_theta = new_Car_theta;*/
/*
        right_counter = 0;
        left_counter = 0;*/
        global_counter = 0;

        present.v = central_v;
        present.w = central_w;

        dead_reckoning();
        relative_path_error();
        velocity_control();
        wheel_velocity();
        set_car(&present, &next);

        right_pwm = wr_cmd * WHEEL_RADIUS;
        left_pwm = wl_cmd * WHEEL_RADIUS;    
    }
    global_counter++;
    // portENTER_CRITICAL_ISR(&timerMux);
    // right_motor_speed++;
    // portEXIT_CRITICAL_ISR(&timerMux);
}
QueueArray<Car> targets;

void setup() {
    Serial.begin(115200);
    #ifdef ENABLE_WIFI
    wifi_init();
    #endif
    pinMode(right_encoder_A, INPUT);
    pinMode(right_encoder_B, INPUT);
    pinMode(left_encoder_A, INPUT);
    pinMode(left_encoder_B, INPUT);
    ledcSetup(RIGHT_MOTOR_A_CHENNEL, MOTOR_FREQ, MOTOR_PWM_RESOLUTION);
    ledcSetup(RIGHT_MOTOR_B_CHENNEL, MOTOR_FREQ, MOTOR_PWM_RESOLUTION);
    ledcSetup(LEFT_MOTOR_A_CHENNEL, MOTOR_FREQ, MOTOR_PWM_RESOLUTION);
    ledcSetup(LEFT_MOTOR_B_CHENNEL, MOTOR_FREQ, MOTOR_PWM_RESOLUTION);
    ledcAttachPin(right_motor_A, RIGHT_MOTOR_A_CHENNEL);
    ledcAttachPin(right_motor_B, RIGHT_MOTOR_B_CHENNEL);
    ledcAttachPin(left_motor_A, LEFT_MOTOR_A_CHENNEL);
    ledcAttachPin(left_motor_B, LEFT_MOTOR_B_CHENNEL);
    /* Encoder Timer setting */
    // Use timer 0 as encoder timer
    // Set Prescaler to 80
    // Timer counts up(True), counts down(false)
    encoder_timer = timerBegin(1, PRESCALER, true);

    // Attach ISR to specific timer
    // Trigger in edge type(true)
    timerAttachInterrupt(encoder_timer, &onEncoderTimer, true);

    // Set OCR to 25 so 80MHz -> Prescaled = 4MHz
    // 1Mhz / 25 = 40kHz
    // Highest speed high length = 250us = 4KHz = 10 times ISR
    // Lowest speed high length = 2.5ms = 0.4KHz = 100 times ISR
    // Counter will reload while reaching OCR(true)
    timerAlarmWrite(encoder_timer, OCR_COUNTER, true);

    timerAlarmEnable(encoder_timer);

    Car temp;
    /*
     *     volatile double x;//x座標
    volatile double y;//y座標
    volatile double theta;//角度
    volatile double v;//速度
    volatile double w;//角速度
    */
    rest_car(&temp, 1000, 1000, 10, 0, 0);
    //targets.enqueue(temp);
    //target = targets.dequeue();
    rest_car(&target, &temp);
    //target = temp;

    rest_car(&present, 0, 0, 0, 0, 0);
}

class Timer{
  public:
    int duration;
    Timer(int dur){
      duration = dur;
      pre_time = millis();
    }
    bool update(){
      unsigned long now_time = millis();
      if(now_time-pre_time > duration){
        pre_time = now_time;
        return true;
      }
      return false;
    }
  private:
    unsigned long pre_time;
};
Timer timer_update(1000);
PID_Controller right_motor(12, 2, 0);
PID_Controller left_motor(12, 2, 0);
void loop() {
    ledcWrite(RIGHT_MOTOR_A_CHENNEL, 0);
    ledcWrite(LEFT_MOTOR_A_CHENNEL, 0);
    int r_out = right_motor.update(right_pwm, Car_rotary_offset[RIGHT_MOTOR]);
    int l_out = left_motor.update(left_pwm, Car_rotary_offset[LEFT_MOTOR]);
    //int r_out = right_motor.update(50, Car_rotary_offset[RIGHT_MOTOR]);
    //int l_out = left_motor.update(50, Car_rotary_offset[LEFT_MOTOR]);
    ledcWrite(RIGHT_MOTOR_B_CHENNEL, r_out);
    ledcWrite(LEFT_MOTOR_B_CHENNEL, l_out);
    #ifdef ENABLE_WIFI
    if(timer_update.update()){
      send_update(present.x, present.y, present.v, present.theta);
    Serial.print("cmd: ");
    Serial.print(right_pwm);
    Serial.print(", ");
    Serial.print(left_pwm);
    Serial.print(", err: ");
    Serial.print(r_out);
    Serial.print(", ");
    Serial.print(l_out);
    //Serial.print(", ");
    //Serial.print(e_theta);
    Serial.print(", pos: ");
    Serial.print(present.x);
    Serial.print(", ");
    Serial.print(present.y);
    Serial.print(", ");
    //Serial.print(target.theta);
    Serial.print(",  Velocity: ");
    Serial.print(Car_rotary_offset[RIGHT_MOTOR]);
    Serial.print(", ");
    Serial.println(Car_rotary_offset[LEFT_MOTOR]);
    }
    #endif
    /*
    String msg = "";
    msg += present.x;
    msg += ",";
    msg += present.y;
    send_data(msg);*/
    /*
    Serial.print(", present: ");
    Serial.print(present.x);
    Serial.print(", ");
    Serial.print(present.y);
    Serial.print(", ");
    Serial.println(present.theta);*//*
                                     Serial.print(r_out);
                                     Serial.print(", ");
                                     Serial.print(l_out);
                                     Serial.print(", Position: ");
                                     Serial.print(Car_position[X_AXIS]);
                                     Serial.print(", ");
                                     Serial.print(Car_position[Y_AXIS]);
                                     Serial.print(",  Theta: ");
                                     Serial.print(Car_theta);
                                     Serial.print(",  Velocity: ");
                                     Serial.print(Car_rotary_offset[RIGHT_MOTOR]);
                                     Serial.print(", ");
                                     Serial.println(Car_rotary_offset[LEFT_MOTOR]);
    /*
    Serial.print(wr_cmd);
    Serial.print(", ");
    Serial.println(wl_cmd);*/
    //delay(25);
    /*
       if(right_motor_speed > 0){
       portENTER_CRITICAL(&timerMux);
       right_motor_speed--;
       portEXIT_CRITICAL(&timerMux);
       Serial.println("hey");
       }*/
}
