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
    byte readBuff[11];
    Serial2.readBytes(readBuff,11);
    int buffLength = readBuff[3];
    for(int i=5; i<buffLength+5; ++i){
      char temp = readBuff[i];
      Serial.print(temp);
    }
    if(Serial2.read()>0){;}
  }

  // Unityに画像データを送る
  char temp = Serial.read();
  if(temp == 'h'){
    byte dataLength[4] = {(byte)0, (byte)0, (byte)0, (byte)0};
    byte dataType[1] = {(byte)0};
    byte signBody[4] = {(byte)0, (byte)0, (byte)0, (byte)0};
    byte dataBody[1] = {(byte)0};

    // 画像データ送信の合図
    Serial.println("Send Int ");
    dataLength[3] = (byte)5;
    dataType[0] = (byte)0;
    signBody[0] = (byte)1;
    Serial2.write(dataLength, 4);
    Serial2.write(dataType, 1);
    Serial2.write(signBody, 4);

    delay(5000);
    
    // 画像データ本体の送信
    dataLength[2] = (byte)2;
    dataLength[3] = (byte)129;
    dataType[0] = (byte)3;
    int j;
    for(j=1; j<=240; ++j){
      // サインs がunityから来たら640バイト送る
      // それを240回やる
      //while(Serial2.available()<=6){Serial.println("waiting data");}
      byte readBuff[6];
      Serial2.readBytes(readBuff,6);
      char sign = readBuff[5];
      //if(Serial2.read()>0){;}

      delay(100);

      //if(sign == 's'){
        // デバッグ用
        Serial.print("Send Data: ");
        Serial.println(j);

        // 640バイト送る部分
        Serial2.write(dataLength, 4);
        Serial2.write(dataType, 1);
        int i;
        for(i=1; i<=640; ++i){
          while(Serial2.availableForWrite()<=10){;}
          dataBody[0] = (byte)(i%100);
          Serial2.write(dataBody, 1);
        }

        delay(100);

      //}
    }
  }

}
