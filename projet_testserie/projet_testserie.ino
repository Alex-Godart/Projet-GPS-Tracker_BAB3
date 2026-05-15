#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// Pins pour le module GPS
static const int RXPin = 3, TXPin = 4;
static const uint32_t GPSBaud = 9600; // Vitesse par défaut de la plupart des modules GPS
const int RXPinscreen = 11, TXPinscreen = 12; 

long Lati;
long Longi;
long Alti;
long Vit;

TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);
SoftwareSerial nextSerial(RXPinscreen, TXPinscreen);  // serial to nextion screen
 
void setup() {
  Serial.begin(9600);
  nextSerial.begin(9600); // Communication série vers le PC
  ss.begin(GPSBaud); // Communication série vers le module GPS
  nextSerial.print("state.txt=\"Recherche de satellites\"");
  nextSerial.write(0xFF);
  nextSerial.write(0xFF);
  nextSerial.write(0xFF);
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
    nextSerial.print("state.txt=\"GPS non détecté: vérifiez le câblage.\"");
    nextSerial.write(0xFF);
    nextSerial.write(0xFF);
    nextSerial.write(0xFF);
    while(true);
  }
}

void displayInfo() {
  if (gps.location.isValid()) {
    nextSerial.print("state.txt=\"GPS trouve\"");
    nextSerial.write(0xFF);
    nextSerial.write(0xFF);
    nextSerial.write(0xFF);

    
    Lati=conversion(gps.location.lat());
    nextSerial.print("lati.val=");
    nextSerial.print(Lati);
    nextSerial.write(0xFF);
    nextSerial.write(0xFF);
    nextSerial.write(0xFF);
    
    Longi=conversion(gps.location.lng());
    nextSerial.print("longi.val=");
    nextSerial.print(Longi);
    nextSerial.write(0xFF);
    nextSerial.write(0xFF);
    nextSerial.write(0xFF);
    
    
    Alti=conversion(gps.altitude.meters());
    nextSerial.print("alti.val=");
    nextSerial.print(Alti);
    nextSerial.write(0xFF);
    nextSerial.write(0xFF);
    nextSerial.write(0xFF);

    
    Vit=conversion(gps.speed.kmph());
    nextSerial.print("vit.val=");
    nextSerial.print(Vit);
    nextSerial.write(0xFF);
    nextSerial.write(0xFF);
    nextSerial.write(0xFF);
   

  } else {
    nextSerial.print("state.txt=\"Localisation invalide\"");
    nextSerial.write(0xFF);
    nextSerial.write(0xFF);
    nextSerial.write(0xFF);
    
  }
  delay(1000);
}
long conversion(float var){       //Le float de nextion ne peut prendre que des valeurs int ou long, une conversion est donc nécessaire
  return round(var*1000);  
  }
