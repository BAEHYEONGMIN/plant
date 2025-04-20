#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>

// WiFi 정보
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// 웹소켓 서버 URL
const char* serverUrl = "ws://baemin-simple-server.glitch.me";

// 웹소켓 클라이언트 객체 생성
WebSocketsClient webSocket;

// WiFi 연결 설정
void setup() {
  Serial.begin(115200);  // 시리얼 모니터 초기화
  WiFi.begin(ssid, password);  // WiFi 연결 시작

  // WiFi 연결 대기
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // 웹소켓 서버에 연결
  webSocket.begin(serverUrl);
  webSocket.onEvent(webSocketEvent);  // 이벤트 핸들러 설정
}

void loop() {
  webSocket.loop();  // 웹소켓 클라이언트 이벤트 처리
}

// 웹소켓 이벤트 핸들러
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.println("웹소켓 연결 끊김");
      break;
    case WStype_CONNECTED:
      Serial.println("웹소켓 서버와 연결됨");
      webSocket.sendTXT("Hello from ESP01!");  // 서버로 메시지 전송
      break;
    case WStype_TEXT:
      // 서버로부터 받은 텍스트 메시지 처리
      String message = String((char*)payload);
      Serial.println("서버로부터 메시지: " + message);
      
      // 예시: 받은 메시지가 "LED ON"이면 LED를 켬
      if (message == "LED ON") {
        // 여기에 LED 제어 코드 작성
        Serial.println("LED를 켭니다!");
      } else if (message == "LED OFF") {
        // 여기에 LED 제어 코드 작성
        Serial.println("LED를 끕니다!");
      }
      break;
  }
}