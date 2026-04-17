// 引入 MQTT 與 WiFi 函式庫
#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>

// WiFi 名稱與密碼
#define SECRET_SSID "WindyWifi"//Wifi名稱
#define SECRET_PASS "huang0801"//Wifi密碼

// LED矩陣控制函式庫
#include <LedControl.h>
#include <PubSubClient.h>

// WiFi 設定
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

// 建立 WiFi 與 MQTT 物件
WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);

// MQTT Broker 設定
const char broker[] = "test.mosquitto.org";
int        port     = 1883;

// 訂閱的 Topic
const char topic1[]  = "lhucin/D110IOT/Project/CarGTPeople";
const char topic2[]  = "lhucin/D110IOT/Project/CarLTPeople";
const char topic3[]  = "lhucin/D110IOT/Project/Other";

//LED矩陣宣告
// 第一個 LED 矩陣 (倒數秒數)
LedControl display = LedControl(A2,A1,A0,1);//倒數
// 第二個 LED 矩陣 (左轉箭頭)
LedControl display2 = LedControl(A5,A4,A3,1);//箭頭

int count = 0;

//倒數數字圖形
//0~9 的 LED 圖案 (8x8)
const uint64_t R_IMAGES[] = { 
  0x0000000000000000,//0 
  0x00000001ff410000,//1
  0x0000718985834100,//2
  0x00006e9191814200,//3
  0x0000ff4424140c00,//4
  0x00008e919191f200,//5
  0x00004e9191917e00,//6
  0x0000e09088878000,//7
  0x00006e9191916e00,//8
  0x00007e8989897200 //9
};
// 計算圖片數量
const int R_IMAGES_LEN = sizeof(R_IMAGES)/8;

//左轉箭頭 
const uint64_t R_IMAGES2[] = {
  0x38383838fe7c3810,//左轉箭頭
  0x0000000000000000//關
};
const int R_IMAGES2_LEN = sizeof(R_IMAGES2)/8;

//顯示倒數數字
void displayImage(uint64_t image) {
  for (int i = 0; i < 8; i++) {// 逐行控制 LED
    byte row = (image >> i * 8) & 0xFF;// 取得該列資料
    for (int j = 0; j < 8; j++) {
      display.setLed(0, i, j, bitRead(row, j));// 設定 LED 開 / 關
    }
  }
}

//顯示箭頭
void displayImage2(uint64_t image) {
  for (int i = 0; i < 8; i++) {
    byte row = (image >> i * 8) & 0xFF;
    for (int j = 0; j < 8; j++) {
      display2.setLed(0, i, j, bitRead(row, j));
    }
  }
}

//紅綠燈主流程
void LED(int Timer1,int Timer2,int Timer3) {
  //顯示設定時間
  Serial.println("Time1: "+String(Timer1)+ " 秒");
  Serial.println("Time2: "+String(Timer2)+ " 秒");
  Serial.println("Time3: "+String(Timer3)+ " 秒");

  //行人紅燈 → 綠燈
  for(count = Timer1;count >= 0;count--) {
    // 顯示倒數
    displayImage(R_IMAGES[count]);
    delay(1000);
  }

  //行人綠燈
  delay(1000);
  digitalWrite(6, LOW);
  digitalWrite(7, HIGH);

  //行人綠燈倒數
  for(count = Timer1;count >= 0;count--) {
    displayImage(R_IMAGES[count]);
    delay(1000);
  }

  //行人紅燈
  digitalWrite(7, LOW);
  digitalWrite(6, HIGH);

  delay(2000);//倒數2秒

  //車道1綠燈
  digitalWrite(8, LOW);
  digitalWrite(10, HIGH);

  //車道1 綠燈倒數
  for(count = Timer2;count >= 0;count--) {
    displayImage(R_IMAGES[count]);
    delay(1000);
  }

  //車道1黃燈
  digitalWrite(10, LOW);
  digitalWrite(9, HIGH);

  delay(2000);//倒數2秒

  //車道1紅燈
  digitalWrite(9, LOW);
  digitalWrite(8, HIGH);

 //車道1 左轉
  for(count = Timer2;count >= 0;count--) {
    displayImage(R_IMAGES[count]);
    //關閉左轉箭頭
    // 如果剩餘時間 > 3 秒
    if(count > 3){
      // 顯示左轉箭頭
      displayImage2(R_IMAGES2[0]);
    }else if(count < 3){
      // 關閉箭頭
      displayImage2(R_IMAGES2[1]);
    }
    delay(1000);
  }
  delay(1000);

   //車道2 綠燈
  digitalWrite(11, LOW);
  digitalWrite(13, HIGH);
  for(count = Timer3;count >= 0;count--) {
    displayImage(R_IMAGES[count]);
    delay(1000);
  }
  delay(1000);

  //車道2黃燈
  digitalWrite(12, HIGH);
  digitalWrite(13, LOW);

  delay(2000);//倒數2秒

  //車道2紅燈
  digitalWrite(11, HIGH);
  digitalWrite(12, LOW);


  //車道2 左轉
  for(count = Timer3;count >= 0;count--) {
    displayImage(R_IMAGES[count]);
    //關閉左轉箭頭
    if(count > 3){
      displayImage2(R_IMAGES2[0]);
    }else if(count < 3){
      displayImage2(R_IMAGES2[1]);
    }
    delay(1000);
  }
  delay(1000);

}
//初始化
void setup() {
  Serial.begin(9600);
   // 初始化倒數 LED
  display.clearDisplay(0);    // 清除螢幕
  display.shutdown(0, false);  // 關閉省電模式
  display.setIntensity(0, 10); // 設定亮度為 8 (介於0~15之間)

  // 初始化箭頭 LED
  display2.clearDisplay(0);    // 清除螢幕
  display2.shutdown(0, false);  // 關閉省電模式
  display2.setIntensity(0, 10); // 設定亮度為 8 (介於0~15之間)
  // 設定腳位為輸出
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(13, OUTPUT);

  //全體紅燈
  digitalWrite(6, HIGH);
  digitalWrite(8, HIGH);
  digitalWrite(11, HIGH);

  //WiFi 連線
  //嘗試連接到 WiFi 網路
  Serial.print("Wifi...");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // 連線失敗, 重試
    Serial.print("...");
    delay(5000);
  }
  Serial.println("Wifi OK");
  Serial.println();
  //MQTT 連線
  Serial.print("MQTT...");

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT未連接! 錯誤代碼 = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }
  Serial.println("MQTT OK");
  Serial.println();
  // 訂閱主題
  mqttClient.subscribe(topic1);
  mqttClient.subscribe(topic2);
  mqttClient.subscribe(topic3);
  Serial.print("Topic: ");
  Serial.println(topic1);
  Serial.print("Topic: ");
  Serial.println(topic2);
  Serial.print("Topic: ");
  Serial.println(topic3);
  Serial.println();
}
//主迴圈
void loop() {
  mqttClient.poll();
  // 如果 MQTT 斷線
  if (!mqttClient.connected()) {
    reconnect();
  }// 如果有訊息
  if (mqttClient.available()){
    MQTTMessage();
  }
}
//重新連線
boolean reconnect() {
  if (mqttClient.connect(broker, port)) {
    mqttClient.subscribe(topic1);
    mqttClient.subscribe(topic2); 
    mqttClient.subscribe(topic3);
  }
  return mqttClient.connected();
}
//接收 MQTT 訊息
void MQTTMessage() {
    String topic = mqttClient.messageTopic();
    String message = mqttClient.readString();
    if (message == "1") {// 如果收到 1 → 啟動紅綠燈流程
      if (topic == "lhucin/D110IOT/Project/CarGTPeople") {
        Serial.print("接收到主題: ");
        Serial.println(topic);
        Serial.print("為: ");
        Serial.println(message);
        LED(4,7,7);
      }
      if (topic == "lhucin/D110IOT/Project/CarLTPeople") {
        Serial.print("接收到主題: ");
        Serial.println(topic);
        Serial.print("為: ");
        Serial.println(message);
        LED(8,5,5);
      }
      if (topic == "lhucin/D110IOT/Project/Other") {
        Serial.print("接收到主題: ");
        Serial.println(topic);
        Serial.print("為: ");
        Serial.println(message);
        LED(6,6,6);
      }
    }else if(message == "0"){
      if(topic == "lhucin/D110IOT/Project/CarGTPeople"){
        Serial.print("接收到主題: ");
        Serial.println(topic);
        Serial.print("為: ");
        Serial.println(message);
      }
      if (topic == "lhucin/D110IOT/Project/CarLTPeople"){
        Serial.print("接收到主題: ");
        Serial.println(topic);
        Serial.print("為: ");
        Serial.println(message);
      }
      if(topic == "lhucin/D110IOT/Project/Other"){
        Serial.print("接收到主題: ");
        Serial.println(topic);
        Serial.print("為: ");
        Serial.println(message);
      }
    }
}

