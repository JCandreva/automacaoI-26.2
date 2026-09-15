void setup() {
  // put your setup code here, to run once:
  pinMode(11, OUTPUT); // verde
  pinMode(13, OUTPUT);  // amarelo
  pinMode(12, OUTPUT); // vermelho
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(11, HIGH);
  Serial.println("Verde");
  delay(3000);
  digitalWrite(11, LOW);
  digitalWrite(12, HIGH);
  Serial.println("Amarelo");
  delay(1000);
  digitalWrite(12, LOW);
  digitalWrite(13, HIGH);
  Serial.println("Vermelho");
  delay(4000);
  digitalWrite(13, LOW);
}
