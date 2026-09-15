#include <Servo.h>
Servo s1;
bool aberto = false;
int i;
void setup() {
  // put your setup code here, to run once:
  s1.attach(7);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(10, OUTPUT);
  s1.write(0);
}

void loop() {
  // // put your main code here, to run repeatedly:
  if (!digitalRead(2)){
 
      if (aberto){ 
        digitalWrite(3, HIGH);
        tone(10, 1000, 2000);
        delay(3000);
        tone(10, 1200, 2000);
        delay(3000);
        tone(10, 1500, 2000);
        delay(3000);
        for (i=0; i<180 ; i++) {
          s1.write(i);
          delay(15);
        }
        aberto = false;
        digitalWrite(3, LOW);
      }
      else{
        digitalWrite(4, HIGH);
        for (i=179; i>=0; i--) {
          s1.write(i);
          delay(15);
        }
        aberto = true;
        digitalWrite(4, LOW);
      }
      while(!digitalRead(2)){
        delay(50);
      }
  }
  
}

