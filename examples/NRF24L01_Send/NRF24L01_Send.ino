#include <SPI.h>
#include "RF24.h"

#define CE_PIN 6
#define CSN_PIN 7
RF24 radio(CE_PIN, CSN_PIN);

uint8_t address[6] = "1Node";

struct TelemetryData {
  float value1;
  float value2;
  uint32_t counter;
} data;

void setup() {
  Serial.begin(115200);
  if (!radio.begin()) {
    Serial.println(F("NRF nicht gefunden!"));
    while (1);
  }

  radio.setPALevel(RF24_PA_HIGH);   // starke, aber sichere Sendeleistung
  radio.setDataRate(RF24_1MBPS);    // stabiler als 2 Mbps
  radio.setChannel(90);             // außerhalb üblicher WLAN-Kanäle
  radio.setAutoAck(true);           // Bestätigungen an
  radio.enableDynamicPayloads();    // nur nötige Bytes senden
  radio.setRetries(3, 15);          // 3 Versuche, max. Wartezeit

  radio.openWritingPipe(address);
  radio.stopListening();
}

void loop() {
  static uint32_t counter = 0;

  // Beispiel-Telemetriedaten
  data.value1 = analogRead(A0) / 1023.0;
  data.value2 = analogRead(A1) / 1023.0;
  data.counter = counter++;

  if (radio.write(&data, sizeof(data))) {
    Serial.println(F("OK gesendet"));
  } else {
    Serial.println(F("Fehler beim Senden"));
  }
}
