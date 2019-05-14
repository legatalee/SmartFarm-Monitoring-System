#include "ESP8266WiFi.h"
#include "WebSocketsServer.h"
//Wifi 연결하기
const char* ssid = "legatalee_laptop";
const char* password = "dnflwlq4";
float humi;
float temp;
int light;

String data;

//WebSocket
WebSocketsServer webSocket = WebSocketsServer(80);
//Callback function(웹소켓 메세지를 수신하면 실행)
void onWebSocketEvent(uint8_t num,
                      WStype_t type,
                      uint8_t * payload,
                      size_t length) {
  //웹소켓이벤트의 타입
  switch (type) {
    //클라이언트 연결 해제시
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    //클라이언트 연결시
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected!\n", num);
        Serial.println(ip.toString());
      }
      break;

    //클라이언트에게 응답을 보낸다.
    case WStype_TEXT:
      payload = (uint8_t*)data.c_str();
      Serial.printf("[%u] Text: %s\n", num, payload);
      webSocket.sendTXT(num, payload);
      //      webSocket.sendTXT(num, payload);
      break;

    //기타 필요에 따라
    case WStype_BIN:
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    default:
      break;
  }
}

#include <DHT11.h>
DHT11 dht11(D4);

void setup() {
  Serial.begin(115200);

  connectWifi();
  //Start websocket server
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);//콜백실행
  dht11.read(temp, humi);
}
int cnt = 0;
void loop() {
  String temporary = "";
  
  if (cnt >= 20000) {
    dht11.read(temp, humi);
    cnt=0;
    Serial.println("read!");
  }
  cnt++;

  webSocket.loop();
//  Serial.println(temp);
//  Serial.println(humi);

  light = random(101);

  temporary += (int)humi;
  temporary += ",";
  temporary += (int)temp;
  temporary += ",";
  temporary += light;

  data = temporary;
}

//wifi
void connectWifi() {
  Serial.print("Connecting to ");
  Serial.print(ssid);
  WiFi.disconnect(true);

  IPAddress ip(192, 168, 137, 228);
  IPAddress gateway(192, 168, 137, 1);
  IPAddress subnet(255, 255, 255, 0);

  WiFi.config(ip, gateway, subnet);

  delay(500);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP Address: ");
  Serial.print(WiFi.localIP());
  Serial.println("");
}
