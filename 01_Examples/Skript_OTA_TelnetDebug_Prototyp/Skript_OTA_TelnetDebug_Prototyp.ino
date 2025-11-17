#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
#include <WiFiClient.h>

// WLAN-Zugangsdaten
const char* ssid = "TeamGermanyMain";
const char* password = "24hStJoWLAN";

// Telnet-Server
WiFiServer telnetServer(23);
WiFiClient telnetClient;

void setup() {
  Serial.begin(115200);
  Serial.println("\nBooting...");

  // WLAN verbinden
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WLAN-Verbindung fehlgeschlagen! Neustart in 5 Sekunden...");
    delay(5000);
    ESP.restart();
  }

  Serial.print("Verbunden mit IP: ");
  Serial.println(WiFi.localIP());

  // OTA Setup
  ArduinoOTA.setHostname("esp8266-ota");
  ArduinoOTA.setPassword("1234");
  ArduinoOTA.onStart([]() {
    String type = (ArduinoOTA.getCommand() == U_FLASH) ? "Sketch" : "Filesystem";
    Serial.println("OTA Start: " + type);
    if (telnetClient && telnetClient.connected()) {
      telnetClient.println("OTA Update startet: " + type);
    }
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("OTA abgeschlossen.");
    if (telnetClient && telnetClient.connected()) {
      telnetClient.println("OTA Update abgeschlossen.");
    }
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    String msg = "OTA Fortschritt: " + String((progress * 100) / total) + "%";
    Serial.println(msg);
    if (telnetClient && telnetClient.connected()) {
      telnetClient.println(msg);
    }
  });
  ArduinoOTA.onError([](ota_error_t error) {
    String errMsg = "OTA Fehler: ";
    switch (error) {
      case OTA_AUTH_ERROR: errMsg += "Auth Fehler"; break;
      case OTA_BEGIN_ERROR: errMsg += "Begin Fehler"; break;
      case OTA_CONNECT_ERROR: errMsg += "Verbindungsfehler"; break;
      case OTA_RECEIVE_ERROR: errMsg += "Empfangsfehler"; break;
      case OTA_END_ERROR: errMsg += "End Fehler"; break;
    }
    Serial.println(errMsg);
    if (telnetClient && telnetClient.connected()) {
      telnetClient.println(errMsg);
    }
  });
  ArduinoOTA.begin();

  // Telnet-Server starten
  telnetServer.begin();
  telnetServer.setNoDelay(true);
  Serial.println("Telnet-Server gestartet (Port 23)");
}

void loop() {
  // OTA
  ArduinoOTA.handle();

  
  // Telnet-Client prüfen
  if (telnetServer.hasClient()) {
    if (!telnetClient || !telnetClient.connected()) {
      if (telnetClient) telnetClient.stop();
      telnetClient = telnetServer.available();
      Serial.println("Telnet-Client verbunden");
      telnetClient.println("Willkommen beim ESP8266 Telnet Debug!");
    } else {
      WiFiClient newClient = telnetServer.available();
      newClient.println("Telnet schon belegt!");
      newClient.stop();
    }
  }

  // Beispiel-Debug-Ausgabe
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 5000) {
    String debugMsg = "Debug-Ausgabe: millis = " + String(millis());
    Serial.println(debugMsg);
    if (telnetClient && telnetClient.connected()) {
      telnetClient.println(debugMsg);
    }
    lastPrint = millis();
  }


  // Optional: hier kommt dein eigener Code rein
}
