#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// Pins pour le module GPS
static const int RXPin = 3, TXPin = 4;
static const uint32_t GPSBaud = 9600; // Vitesse par défaut de la plupart des modules GPS
//const int RXPinscreen = 11, TXPinscreen = 12; 

long Lati;
long Longi;
long Alti;
long Vit;

TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);
//SoftwareSerial nextSerial(RXPinscreen, TXPinscreen);  // serial to nextion screen
 
void setup() {
  Serial.begin(9600); // Communication série vers le PC
  ss.begin(GPSBaud); // Communication série vers le module GPS
  Serial.print("state.txt=\"Recherche de satellites\"");
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);
  delay(5000);
}

void loop() {
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
    Serial.print("state.txt=\"GPS trouve\"");
    Serial.write(0xFF);
    Serial.write(0xFF);
    Serial.write(0xFF);

    
    Lati=conversion(gps.location.lat());
    Serial.print("lati.val=");
    Serial.print(Lati);
    Serial.write(0xFF);
    Serial.write(0xFF);
    Serial.write(0xFF);
    
    Longi=conversion(gps.location.lng());
    Serial.print("longi.val=");
    Serial.print(Longi);
    Serial.write(0xFF);
    Serial.write(0xFF);
    Serial.write(0xFF);
    
    
    Alti=conversion(gps.altitude.meters());
    Serial.print("alti.val=");
    Serial.print(Alti);
    Serial.write(0xFF);
    Serial.write(0xFF);
    Serial.write(0xFF);

    
    Vit=conversion(gps.speed.kmph());
    Serial.print("vit.val=");
    Serial.print(Vit);
    Serial.write(0xFF);
    Serial.write(0xFF);
    Serial.write(0xFF);
   

  } else {
    Serial.print("state.txt=\"Localisation invalide\"");
    Serial.write(0xFF);
    Serial.write(0xFF);
    Serial.write(0xFF);
    
  }
  delay(1000);
}
long conversion(float var){       //Le float de nextion ne peut prendre que des valeurs int ou long, une conversion est donc nécessaire
  return round(var*1000);  
  }
