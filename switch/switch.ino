#define LED_PIN	7
#define SWITCH 8
#define PUMP 10
#define RX 2
#define TX 3

int readValue = -1;
bool switch_on = false;
#include <SoftwareSerial.h>
 
const String ssid   = "KT_GiGA_3A57"; 
const String password = "8aek04zh16";
SoftwareSerial espSerial(RX, TX); // RX, TX

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_PIN, OUTPUT);
  pinMode(SWITCH, INPUT_PULLUP);
  pinMode(PUMP,OUTPUT);
  Serial.begin(9600);
  espSerial.begin(9600);       // ESP-01과 연결
  digitalWrite(LED_PIN,LOW);
  digitalWrite(PUMP,HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  int temp = !digitalRead(SWITCH);
  // Serial.println(readValue);
  if (temp!=readValue){
    readValue=temp;
    if (temp==1){
    switch_on = !switch_on;
    }
    Serial.println(readValue);
    Serial.println(switch_on);
    if(switch_on == true){
      digitalWrite(LED_PIN,HIGH);
      // digitalWrite(PUMP,LOW);
    }
    else{
      digitalWrite(LED_PIN,LOW);
      // digitalWrite(PUMP,HIGH);
    }
  }
  delay(100);
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
      if (response.indexOf("OK") != -1 || response.indexOf("ERROR") != -1 || response.indexOf("FAIL") != -1) {
        delay(1000);
        return response;
      }
    }
  }
}
