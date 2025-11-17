void setup() { 
  pinMode(1, INPUT);
  pinMode(2, INPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);

}

void loop() {
  unsigned long steering = pulseIn(1, HIGH, 25000);
  unsigned long throttle = pulseIn(2, HIGH, 30000);

  if (steering > 800 && steering < 2200) {
    digitalWrite(8, HIGH);
    delayMicroseconds(steering);
    digitalWrite(8, LOW);
    delayMicroseconds(20000 - steering);
  }

  
  if (throttle> 800 && throttle < 2200) {
    digitalWrite(8, HIGH);
    delayMicroseconds(throttle);
    digitalWrite(8, LOW);
    delayMicroseconds(20000 - throttle);
  }

}
