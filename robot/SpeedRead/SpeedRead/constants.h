#ifndef _CONSTANTS_H
#define _CONSTANTS_H

/* Pin Setting */
#define right_encoder_A 34
#define right_encoder_B 35
#define left_encoder_A 36
#define left_encoder_B 39

#define right_motor_A 18
#define right_motor_B 19
#define left_motor_A 32
#define left_motor_B 33

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

/* Physical Constants*/
#define X_AXIS 0
#define Y_AXIS 1

#define Car_TRACK 103.0     // mm
#define WHEEL_RADIUS 16.5   // mm

#endif
