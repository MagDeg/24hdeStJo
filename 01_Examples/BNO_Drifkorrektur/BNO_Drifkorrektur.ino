#include <Wire.h>
#include <Adafruit_BNO08x.h>

Adafruit_BNO08x bno = Adafruit_BNO08x();

const int servoPin = 13;
float correctedYaw = 0.0;
const float driftFactor = 0.1; //je ggrößer, desto schneller regiert servo

void setup() {
  Serial.begin(115200);
  Wire.begin();
  pinMode(servoPin, OUTPUT);

  if (!bno.begin_I2C()) {
    Serial.println("BNO085 konnte nicht initialisiert werden!");
    while (1);
  }

  if (!bno.enableReport(SH2_ARVR_STABILIZED_RV)) {
    Serial.println("Rotation Vector konnte nicht aktiviert werden!");
    while (1);
  }

  Serial.println("Setup fertig!");
}

void loop() {
  sh2_SensorValue_t sensorValue;

  if (bno.getSensorEvent(&sensorValue)) {
    if (sensorValue.sensorId == SH2_ARVR_STABILIZED_RV) {
      sh2_RotationVectorWAcc_t* rv = &sensorValue.un.arvrStabilizedRV;
      float qw = rv->real;
      float qx = rv->i;
      float qy = rv->j;
      float qz = rv->k;

      float yaw = atan2(2.0 * (qx * qy + qz * qw), 1.0 - 2.0 * (qy * qy + qz * qz)) * 180.0 / PI;

      correctedYaw += (yaw - correctedYaw) * driftFactor;

      int angle = constrain(correctedYaw + 90, 0, 180);
      int pulseWidth = map(angle, 0, 180, 1000, 2000);

      digitalWrite(servoPin, HIGH);
      delayMicroseconds(pulseWidth);
      digitalWrite(servoPin, LOW);
      delayMicroseconds(20000 - pulseWidth);

      Serial.print("Yaw: "); Serial.print(yaw);
      Serial.print(" | Korrigiert: "); Serial.println(correctedYaw);
    }
  }
}
