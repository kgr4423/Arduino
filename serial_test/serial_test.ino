void setup() {
  Serial.println("setup");
  Serial.begin(9600);
  Serial2.begin(9600);
  while (Serial.available() > 0) Serial.read();
  while (Serial2.available() > 0) Serial2.read();
}

void loop() {
  //Serial.println("in loop");
  //uint8_t data = 3;
  //Serial2.write(data);

  while (Serial.available() > 0) {
    int inByte0 = Serial.read();
    Serial.println("in 1");
    Serial2.write(inByte0);
  }

  while (Serial2.available() > 0) {
    Serial.println("in 2");
    int inByte1 = Serial2.read();
    Serial.write(inByte1);
  }

  delay(100);
}
