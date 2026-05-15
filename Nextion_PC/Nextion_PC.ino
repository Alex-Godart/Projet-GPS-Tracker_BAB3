#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// Pins pour le module GPS
static const int RXPin = 3, TXPin = 4;
static const uint32_t GPSBaud = 9600; // Vitesse par défaut de la plupart des modules GPS
float lati;
long Lati;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);
 
void setup() {
  Serial.begin(9600); // Communication série vers le PC
  ss.begin(GPSBaud); // Communication série vers le module GPS
  Serial.println("state.txt=\"Recherche gps\"");
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);
  delay(5000);
}

void loop() {
    Serial.print("t0.txt=\"debut\"");
    Serial.write(0xFF);
    Serial.write(0xFF);
    Serial.write(0xFF);
  // Lire les données du GPS
  while (ss.available() > 0) { 
    if (gps.encode(ss.read())) { //  Lit les trames du gps et les convertit dans un autre format
      displayInfo();
    }
  }

  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.print("state.txt=\"GPS non détecté: vérifiez le câblage.\"");
    Serial.write(0xFF);
    Serial.write(0xFF);
    Serial.write(0xFF);
    while(true);
  }
}


void displayInfo() {
  if (gps.location.isValid()) {
    Serial.print("state.txt=\"GPS trouve.\"");
    Serial.write(0xFF);
    Serial.write(0xFF);
    Serial.write(0xFF);
    lati=gps.location.lat();
    Lati = round(lati*1000);//
    Serial.print("lati.val=");
    Serial.print(Lati);
    Serial.write(0xFF);
    Serial.write(0xFF);
    Serial.write(0xFF);
    Serial.print("n0.val=");
    Serial.print(Lati);
    Serial.write(0xFF);
    Serial.write(0xFF);
    Serial.write(0xFF);
  } else {
    Serial.print("state.txt=\"Localisation invalide\"");
    Serial.write(0xFF);
    Serial.write(0xFF);
    Serial.write(0xFF);
  }
  Serial.print("t0.txt=\"fin\"");
    Serial.write(0xFF);
    Serial.write(0xFF);
    Serial.write(0xFF);
  delay(1000);
}
