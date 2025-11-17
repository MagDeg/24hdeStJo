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

  radio.setPALevel(RF24_PA_HIGH);
  radio.setDataRate(RF24_1MBPS);
  radio.setChannel(90);
  radio.setAutoAck(true);
  radio.enableDynamicPayloads();

  radio.openReadingPipe(1, address);
  radio.startListening();
}

void loop() {
  while (radio.available()) {
    radio.read(&data, sizeof(data));
    Serial.print(F("v1=")); Serial.print(data.value1, 3);
    Serial.print(F(" v2=")); Serial.print(data.value2, 3);
    Serial.print(F(" cnt=")); Serial.println(data.counter);
  }
}
