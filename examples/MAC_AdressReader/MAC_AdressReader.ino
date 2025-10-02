#include <ESP8266WiFi.h>

void setup() {
  Serial.begin(115200);
  delay(1000);

  // MAC-Adresse des ESP8266 auslesen
  String macAddress = WiFi.macAddress();
  Serial.println("MAC-Adresse des ESP8266:");
  Serial.println(macAddress);
}

void loop() {
  // Nichts im Loop nötig
}
