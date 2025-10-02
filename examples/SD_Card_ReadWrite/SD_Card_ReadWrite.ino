#include <SPI.h>
#include <SD.h>  // In diesem Sketch verwenden wir die  library

File Textdatei;  // An dieser Stelle wird die Variable „Textdatei“ als File (dts. Datei) deklariert.

void setup() {
  Serial.begin(9600);
  Serial.println("Initialisiere SD-Karte");
  if (!SD.begin(5)) {  // Wenn die SD-Karte nicht (!SD.begin) gefunden werden kann, …
    Serial.println("Initialisierung fehlgeschlagen!");  // … soll eine Fehlermeldung ausgegeben werden. ….
    return;
  }
  Serial.println("Initialisierung abgeschlossen");  // … Ansonsten soll die Meldung „Initialisierung abgeschlossen.“ ausgegeben werden.
  Textdatei = SD.open("test.txt", FILE_WRITE);  // An dieser Stelle wird die Textdatei erstellt. Unsere Textdatei soll „test“ heißen und im Format „.txt“ (Text) erstellt werden.

  if (Textdatei) {  // Wenn die Textdatei („test.txt“) gefunden wurde….
    Serial.println("Schreibe in Textdatei…");  // … soll eine Meldung im seriellen Monitor erscheinen…
    Textdatei.println("Funduino GmbH");  // … und die Textdatei anschließend befüllt werden.
    Textdatei.println("1, 2, 3, 4, 5");
    Textdatei.println("a, b, c, d, e");
    Textdatei.println();
    Textdatei.close();  // Anschließend wird die Textdatei wieder geschlossen…
    Serial.println("Abgeschlossen.");  // … und eine erneute Meldung im seriellen Monitor ausgegeben.
    Serial.println();
  } else {  // Wenn !keine! Textdatei gefunden werden kann …
    Serial.println("Textdatei konnte nicht ausgelesen werden");  // … erscheint eine Fehlermeldung im seriellen Monitor.
  }

  // NUN WIRD DIE TEXTDATEI AUSGELESEN
  Textdatei = SD.open("test.txt");  // Die Textdatei auf der SD-Karte wird wieder geoeffnet…
  if (Textdatei) {
    Serial.println("test.txt:");  // … und der Name der Datei wird ausgegeben.
    while (Textdatei.available())  // Anschließend wird die Datei so lange ausgelesen (while)…
    {
      Serial.write(Textdatei.read());  // … bis keine Daten mehr gefunden werden können.
    }
    Textdatei.close();  // Im Anschluss wird die Textdatei wieder geschlossen.
  } else  // Sollte keine Textdatei (also test.txt) gefunden werden können…
  {
    Serial.println("Textdatei konnte nicht geoeffnet werden");  // … erscheint eine Fehlermeldung im seriellen Monitor.
  }
}

void loop() {
}
