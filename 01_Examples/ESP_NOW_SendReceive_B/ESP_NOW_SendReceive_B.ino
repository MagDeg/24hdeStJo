#include <ESP8266WiFi.h>
#include <espnow.h>

// Struktur für die zu sendenden/empfangenen Daten
typedef struct struct_message {
  int id;
  float temperatur;
} struct_message;

struct_message myData;

// MAC-Adresse des Partners (anpassen!)
uint8_t peerAddress[] = {0x48, 0x3F, 0xDA, 0x74, 0x89, 0xB5}; // MAC des anderen ESP

// Callback: Wenn Daten empfangen wurden
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Empfangen -> ID: ");
  Serial.print(myData.id);
  Serial.print(" | Temperatur: ");
  Serial.println(myData.temperatur);
}

// Callback: Wenn Daten gesendet wurden
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Sendestatus: ");
  Serial.println(sendStatus == 0 ? "Erfolgreich" : "Fehlgeschlagen");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  wifi_set_channel(1);

  if (esp_now_init() != 0) {
    Serial.println("Fehler beim Initialisieren von ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_COMBO); // Senden & Empfangen
  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent);

  // Peer hinzufügen
  if (esp_now_add_peer(peerAddress, ESP_NOW_ROLE_COMBO, 1, NULL, 0) != 0) {
    Serial.println("Fehler beim Hinzufügen des Peers");
    return;
  }
}

void loop() {
  // Beispiel-Daten senden
  myData.id = random(1, 100);
  myData.temperatur = random(200, 300) / 10.0;

  esp_now_send(peerAddress, (uint8_t *)&myData, sizeof(myData));
  delay(3000); //sollte am ende 10-20ms (=50-100 Hz)
}
