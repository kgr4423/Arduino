int  pin  = 0;
int  pin2 = 13;
char data;
int  val;

void setup() {
  Serial.begin(115200);
  pinMode(pin2, OUTPUT);
}

void loop()
{
  if ( Serial.available()  > 0 ) {   // 受信データがあるか？
    data = Serial.read();            // 1文字だけ読み込む
    if (data == '1') { 
      digitalWrite(pin2, HIGH);
    }else if(data == '0'){
      digitalWrite(pin2, LOW);
    }
    while(Serial.available())Serial.read(); //受信データバッファをクリア
  }
  val = analogRead(pin);//CdSの電圧値を読み取る
  Serial.print(val);
  Serial.println();
  delay(100);
}
