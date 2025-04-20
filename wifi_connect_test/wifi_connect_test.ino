#include <SoftwareSerial.h>
SoftwareSerial espSerial(2, 3); // RX, TX

void setup() {
  Serial.begin(9600);
  espSerial.begin(9600);
  delay(2000);

  espSerial.println("AT");
  delay(1000);

  while (espSerial.available()) {
    char c = espSerial.read();
    Serial.write(c);  // 시리얼 모니터에 출력
  }
}

void loop() {}