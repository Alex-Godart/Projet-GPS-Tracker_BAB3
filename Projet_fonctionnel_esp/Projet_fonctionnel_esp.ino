#include <SoftwareSerial.h>
#include <TinyGPS++.h>

// Pins pour le module GPS
static const int RXPin = 4, TXPin = 5;
static const uint32_t GPSBaud = 9600; // Vitesse par défaut de la plupart des modules GPS
long lati;
long longi;
long alti;
long vit;
long Lati;
long Longi;
long distotal_int;
String texte="Position invalide";
int n=0;
float dist=0;
double distotal=0;
TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

 
void setup() {
  Serial.begin(9600); // Communication série vers le PC/nextion
  ss.begin(GPSBaud); // Communication série vers le module GPS
  Serial.print("state.txt=\"Recherche de satellites\"");
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);
  delay(5000);
}

void loop() {
  // Lire les données du GPS
  while (ss.available() > 0) { //
    if (gps.encode(ss.read())) { //  Lit les trames du gps et les convertit dans un autre format
      displayInfo();
      yield();
    }
  }

  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.print("state.txt=\"GPS non détecté: vérifiez le câblage.\"");
    Serial.write(0xFF);
    Serial.write(0xFF);
    Serial.write(0xFF);
    while(true){
      yield();    //Sert à limiter le reboot de l'esp qui pense etre bloqué
    }
  }
}

void displayInfo() {
  if (gps.location.isValid()) {
    Serial.print("state.txt=\"GPS trouve\"");
    Serial.write(0xFF);
    Serial.write(0xFF);
    Serial.write(0xFF);
    lati=round(1000*gps.location.lat());
    longi=round(1000*gps.location.lng());
    alti=round(1000*gps.altitude.meters());
    vit=round(1000*gps.speed.kmph());
    
    Serial.print("lati.val=");
    Serial.print(lati);
    Serial.write(0xFF);
    Serial.write(0xFF);
    Serial.write(0xFF);
    
    Serial.print("longi.val=");
    Serial.print(longi); 
    Serial.write(0xFF);
    Serial.write(0xFF);
    Serial.write(0xFF);

    Serial.print("alti.val=");
    Serial.print(alti); 
    Serial.write(0xFF);
    Serial.write(0xFF);
    Serial.write(0xFF);

    Serial.print("vit.val=");
    Serial.print(vit); 
    Serial.write(0xFF);
    Serial.write(0xFF);
    Serial.write(0xFF);

    dist = TinyGPSPlus::distanceBetween(Lati, Longi, gps.location.lat(), gps.location.lng());
    distotal+=dist;
    distotal_int = distotal;
    Longi=gps.location.lng();
    Lati=gps.location.lat();
    Serial.print("dist.val=");
    Serial.print(distotal_int); 
    Serial.write(0xFF);
    Serial.write(0xFF);
    Serial.write(0xFF);
    
  
  } else {
    
    Serial.print("state.txt=\"");
    Serial.print(texte);
    Serial.print("\"");
    Serial.write(0xFF);
    Serial.write(0xFF);
    Serial.write(0xFF);
    n+=1;
    texte+=".";
    if (n==4) {
      texte="Position invalide";
      n=0;
    }
  }
  delay(1000);
}
