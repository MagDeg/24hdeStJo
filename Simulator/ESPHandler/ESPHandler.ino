void setup() {
  Serial.begin(115200);
  randomSeed(analogRead(0));  // optional, besseres random
}

void loop() {
  // float-Werte zwischen 0.0 und 1.0
  float steering = (float)analogRead(1) /4095; // z.B. 0.000 bis 1.000
  float velocity = (float)analogRead(2)/4095;
  float stop = 0.0;//random(0, 1001) / 1000.0;

  Serial.print(steering, 3);  // 3 Nachkommastellen
  Serial.print(",");
  Serial.print(velocity, 3); 
  Serial.print(",");
  Serial.print(stop, 3);
  Serial.println();

  delay(100);  // alle 100 ms senden
}