#ifndef _CONSTANTS_H
#define _CONSTANTS_H

#define ROBOT_ID "R01"
#define MODE_UPDATE 0

/* Pin Setting */
#define right_encoder_A 39
#define right_encoder_B 36
#define left_encoder_A 35
#define left_encoder_B 34

#define right_motor_A 32
#define right_motor_B 33
#define left_motor_A 18
#define left_motor_B 19

#define ultraA_echo 22
#define ultraA_trig 23
#define ultraA_servo 4
#define ultraB_servo 2

/* Math Constant */
#define Math_PI 3.1415926

/* Timer Setting*/
// 80MHz / 80 = 1MHz
//  1MHz / 25 = 40KHz
#define PRESCALER 80
#define OCR_COUNTER 25
// Calculate rate = 40Hz = 1/40s
#define SPEED_SAMPLE_COUNT 1000
// 3.14/2*40/50*2
// WHEEL_ANGULAR_RATIO ENCODER_NUM_TICK*WHEEL_RADIUS*40/WHEEL_REDUCTION
// 1 Rotation = 1400 encoder signal
// 1 encoder signal = 37mm * PI / 1400 = 0.083 mm  per interrupt cycle(25ms)
// 0.083(mm/encoder/(25ms)) -> 3.32(mm/encoder/s)

/* Motor PWM Setting */
#define MOTOR_FREQ 5000
#define MOTOR_PWM_RESOLUTION 5000
#define RIGHT_MOTOR_A_CHENNEL 0
#define RIGHT_MOTOR_B_CHENNEL 1
#define LEFT_MOTOR_A_CHENNEL 2
#define LEFT_MOTOR_B_CHENNEL 3

/* Encoder Setting*/
#define WHEEL_ANGULAR_RATIO 3.3211
//#define WHEEL_ANGULAR_RATIO 1
#define BINARY_NOW_STATE 0
#define BINARY_PRE_STATE 1
#define ENCODER_PHASE_A 0
#define ENCODER_PHASE_B 1

#define RIGHT_MOTOR 1
#define LEFT_MOTOR 0

/* Ultrasonic Sensor Setting*/
#define ULTRA_MAX_DIS 250       // If distance exceed this value, treat as maximum value
#define ULTRA_INIT_DELAY 10     // Interval of trigger signal (us)
#define ULTRA_TURN_RATIO 5      // Delay time of servo, (ms/degree)
#define ULTRA_SERVO_MIN 20
#define ULTRA_SERVO_MAX 160

/* Map*/
#define MAP_STATUS_UNKNOWN 0
#define MAP_STATUS_EMPTY 1
#define MAP_STATUS_OCCUPIED 2
#define MAP_STATUS_CONFLICT 3

/* Physical Constants*/
#define X_AXIS 0
#define Y_AXIS 1

#define Car_TRACK 103.0     // mm
#define WHEEL_RADIUS 16.5   // mm

/* Wifi Transfering Setting */
#define JSON_BYTE_SIZE     300
#define STRING_BUFFER_SIZE 600


#endif
