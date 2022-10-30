//押している間、LEDが点灯

const int led = 13;
const int BUTTON = 7;

int val = 0; //入力ピンの状態を記録

void setup() { 
  pinMode(led, OUTPUT);
  pinMode(BUTTON, INPUT);
}

void loop() {
  //入力をvalに入れる
  val = digitalRead(BUTTON);
  
  //入力値valの値に応じてオンオフ
  if (val == HIGH){
    digitalWrite(LED, HIGH);
  } else {
    digitalWrite(LED, LOW);
  }
}
