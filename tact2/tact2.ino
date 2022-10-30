//押したあと、指を離してもLEDが点灯

const int led = 13; 
const int BUTTON = 7; 

int val = 0;      // 入力ピンの状態を記録
int old_val = 0;  // 1つ前の入力ピンの状態を記録
int state = 0;    // LEDの状態を記録

void setup() {   
  pinMode(led, OUTPUT);
  pinMode(BUTTON, INPUT);
}

void loop(){
  //入力をvalに入れる
  val = digitalRead(BUTTON);

  //スイッチを押した瞬間だけstateを更新
  if((val==HIGH)&&(old_val==LOW)){
    if(state == 0){
      state = 1;
    }else{
      state = 0;
    }
  }

  //古くなったvalの値を入れる
  old_val = val;

  //stateが1ならオン、0ならオフ
  if(state == 1){
    digitalWrite(LED, HIGH);
  }else{
    digitalWrite(LED, LOW);
  }
}
