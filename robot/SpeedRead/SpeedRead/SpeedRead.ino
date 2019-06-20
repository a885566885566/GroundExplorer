
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

#define right_encoder_A 34
#define right_encoder_B 35
#define left_encoder_A 36
#define left_encoder_B 39

#define right_motor_A 18
#define right_motor_B 19
#define left_motor_A 32
#define left_motor_B 33

#define Math_PI 3.1415926

// 80MHz / 80 = 1MHz
//  1MHz / 25 = 40KHz
#define PRESCALER 80
#define OCR_COUNTER 25
// Calculate rate = 40Hz = 1/40s
#define SPEED_SAMPLE_COUNT 1000
// 3.14/2*40/50*2
// WHEEL_ANGULAR_RATIO ENCODER_NUM_TICK*WHEEL_RADIUS*40/WHEEL_REDUCTION
#define WHEEL_ANGULAR_RATIO 2.513

#define BINARY_NOW_STATE 0
#define BINARY_PRE_STATE 1
#define ENCODER_PHASE_A 0
#define ENCODER_PHASE_B 1

#define RIGHT_MOTOR 1
#define LEFT_MOTOR 0
#define X_AXIS 0
#define Y_AXIS 1

#define CAR_TRACK 10.0
#define WHEEL_RADIUS 2.0

#define d_t 1/40;

volatile int car_rotary_offset[2] = {0};
volatile double car_position[2] = {0};
volatile double car_theta = 0;

volatile double central_v = 0;
volatile double central_w = 0;
int t = 0;
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
    /* =============Global============= */
    if(global_counter >= SPEED_SAMPLE_COUNT){
        car_rotary_offset[RIGHT_MOTOR] = right_counter * WHEEL_ANGULAR_RATIO;
        car_rotary_offset[LEFT_MOTOR]  = left_counter * WHEEL_ANGULAR_RATIO;

        /* =======Calculate Absolute Position======= */
        /*
        double d_theta = (double)(car_rotary_offset[LEFT_MOTOR] - car_rotary_offset[RIGHT_MOTOR]) / CAR_TRACK;
        //double radius = car_rotary_offset[RIGHT_MOTOR] / abs(d_theta) + CAR_TRACK/2;
        //double central_offset = radius * d_theta;
        int i = (d_theta>0) ? 1 : -1;
        double central_offset = i * car_rotary_offset[RIGHT_MOTOR] + CAR_TRACK*d_theta/2;
        if(car_rotary_offset[RIGHT_MOTOR] >= 0 && car_rotary_offset[LEFT_MOTOR] >= 0 ){
            car_position[X_AXIS] += central_offset * sin(d_theta);
            car_position[Y_AXIS] += central_offset * cos(d_theta);
            car_theta += d_theta;
        }
        else{
            car_position[X_AXIS] += central_offset * sin(d_theta);
            car_position[Y_AXIS] -= central_offset * cos(d_theta);
            car_theta -= d_theta;
        }*/
        
        central_v = (car_rotary_offset[LEFT_MOTOR] + car_rotary_offset[RIGHT_MOTOR]) / 2;
        central_w = (car_rotary_offset[LEFT_MOTOR] - car_rotary_offset[RIGHT_MOTOR]) / CAR_TRACK;
        double new_car_theta = car_theta + central_w * d_t;

        if( abs(central_w) > 0.1){
            car_position[X_AXIS] += central_v*(sin(new_car_theta) - sin(car_theta)) / central_w;
            car_position[Y_AXIS] -= central_v*(cos(new_car_theta) - cos(car_theta)) / central_w;
            car_theta = new_car_theta;
        }
        else{
            car_position[X_AXIS] += central_v*cos(car_theta)*d_t;
            car_position[Y_AXIS] += central_v*sin(car_theta)*d_t;
        }

        right_counter = 0;
        left_counter = 0;
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
        t++;
    }
    global_counter++;
    // portENTER_CRITICAL_ISR(&timerMux);
    // right_motor_speed++;
    // portEXIT_CRITICAL_ISR(&timerMux);
}
void setup() {
    Serial.begin(115200);
    #ifdef ENABLE_WIFI
    wifi_init();
    #endif

    pinMode(right_encoder_A, INPUT);
    pinMode(right_encoder_B, INPUT);
    pinMode(left_encoder_A, INPUT);
    pinMode(left_encoder_B, INPUT);
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
    Serial.begin(115200);
}

int pre_t = 0;
void loop() {
    Serial.print(t-pre_t);
    Serial.print("Position: ");
    Serial.print(car_position[X_AXIS]);
    Serial.print(", ");
    Serial.print(car_position[Y_AXIS]);
    Serial.print(",  Theta: ");
    Serial.print(car_theta);
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
                                     Serial.println(Car_rotary_offset[LEFT_MOTOR]);*/
    /*
    Serial.print(wr_cmd);
    Serial.print(car_rotary_offset[RIGHT_MOTOR]);
    Serial.print(", ");
    Serial.println(car_rotary_offset[LEFT_MOTOR]);
    pre_t = t;
    delay(100);*/
    /*
       if(right_motor_speed > 0){
       portENTER_CRITICAL(&timerMux);
       right_motor_speed--;
       portEXIT_CRITICAL(&timerMux);
       Serial.println("hey");
       }*/
}
