void setup() {
  // put your setup code here, to run once:
  pinMode(2, INPUT);
  pinMode(3, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(digitalRead(2));
  if (digitalRead(2)) {
    tone(3, 500);
    delay(500);
  }
  else{
    noTone(3);
  }
}
