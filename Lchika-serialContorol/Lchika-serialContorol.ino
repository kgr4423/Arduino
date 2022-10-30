//シリアル通信を利用してLEDのオンオフを制御

void setup(){
  Serial.begin(9600);  //シリアルポートを開く・毎秒9600bitでコンピュータにデータを通信
  pinMode(13, OUTPUT);
}

void loop(){
  //シリアル通信よりnが入力されたらLEDオン、fが入力されたらLEDオフ
  if(Serial.available()>0){
    char c=Serial.read();
    if(c=='n'){
      digitalWrite(13, HIGH);
    }else if(c=='f'){
      digitalWrite(13, LOW);
    }
  }
}
