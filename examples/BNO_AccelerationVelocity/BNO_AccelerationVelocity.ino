#include <Wire.h>
#include <Adafruit_BNO08x.h>

Adafruit_BNO08x bno08x;

sh2_SensorValue_t sensorValue;

float velocityX = 0.0, velocityY = 0.0, velocityZ = 0.0;
unsigned long lastUpdate = 0;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  if (!bno08x.begin_I2C()) {
    Serial.println("Fehler beim Initialisieren des BNO085!");
    while (1);
  }

  // Aktiviert lineare Beschleunigung (ohne Gravitation)
  bno08x.enableReport(SH2_LINEAR_ACCELERATION, 5000); // 5ms = 200Hz

  lastUpdate = millis();
}

void loop() {
  if (bno08x.getSensorEvent(&sensorValue)) {
    if (sensorValue.sensorId == SH2_LINEAR_ACCELERATION) {
      float ax = sensorValue.un.linearAcceleration.x;
      float ay = sensorValue.un.linearAcceleration.y;
      float az = sensorValue.un.linearAcceleration.z;

      unsigned long currentTime = millis();
      float dt = (currentTime - lastUpdate) / 1000.0; // Zeit in Sekunden
      lastUpdate = currentTime;

      // Integration zur Geschwindigkeitsberechnung
      velocityX += ax * dt;
      velocityY += ay * dt;
      velocityZ += az * dt;

      // Betrag der Beschleunigung
      float accelerationMagnitude = sqrt(ax * ax + ay * ay + az * az);

      // Betrag der Geschwindigkeit
      float velocityMagnitude = sqrt(velocityX * velocityX +
                                     velocityY * velocityY +
                                     velocityZ * velocityZ);

      // Umrechnung in km/h
      float velocityKmh = velocityMagnitude * 3.6;

      Serial.print("Beschleunigung [m/s²]: ");
      Serial.println(accelerationMagnitude, 1);

      Serial.print("Geschwindigkeit [km/h]: ");
      Serial.println(velocityKmh, 1);


    }
  }
  delay(20);
}
