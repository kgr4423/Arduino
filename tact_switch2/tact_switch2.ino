// Example 02A 押している間、LEDが点灯
const int led = 13; // LEDはデジタルピン13に接続
const int BUTTON = 7; // タクトスイッチをデジタルピン7に接続
int val = 0; // 入力ピンの状態を変数に記録

void setup() { 
  pinMode(led, OUTPUT); //デジタルピンを出力に設定
  pinMode(BUTTON, INPUT); //デジタルピンを入力に設定
}

void loop() {
  val = digitalRead(BUTTON) // 入力を読み取り(digitalRead)変数に格納
  If (val == HIGH){
    digitalWrite(LED, HIGH) // LED on
   } else {
    digitalWrite(LED, LOW) // LED off
   }
}
