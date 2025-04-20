#include <SoftwareSerial.h>
 
const String ssid   = "KT_GiGA_3A57"; 
const String password = "8aek04zh16";
#define RX 2
#define TX 3
SoftwareSerial espSerial(RX, TX); // RX, TX

//함수 선언부분
void wait();
String sendAT(String cmd);
char sendRequest(String request);
bool waitForWifi(unsigned long timeout = 15000);

void setup() {
  Serial.begin(9600);          // PC와 연결
  espSerial.begin(9600);       // ESP-01과 연결
  String result = "";
  delay(2000);
  sendAT("AT");                // 모듈 응답 확인
  sendAT("AT+CWMODE=1");       // WiFi Station 모드
  
  String connect_query = "AT+CWJAP=\""+ssid+"\",\""+password+"\"";
  bool wifi = false;
  int wifi_fail_cnt = 0;
  while(wifi==false){
    sendAT(connect_query);  // WiFi 연결
    wifi = waitForWifi();
    wifi_fail_cnt+=1;
    if (wifi_fail_cnt==5){
      return -1;
    }
  }
  delay(2000);

  result = sendAT("AT+CWJAP?"); // 현재 연결된 WiFi 정보 출력
  Serial.println(result);

  sendAT("AT+CIPSTART=\"TCP\",\"baemin-simple-server.glitch.me\",80");
  // HTTP GET 요청
  String request = 
      "GET /led/on HTTP/1.1\r\n"
      "Host: baemin-simple-server.glitch.me\r\n"
      "User-Agent: ESP8266\r\n"
      "Connection: close\r\n\r\n";
  sendRequest(request);
}

void loop() {
}

String sendAT(String cmd) {
  espSerial.println(cmd);
  Serial.println(">>>>>>>> " + cmd);
  unsigned long t_start = millis();
  String response = "";
  while (millis() - t_start < 5000) {
    while (espSerial.available()) {
      char c = espSerial.read();
      response += c;
      if (response.indexOf("OK") != -1 || 
          response.indexOf("ERROR") != -1 || 
          response.indexOf("FAIL") != -1 || 
          response.indexOf("ALREADY CONNECTED") != -1) {
        return response;
      }
    }
  }
  return response;
}
void wait(){
  unsigned long timeout = millis();
  bool promptReceived = false;
  while (millis() - timeout < 5000) {
    if (espSerial.available()) {
      char c = espSerial.read();
      Serial.write(c);
      if (c == '>') {
        promptReceived = true;
        break;
      }
    }
  }
}
bool waitForWifi(unsigned long timeout = 15000) {
  String buffer = "";
  unsigned long start = millis();

  while (millis() - start < timeout) {
    while (espSerial.available()) {
      char c = espSerial.read();
      buffer += c;
      Serial.write(c);

      if (buffer.indexOf("WIFI GOT IP") != -1) {
        Serial.println("[✓] WiFi 연결 성공");
        return true;
      }
    }
  }
  Serial.println("[!] WiFi 연결 실패 또는 타임아웃");
  return false;
}

char sendRequest(String request) {
  char buffer[512];
  int index = 0;

  // 요청 전송 준비
  sendAT("AT+CIPSEND=" + String(request.length()));
  wait();  // '>' 프롬프트 대기
  espSerial.print(request);

  // 응답 수신
  unsigned long start = millis();
  unsigned long lastReceive = millis();

  while (millis() - lastReceive < 500) {
    if (espSerial.available()) {
      char c = espSerial.read();
      if (index < 511) {
      buffer[index++] = c;
    }
      lastReceive = millis(); // 마지막 수신 시간 갱신
    }

    if (millis() - start > 8000) {  // 최대 대기시간 8초
      break;
    }
  }

  Serial.println("== 응답 시작 ==");
  // Serial.println(response);
  buffer[index] = '\0'; // 문자열 종료
  Serial.println(buffer);
  Serial.println("== 응답 끝 ==");

  return buffer;
}