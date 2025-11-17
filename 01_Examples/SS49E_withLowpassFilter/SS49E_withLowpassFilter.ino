
#define BUFFER_SIZE 5

const int AnalogPin = A1;


unsigned long timeLastTrigger = 0;
unsigned long timeSinceLastTrigger = 0;


bool triggered = false;

float buffer[BUFFER_SIZE] = { 0 };

float rawValue = 0.0;
float rounds = 0.0;
int bufferIndex = 0;
int bufferCount = 0;

float slopeThreshold = 5.0;
float alpha = 0.2; 
float filteredValue = 0.0;      // Gefilterter Sensorwert
float lastFilteredValue = 0.0;  // Letzter Wert für Flankenerkennung


void addToBuffer(float value) {
  buffer[bufferIndex] = value;
  bufferIndex = (bufferIndex + 1) % BUFFER_SIZE;
  if (bufferCount < BUFFER_SIZE) bufferCount++;
}

float getBufferAverage() {
  float sum = 0;
  for (int i = 0; i < bufferCount; i++) {
    sum += buffer[i];
  }
  return (bufferCount > 0) ? sum / bufferCount : 0;
}

void setup() {
  pinMode(AnalogPin, INPUT);
  Serial.begin(9600);  // Set comm speed for debug window messages

  rawValue = analogRead(AnalogPin);
  filteredValue = rawValue;
  lastFilteredValue = filteredValue;
}


void loop() {
  rawValue = analogRead(AnalogPin);

  filteredValue = alpha * rawValue + (1 - alpha) * filteredValue;

  // Flankenerkennung: von unterhalb Schwelle auf darüber
  float delta = filteredValue - lastFilteredValue;

  if ((delta > slopeThreshold) && !triggered) {
    if (timeLastTrigger != 0) {
      timeSinceLastTrigger = micros() - timeLastTrigger;

      if (timeSinceLastTrigger > 0) {
        // RPM berechnen: 4 Magnete pro Umdrehung
        float rpm = 60.0 * 1000000.0 / (4.0 * timeSinceLastTrigger);
        addToBuffer(rpm);
      }
    }
    
    timeLastTrigger = micros();
    triggered = true;
  }


  // Trigger zurücksetzen, wenn Wert wieder unter Schwelle fällt
  if (delta < slopeThreshold) {
    triggered = false;
  }

  lastFilteredValue = filteredValue;



  Serial.println(getBufferAverage());

  /*
  if (rawValue > 530 && !triggered) {
    rounds += 1.0;
    triggered = true;
  } 
  if (rawValue < 520) {
    triggered = false;
  }
  Serial.print("  ");
  Serial.println(rounds/4);
  */

  delay(5);
}