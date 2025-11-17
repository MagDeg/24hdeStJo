#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;

void setup() {
  Serial.begin(115200);
  Wire.begin(D2, D1); // SDA = D2, SCL = D1 bei ESP8266 (NodeMCU)

  Serial.println("Scanning I2C bus...");
  
  for (byte address = 1; address < 127; ++address) {
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0) {
      Serial.print("Found I2C device at 0x");
      Serial.println(address, HEX);
    }
  }


  if (!ina219.begin()) {
    Serial.println("INA219 nicht gefunden. Prüfe die Verkabelung!");

  }

  Serial.println("INA219 bereit.");
}

void loop() {
  Serial.println("\033[31m[ERROR] \033[0m");
  float busVoltage = ina219.getBusVoltage_V();       // Spannung in Volt
  float shuntVoltage = ina219.getShuntVoltage_mV();  // Shunt-Spannung in mV
  float current_mA = ina219.getCurrent_mA();         // Strom in mA
  float power_mW = ina219.getPower_mW();            // Leistung in mW
  float loadVoltage = busVoltage + (shuntVoltage / 1000); // Gesamtspannung über Last

  Serial.print("Spannung: "); Serial.print(loadVoltage); Serial.println(" V");
  Serial.print("Strom: "); Serial.print(current_mA); Serial.println(" mA");
  Serial.print("Leistung: "); Serial.print(power_mW); Serial.println(" mW");
  Serial.println("----------------------");

  delay(1000); // 1 Sekunde warten
}
