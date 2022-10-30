#include<Servo.h>

Servo servo;

void setup() {
  // put your setup code here, to run once:
  servo.attach(3);
}

void loop() {
  // put your main code here, to run repeatedly:
  int val=analogRead(0);

  int deg=map(val, 0, 1023, 0, 180);
  servo.write(deg);
}
