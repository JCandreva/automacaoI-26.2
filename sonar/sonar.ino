#include <Servo.h>
#include <Ultrasonic.h>
#include <LiquidCrystal_I2C.h>

Servo s1;
Ultrasonic u1(4, 5);
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  // put your setup code here, to run once:
  pinMode(3, OUTPUT);
  pinMode(52, OUTPUT);
  pinMode(53, OUTPUT);
  s1.attach(2);
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("LIVRE!!!");
  }

void loop() {
  // put your main code here, to run repeatedly:
  int i;
  for (i=0; i<180 ; i++) {
      iteracaovolta(i);
    }
  for (i=179; i>=0; i--) {
      iteracaovolta(i);
  }
}


void iteracaovolta(int i){
  s1.write(i);
  if(u1.read() < 15){
    lcd.clear();
    lcd.print("Objeto Detectado");
    lcd.setCursor(0, 1);
    lcd.print("a ");
    lcd.print(u1.read());
    lcd.print("cm");
    digitalWrite(52, LOW);
    digitalWrite(53, HIGH);
    tone(3, 500);
    while(u1.read() < 15){
      delay(100);
    }
  }
  else{
    noTone(3);
    digitalWrite(52, HIGH);
    digitalWrite(53, LOW);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("LIVRE!!!");
  }

  delay(15);
}