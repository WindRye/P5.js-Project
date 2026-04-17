//讀取Video
let video;

//模型路徑
let classifier;
let imageModelURL = 'https://teachablemachine.withgoogle.com/models/2DNkUqA-O/';

// 控制是否發送訊息
let sendMessage = false;

//串接 MQTT 連線
var client = mqtt.connect("wss://broker.mqttgo.io:8084/mqtt");
// 如果上述伺服器無法使用，可改為測試伺服器
// var client = mqtt.connect("wss://test.mosquitto.org:8081");

// 訂閱主題
var topic = ["lhucin/D110IOT/Project/CarGTPeople","lhucin/D110IOT/Project/CarLTPeople","lhucin/D110IOT/Project/Other"];

// MQTT 連線成功後，訂閱主題
function EventoConectar() {
  for(let i = 0; i < 3; i++){
    client.subscribe(topic[i], function(err) {
    if (!err) {
      console.log(topic[i]+"訂閱成功");
      }
    })
  }
}
client.on("connect", EventoConectar);

//載入影像分類模型
function preload() {
  classifier = ml5.imageClassifier(imageModelURL + 'model.json');
}
                                   
function setup() {
  createCanvas(windowWidth, windowHeight);// 建立全螢幕畫布
  //讀取Video
  video = createCapture(VIDEO);
  video.hide();// 隱藏原始 video 元素，只在畫布顯示

  //開始影像分類
  classifyVideo();
  //影像分類函數
  function classifyVideo() {
    classifier.classify(video, gotResult);  
  }

  //取得分類結果
  function gotResult(error, results) {
    if (error) {
      console.error(error);
      return;
    }

  //console.log(results);
    label = results[0].label;
    conf = results[0].confidence;
    

let timer = 0; // 計時器初始值
let COOLDOWN = 55000; // 設定發送間隔時間(單位:毫秒)
  //控制
function sendMessages(){
  if (conf > 0.5) {
    if (label == "CarGTPeople") {
      if(millis() - timer > COOLDOWN) {
        client.publish("lhucin/D110IOT/Project/CarGTPeople", "1");
        client.publish("lhucin/D110IOT/Project/CarLTPeople", "0");
        client.publish("lhucin/D110IOT/Project/Other", "0");
        timer = millis(); // 更新計時器
        sendMessage = false; // 重置發送狀態
        console.log("CarGTPeople");
      }
    }
      else if (label == "CarLTPeople") {
        if(millis() - timer > COOLDOWN) {
          client.publish("lhucin/D110IOT/Project/CarGTPeople", "0");
          client.publish("lhucin/D110IOT/Project/CarLTPeople", "1");
          client.publish("lhucin/D110IOT/Project/Other", "0");
          timer = millis(); // 更新計時器
          sendMessage = false; // 重置發送狀態
          console.log("CarLTPeople");
        }
      }
      else if(label == "Other") {
      if(millis() - timer > COOLDOWN) {
          client.publish("lhucin/D110IOT/Project/CarGTPeople", "0");
          client.publish("lhucin/D110IOT/Project/CarLTPeople", "0");
          client.publish("lhucin/D110IOT/Project/Other", "1");
          timer = millis(); // 更新計時器
          sendMessage = false; // 重置發送狀態
          console.log("Other");
      }
    }
  }
} 
     if (!sendMessage) {
      setTimeout(sendMessages, COOLDOWN); // 設定延遲執行 sendMessages 函數
      sendMessage = true; // 設置發送狀態為 true
    }
    
    push();
    strokeWeight(4);      // 外框粗細
    stroke(236, 125, 16); // 外框顏色
    fill(255, 255, 0);
    textSize(30);
    text(label, width/5, height*1/2);
    text(conf, width/5, height*1/2+40);
    pop();
    classifyVideo();
  }
}
function draw() {
  background(250);
  //顯示影像
  image(video, 0, 0);
}