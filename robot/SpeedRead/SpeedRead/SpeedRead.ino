
// pointer of timer
hw_timer_t *encoder_timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

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
        t++;
    }
    global_counter++;
    // portENTER_CRITICAL_ISR(&timerMux);
    // right_motor_speed++;
    // portEXIT_CRITICAL_ISR(&timerMux);
}
void setup() {

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
    Serial.print(car_rotary_offset[RIGHT_MOTOR]);
    Serial.print(", ");
    Serial.println(car_rotary_offset[LEFT_MOTOR]);
    pre_t = t;
    delay(100);
    /*
       if(right_motor_speed > 0){
       portENTER_CRITICAL(&timerMux);
       right_motor_speed--;
       portEXIT_CRITICAL(&timerMux);
       Serial.println("hey");
       }*/
}
