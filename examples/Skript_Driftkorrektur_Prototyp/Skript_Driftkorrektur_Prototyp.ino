#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Servo.h>

Servo Servomotor;
Adafruit_MPU6050 mpu;

unsigned long lastTime = 0;
float gyroZOffset = 0.0;
bool calibrated = false;

const int calibSamples = 500;

// Kalman-Filter Klasse (wie vorher)
class KalmanFilter {
public:
  float Q_angle = 0.001;
  float Q_bias = 0.003;
  float R_measure = 0.03;

  float angle = 0;
  float bias = 0;
  float rate = 0;

  float P[2][2] = {{0,0},{0,0}};

  float update(float newRate, float dt) {
    rate = newRate - bias;
    angle += dt * rate;

    P[0][0] += dt * (dt*P[1][1] - P[0][1] - P[1][0] + Q_angle);
    P[0][1] -= dt * P[1][1];
    P[1][0] -= dt * P[1][1];
    P[1][1] += Q_bias * dt;

    float S = P[0][0] + R_measure;
    float K[2];
    K[0] = P[0][0] / S;
    K[1] = P[1][0] / S;

    float y = 0 - angle; // Innovation (keine echte Messung)
    angle += K[0] * y;
    bias += K[1] * y;

    float P00_temp = P[0][0];
    float P01_temp = P[0][1];

    P[0][0] -= K[0] * P00_temp;
    P[0][1] -= K[0] * P01_temp;
    P[1][0] -= K[1] * P00_temp;
    P[1][1] -= K[1] * P01_temp;

    return angle;
  }
};

KalmanFilter kalman;

void calibrateGyroZ() {
  Serial.println("Kalibriere Gyroskop-Z-Achse...");
  float sum = 0;
  for (int i = 0; i < calibSamples; i++) {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    sum += g.gyro.z;
    delay(5);
  }
  gyroZOffset = sum / calibSamples;
  Serial.print("Offset Z: ");
  Serial.println(gyroZOffset, 6);
  calibrated = true;
}

void setup() {
  Servomotor.attach(D5);
  Servomotor.write(90); // Neutralposition
  Serial.begin(9600);
  Serial.println("Starte MPU6050...");

  if (!mpu.begin()) {
    Serial.println("MPU6050 nicht gefunden. Checke Verkabelung!");

  }

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  delay(1000);
  calibrateGyroZ();
  lastTime = millis();
}

void loop() {
  if (!calibrated) return;

  sensors_event_t accel, gyro, temp;
  mpu.getEvent(&accel, &gyro, &temp);

  unsigned long now = millis();
  float dt = (now - lastTime) / 1000.0;
  lastTime = now;

  float gyroZ = gyro.gyro.z - gyroZOffset; // rad/s

  // Kalman-Filter
  float filteredYaw = kalman.update(gyroZ, dt);

  // Umrechnung in Grad
  float yawDeg = filteredYaw * 180.0 / PI;

  // Servo steuern: Servo soll den Drift ausgleichen, also gegenlenken
  // Neutral = 90°, max ±30°
  float servoAngle = 90 - yawDeg; // gegenlenken
  servoAngle = constrain(servoAngle, 60, 120);

  Serial.print("Yaw (Kalman): ");
  Serial.print(yawDeg, 2);
  Serial.print(" | Servo Winkel: ");
  Serial.println(servoAngle, 1);

  Servomotor.write(servoAngle);

  delay(20);
}
