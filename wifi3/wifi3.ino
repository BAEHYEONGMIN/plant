#include <SoftwareSerial.h>
#include <ArduinoJson.h>
const String ssid   = "KT_GiGA_3A57"; 
const String password = "8aek04zh16";
#define RX 2
#define TX 3
#define LED_PIN	7
SoftwareSerial espSerial(RX, TX); // RX, TX
//함수 선언부분
void wait();
String sendAT(String cmd, unsigned long timeout = 5000);
String sendRequest(String request);
bool waitForWifi(unsigned long timeout = 15000);
String getjson(String response);

void setup() {
  Serial.begin(9600);          // PC와 연결
  espSerial.begin(9600);       // ESP-01과 연결
  pinMode(LED_PIN, OUTPUT);
  String result = "";
  delay(2000);
  sendAT("AT");                // 모듈 응답 확인
  sendAT("AT+CWMODE=1");       // WiFi Station 모드
  
  String connect_query = "AT+CWJAP=\""+ssid+"\",\""+password+"\"";
  bool wifi = false;
  int wifi_fail_cnt = 0;
  while(wifi==false){
    if (wifi_fail_cnt==5){
      return -1;
    }
    sendAT(connect_query);  // WiFi 연결
    wifi = waitForWifi();
    wifi_fail_cnt+=1;
  }
  if (wifi){
  delay(200);
  result = sendAT("AT+CWJAP?"); // 현재 연결된 WiFi 정보 출력
  Serial.println(result);
  }
  // Glitch 서버로 연결
  String pingRequest = 
      "GET /ping HTTP/1.1\r\n"
      "Host: baemin-simple-server.glitch.me\r\n"
      "User-Agent: ESP8266\r\n"
      "Connection: close\r\n\r\n";
  sendRequest(pingRequest);
  delay(2000);
  
}

void loop() {//주기가 대략 17초.
  String request = 
      "GET /get-command HTTP/1.1\r\n"
      "Host: baemin-simple-server.glitch.me\r\n"
      "User-Agent: ESP8266\r\n"
      "Connection: close\r\n\r\n";
  String result = sendRequest(request);
  Serial.println(result);
  if (result=="led_on"){digitalWrite(LED_PIN,HIGH);}
  if (result=="led_off"){digitalWrite(LED_PIN,LOW);}
  delay(4000);

}
String sendAT(String cmd, unsigned long timeout = 5000) {
  while (espSerial.available()) espSerial.read(); // 버퍼 정리
  espSerial.println(cmd);
  Serial.println(">>>>>>>> " + cmd);
  String response = "";
  unsigned long start = millis();
  while (millis() - start < timeout) {
    while (espSerial.available()) {
      char c = espSerial.read();
      response += c;
      Serial.write(c);
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
  
  while (espSerial.available()) espSerial.read(); // 버퍼 정리
  Serial.println("[!] WiFi 연결 실패 또는 타임아웃");
  return false;
}

String sendRequest(String request) {
  Serial.println("==============");
  Serial.println("request start : ");
  Serial.print(request);
  Serial.println("=============");


  char buffer[512];
  int index = 0;
  
  // sendAT("AT+CIPCLOSE");
  // delay(1000); // 최소 1초 대기
  while (espSerial.available()) espSerial.read(); // 버퍼 정리
  delay(200);
  sendAT("AT+CIPSTART=\"TCP\",\"baemin-simple-server.glitch.me\",80");
  delay(1000);
  // 요청 전송 준비
  espSerial.println("AT+CIPSEND=" + String(request.length()));
  delay(200);
  wait();  // '>' 프롬프트 대기
  delay(200);
  espSerial.print(request);
  // 응답 수신
  unsigned long start = millis();
  unsigned long lastReceive = millis();
  bool save_start = false;
  while (millis() - lastReceive < 5000) {
    if (espSerial.available()) {
      char c = espSerial.read();
      if (c=='}'){save_start=false;}
      if (save_start){
        if (index < 511) {
        buffer[index++] = c;
      }
    }
    if (c=='{'){save_start=true;}
    lastReceive = millis(); // 마지막 수신 시간 갱신
    }
  }
  delay(200);
  while (espSerial.available()) {
    char c = espSerial.read();
    if (index < 511) {
      buffer[index++] = c;
    }
  }
  Serial.println("== 응답 시작 ==");
  buffer[index] = '\0'; // 문자열 종료
  Serial.println(buffer);
  Serial.println("== 응답 끝 ==");
  String response = "";
  for (int i = 0; i < index; i++) {
    response += buffer[i]; 
  }
  return response;
}

String get_json(String response) {
  int jsonStart = response.indexOf('{');
  int jsonEnd = response.lastIndexOf('}');
  
  if (jsonStart != -1 && jsonEnd != -1 && jsonEnd > jsonStart) {
    String jsonPart = response.substring(jsonStart, jsonEnd + 1);
    Serial.println("JSON 응답: " + jsonPart);
    return jsonPart;
  } else {
    Serial.println("[!] JSON 응답을 찾을 수 없음");
    return "error";
  }
}
