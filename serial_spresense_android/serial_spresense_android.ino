#define BAUDRATE  9600
#define BAUDRATE2 38400

void setup() {
  Serial.begin(BAUDRATE);
  Serial2.begin(BAUDRATE2);
  while(Serial2.available()>0){Serial2.read();}
}

void loop() {
  
  // Unityからのデータを読み取る
  if(Serial2.available() > 0){
    char receivedByte = Serial2.read();
    Serial.print(receivedByte);
    Serial.print(' ');
  }

  // Unityに画像データを送る
  char temp = Serial.read();
  if(temp == 'h'){
    byte sendByte[1];

    // 画像データ送信の合図
    Serial.println("Send sign");
    sendByte[0] = 't';
    for(int i=1; i<=20; ++i){
      Serial2.write(sendByte, 1);
    }

    delay(4000);

    // 画像データ本体の送信
    for(int i=1; i<=240; ++i){
    Serial.print("Send byte : ");
    Serial.println(i);
      for(int j=1; j<=640; ++j){
        while(Serial2.availableForWrite()<=10){;}
        sendByte[0] = (byte)(j%100);
        Serial2.write(sendByte, 1);
      }
    }
  }

}
