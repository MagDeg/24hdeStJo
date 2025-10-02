// Pins für CH1 (Lenkung) und CH2 (Gas)
#define CH1_PIN 2   // GPIO4 an Empfänger CH1
#define CH2_PIN 1   // GPIO5 an Empfänger CH2

// Wertebereich PWM (µs)
#define PWM_MIN 1000
#define PWM_MID 1500
#define PWM_MAX 2000

void setup() {
  Serial.begin(115200);
  pinMode(CH1_PIN, INPUT);
  pinMode(CH2_PIN, INPUT);
}

void loop() {
  // Pulsbreiten messen
  unsigned long ch1Pulse = pulseIn(CH1_PIN, HIGH, 25000); // µs
  unsigned long ch2Pulse = pulseIn(CH2_PIN, HIGH, 25000);

  // Normierung auf -100 ... +100 %
  float steering = mapFloat(ch1Pulse, PWM_MIN, PWM_MAX, 0.0, 1.0);
  float throttle = mapFloat(ch2Pulse, PWM_MIN, PWM_MAX, 0.0, 1.0);

  Serial.print(steering, 3);
  Serial.print(",");
  Serial.print(throttle, 3);
  Serial.println();

  delay(20); // 50 Hz Refresh
}

// Hilfsfunktion für float-Mapping
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
