
// Bluetoothモジュール HC-05(HC-06) をATモードで設定確認・変更するためのスケッチ
// シリアルモニタを使用してHC-05(HC-06)と通信する
//
// HC-05はスイッチを押しながら電源投入しスイッチを離すとATモードに入る
// HC-06は電源投入と同時にATモードに入る
// ATモードに入るとLEDが1秒間隔で点滅する
//
// デフォルトのボーレートは HC-05（34800bps)・HC-06(9600bps)
// HC-05は［NL+CR］、HC-06は［改行なし］でコマンド送信
//
// 

 
 
void setup() 
{
    Serial.begin(9600);
    Serial2.begin(38400);
    Serial.println("AT コマンドを入力 :"); 
}
 
void loop()
{
    // HC-05(HC-06)から読み取りArduinoIDEのシリアルモニタに送信する
    if (Serial2.available())
    {  
        Serial.write(Serial2.read());
    }
 
    // ArduinoIDEのシリアルモニタからの入力を読み取りHC-05(HC-06)に送信する
    if (Serial.available())
    {
        Serial2.write(Serial.read());
    }
}
