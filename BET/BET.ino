#include <Servo.h>
Servo s1;
int i;
void setup() {
  // put your setup code here, to run once:
  s1.attach(7);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(10, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // // put your main code here, to run repeatedly:
  for (i=0; i<180 ; i++) {
  digitalWrite(3, HIGH);
  digitalWrite(4, LOW);
  unidadeloop(i);
  }
  for (i=179; i>=0; i--) {
    digitalWrite(3, LOW);
    digitalWrite(4, HIGH);
    unidadeloop(i);
  }
}

void unidadeloop(int i){
    s1.write(i);
    delay(15);
    if(!digitalRead(2)){
      digitalWrite(4, LOW);
      digitalWrite(3, LOW);
      Serial.println("Advinhe o grau:");
      while(!Serial.available()){
          // faz nada de proposito
      }
      int grau = Serial.parseInt();
      int diff = abs(i - grau);
      int porc = (diff * 100) / 180;
      Serial.print("O grau era: ");
      Serial.print(i);
      Serial.print(" e você tentou: ");
      Serial.print(grau);
      Serial.print(". Errou por: ");
      Serial.print(diff);
      Serial.print("(");
      Serial.print(porc);
      Serial.println("%)");
      if (porc < 6){
          tone(10, 1000, 500);
          digitalWrite(3, HIGH);
          delay(250);
          digitalWrite(3, LOW);
          delay(500);
          tone(10, 1000, 500);
          digitalWrite(3, HIGH);
          delay(250);
          digitalWrite(3, LOW);
          delay(500);
          tone(10, 1000, 500);
          digitalWrite(3, HIGH);
          delay(250);
          digitalWrite(3, LOW);
          delay(500);
      }
    else{
      tone(10, 100, 1500);
      digitalWrite(4, HIGH);
      delay(250);
      digitalWrite(4, LOW);
      delay(500);
      digitalWrite(3, LOW);
      digitalWrite(4, HIGH);
      delay(250);
      digitalWrite(4, LOW);
      delay(500);
      digitalWrite(3, LOW);
      digitalWrite(4, HIGH);
      delay(250);
      digitalWrite(4, LOW);
      delay(500);
      
    }
    }
   
}
