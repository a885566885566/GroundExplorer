#include "Mymotor.h"

void setup() {
  pinMode(GPIO1, OUTPUT);
  pinMode(MOTOR1, OUTPUT);
  pinMode(GPIO2, OUTPUT);
  pinMode(MOTOR2, OUTPUT);
  pinMode(ENCODER1,INPUT);
  pinMode(ENCODER2,INPUT);

  EncoderInit();//encoder
  Serial.begin(115200);
}

void loop()
{  
  if(Serial.available() > 0){
    mode = Serial.read();
    change_vcmd(mode);
  }
  analogWrite(MOTOR1 , PWM1);
  analogWrite(MOTOR2 , PWM2);
  delay(100);
}
