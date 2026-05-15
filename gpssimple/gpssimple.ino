#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// Broches RX, TX pour le GPS
static const int RXPin = 3, TXPin = 4;
static const uint32_t GPSBaud = 9600; // Baudrate du module NEO-6M

TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

void setup() {
  Serial.begin(9600);
  ss.begin(GPSBaud);
  Serial.println("Recherche de signal GPS...");
}

void loop() {
  // Lire les données du GPS
  while (ss.available() > 0) {
    if (gps.encode(ss.read())) {
      displayInfo();
    }
  }

  // Si aucune donnée reçue après 5 secondes
  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println("Erreur: Signal GPS non trouve");
    while(true);
  }
}

void displayInfo() {
  if (gps.location.isValid()) {
    Serial.print("Latitude: ");
    Serial.println(gps.location.lat(),6);
    Serial.print("Longitude: ");
    Serial.println(gps.location.lng(), 6);
  } else {
    Serial.println("Position non valide (fix en cours...)");
  }
}
