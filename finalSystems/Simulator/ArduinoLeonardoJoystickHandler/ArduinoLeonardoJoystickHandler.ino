#include <HID-Project.h>
#include <HID-Settings.h>

// Pins für PWM-Kanäle
#define CH1_PIN 3   // Lenkung
#define CH2_PIN 2   // Gas
#define CH3_PIN 4   // Schalter

// PWM-Bereiche in Mikrosekunden
#define PWM_MIN 1000
#define PWM_MID 1500
#define PWM_MAX 2000

void setup() {
  Serial.begin(9600);
  pinMode(CH1_PIN, INPUT);
  pinMode(CH2_PIN, INPUT);
  pinMode(CH3_PIN, INPUT);

  Gamepad.begin();
  delay(1000);   // USB initialisieren
}

// Hilfsfunktion: float-Mapping wie in deinem Original-Skript
float mapFloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void loop() {
  // Pulsbreiten messen
  unsigned long ch1Pulse = pulseIn(CH1_PIN, HIGH, 25000);
  unsigned long ch2Pulse = pulseIn(CH2_PIN, HIGH, 30000);
  unsigned long ch3Pulse = pulseIn(CH3_PIN, HIGH, 25000);

  Serial.print(ch1Pulse);
  Serial.print(" ");
  Serial.print(ch2Pulse);
  Serial.print(" ");
  Serial.println(ch3Pulse);
  

  // PWM-Werte begrenzen
  ch1Pulse = constrain(ch1Pulse, PWM_MIN, PWM_MAX);
  ch2Pulse = constrain(ch2Pulse, PWM_MIN, PWM_MAX);


  // Mapping auf HID-Achsen (-32767 ... 32767)
  int16_t steering = mapFloat(ch1Pulse, PWM_MIN, PWM_MAX, -32767.0, 32767.0);
  int16_t throttle = mapFloat(ch2Pulse, PWM_MIN, PWM_MAX, -32767.0, 32767.0);


    // Schalter → Button 1
  bool button1 = ((ch3Pulse > 2000) || (ch3Pulse < 1000));



  // Gamepad senden
  Gamepad.xAxis(steering);
  Gamepad.yAxis(throttle);
  if (button1) Gamepad.press(1); else Gamepad.release(1);
  Gamepad.write();

  delay(20); // 50 Hz Refresh
}
