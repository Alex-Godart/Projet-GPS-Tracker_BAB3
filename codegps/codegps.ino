#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// Pins pour le module GPS
static const int RXPin = 12, TXPin = 13;
static const uint32_t GPSBaud = 9600; // Vitesse par défaut de la plupart des modules GPS
const int RXPinscreen = 2, TXPinscreen = 3; float lati;
float longi;
float alti;
float vit;
float txt;

TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);
SoftwareSerial nextSerial(RXPinscreen, TXPinscreen);  // serial to nextion screen
 
void setup() {
  Serial.begin(9600); // Communication série vers le PC
  ss.begin(GPSBaud); // Communication série vers le module GPS
  nextSerial.begin(9600);
  Serial.println("Recherche de satellites...");
}

void loop() {
  // Lire les données du GPS
  ss.listen();// l'arduino écoute le gps et ignore le nextion
  while (ss.available() > 0) { //
    if (gps.encode(ss.read())) { //  Lit les trames du gps et les convertit dans un autre format
      displayInfo();
    }
  }

  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println("GPS non détecté : vérifiez le câblage.");
    while(true);
  }
}
void sendToNextion(String cmd) {
  nextSerial.print(cmd);
  nextSerial.write(0xFF);
  nextSerial.write(0xFF);
  nextSerial.write(0xFF);
} 

void displayInfo() {
  if (gps.location.isValid()) {
    Serial.print("Latitude: ");
    Serial.println(gps.location.lat(), 6);// 6 est nombre de decimale
    lati=gps.location.lat();
    Serial.print("Longitude: ");
    Serial.println(gps.location.lng(), 6);
    longi=gps.location.lng();
    Serial.print("Altitude: ");
    Serial.println(gps.altitude.meters());
    alti=gps.altitude.meters();
    if (gps.speed.kmph()> 2.5){
    Serial.print(F(" Speed: "));
    Serial.print(gps.speed.kmph());
    Serial.println(F(" km/h"));
    }
    vit=gps.speed.kmph();
    nextSerial.listen();
    
    String texte = "t3.txt=\"";
    texte += String(vit);
    texte += " km/h\"";
    sendToNextion(texte);
    //Serial.print(texte); Problème, je pense que c'est du au nombre de chiffre trop imporant pour pouvoir afficher sur les petites cases de texte sur le nextion ou peut etre pas au final
    texte = "t0.txt=\""; 
    texte += String(lati);
    texte += "\"";
    sendToNextion(texte);
    
    texte = "t1.txt=\"";
    texte += String(longi);
    texte += "\"";
    
    sendToNextion(texte);
    
    texte = "t2.txt=\"";
    texte += String(alti);
    texte += "\"";
    
    sendToNextion(texte);
  } else {
    Serial.println("Localisation invalide");
  }
  delay(1000);
}
