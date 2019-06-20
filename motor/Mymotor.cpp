#include"Mymotor.h"

const double rad = (360/(reso*2)) * 1000 / 57296;//一格
char mode;
double vcmd1 = 0 ,vcmd2 = 0;
const double KP_1 = 10 , KI_1 = 10;
const double KP_2 = 16.6667 , KI_2 = 16.6667;
volatile int PWM1 = 0 , PWM2 = 0;

void EncoderInit(){
  TCCR2A = 0;
  TCCR2B = 0; 
  TCCR2B |= (1<<WGM22);
  TCCR2B |= (1<<CS22) | (1<<CS20);
  TIMSK2 |= (1 << OCIE2A);
  TCNT2=0;
  OCR2A = 15; //頻率1000
}

void EncoderDisable(){
  TCCR2A = 0;
  TCCR2B = 0; 
}

ISR(TIMER2_COMPA_vect){
  static volatile byte counter_diff1, counter_diff2;
  static volatile byte encoder_pre1 = 0, encoder_current1 = 0;
  static volatile byte encoder_pre2 = 0, encoder_current2 = 0;
  static volatile double speed1 = 0 , speed2 = 0;
  static volatile byte counter = 0;
  //static只會被初始化一次 volatile->因為interrupt要使用
  encoder_current1 = digitalRead(ENCODER1);
  encoder_current2 = digitalRead(ENCODER2);

  if(counter >= 100){//頻率10
    speed1 = (counter_diff1 * rad) * 0.0675;
    PWM1 = pi_control(1,vcmd1,speed1);
    speed2 = (counter_diff2 * rad) * 0.0675;
    PWM2 = pi_control(2,vcmd2,speed2);
    counter_diff1 = 0; counter_diff2 = 0;
    counter = 0; 
  }

  if(encoder_current1 != encoder_pre1)
    counter_diff1 ++;
  if(encoder_current2 != encoder_pre2)
    counter_diff2 ++;

  counter++;
  encoder_pre1 = encoder_current1;
  encoder_pre2 = encoder_current2;
}


int pi_control(int num , double vcmd , double sp){
  static double ERR = 0 , G = 0 , in = 0;
  static int PWM = 0;
  static double G_1 , G_2 , in_1 , in_2;

  ERR = vcmd - sp;
  if(num == 1){//motor1
    G_1 = ERR * KP_1;
    in_1 = in_1 + ERR * 1 * KI_1;
    G = G_1; in = in_1;
  }
  else{//motor2
    G_2 = ERR * KP_2;
    in_2 = in_2 + ERR * 1 * KI_2;
    G = G_2; in = in_2;
  }
  if(G > 255)
    G = 255;
  else if (G < -255)
    G = -255;

  if(in > 255)
    in = 255;
  else if(in < -255)
    in = -255;

  PWM = G + in;
  return (abs(PWM) >=150 ? 150 : PWM);
}

void change_vcmd(char mode){
  switch(mode){
    case 'F': //forward
      vcmd1 = 1;
      vcmd2 = 1;
      break;
    case 'R'://turn right
      vcmd1 = 0.6;
      vcmd2 = 0.3;
      break;
    case 'L'://turn left
      vcmd1 = 0.3;
      vcmd2 = 0.6;
      break;
    case 'S'://stop
      vcmd1 = 0;
      vcmd2 = 0;
  }
}
