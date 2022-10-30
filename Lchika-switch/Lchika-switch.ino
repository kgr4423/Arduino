//スイッチを押している間、LEDをオンにするプログラム
void setup() {
  pinMode(13, OUTPUT); 
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(13, HIGH);
  delay(1000);
}
