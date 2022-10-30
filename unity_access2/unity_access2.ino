int pin = 0;
int val = 0;

void setup() {
  Serial.begin(115200); //シリアルポートを115200bpsで開く
}

void loop()
{
  val = analogRead(pin);//CdSの電圧値を読み取る
  Serial.print(val);
  Serial.println();
  delay(100);
}
