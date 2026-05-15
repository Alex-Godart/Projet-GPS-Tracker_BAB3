#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <SPI.h>
#include <SD.h>

// ---------------- CONFIGURATION ----------------

// GPS sur SoftwareSerial
static const int GPS_RX = 4;  // ESP reçoit depuis TX du GPS
static const int GPS_TX = 5;  // ESP vers RX du GPS (optionnel)
static const uint32_t GPS_BAUD = 9600;

// SD Card
const int SD_CS = 12;          

// Nextion
static const uint32_t NEXTION_BAUD = 115200;

// ---------------- OBJETS ----------------
TinyGPSPlus gps;
SoftwareSerial gpsSerial(GPS_RX, GPS_TX);

// ---------------- STRUCTURES ----------------
struct Position {
  double lat;
  double lng;
};

Position lastPos = {0, 0};
bool hasFix = false;
double distTotal = 0;

// ---------------- OUTILS ----------------
void nextionEnd() {
  Serial.write(0xFF);
  Serial.write(0xFF);
  Serial.write(0xFF);
}

void nextionPrint(String cmd) {
  Serial.print(cmd);
  nextionEnd();
}

// ---------------- AFFICHAGE ----------------
void afficherRechercheGPS() {
  static int dots = 0;
  String txt = "Recherche GPS";

  for (int i = 0; i < dots; i++) txt += ".";
  dots = (dots + 1) % 4;

  nextionPrint("state.txt=\"" + txt + "\"");
}

// ---------------- ENREGISTREMENT SD ----------------
void enregistrerSD(const Position &p) {
  File f = SD.open("/GPS_data.txt", FILE_WRITE);
  if (!f) {
    nextionPrint("carte.txt=\"Erreur fichier\"");
    return;
  }

  f.print(p.lat, 6); f.print(",");
  f.print(p.lng, 6); f.print(",");
  f.print(gps.speed.kmph()); f.print(",");
  f.print(gps.altitude.meters()); f.print(",");
  f.print("0,0,");

  if (gps.date.isValid() && gps.time.isValid()) {
    f.print(gps.time.hour()); f.print(":");
    f.print(gps.time.minute()); f.print(":");
    f.println(gps.time.second());
  } else {
    f.println("00:00:00");
  }

  f.close();
  nextionPrint("carte.txt=\"Données enregistrées\"");
}

// ---------------- AFFICHAGE NEXTION ----------------
void afficherNextion(const Position &p) {
  long lati = round(p.lat * 1000);
  long longi = round(p.lng * 1000);
  long alti = round(gps.altitude.meters() * 1000);
  long vit  = round(gps.speed.kmph() * 100);
  long dist = round(distTotal);

  nextionPrint("lati.val=" + String(lati));
  nextionPrint("longi.val=" + String(longi));
  nextionPrint("alti.val=" + String(alti));
  nextionPrint("vit.val=" + String(vit));
  nextionPrint("dist.val=" + String(dist));
}

// ---------------- TRAITEMENT GPS ----------------
void traiterGPS() {
  if (!gps.location.isValid()) {
    afficherRechercheGPS();
    return;
  }
  nextionPrint("state.txt=\"GPS trouvé\"");
  Position current = { gps.location.lat(), gps.location.lng() };

  if (!hasFix) {
    lastPos = current;
    hasFix = true;    
  }

  double d = TinyGPSPlus::distanceBetween(
    lastPos.lat, lastPos.lng,
    current.lat, current.lng
  );

  if (d >= 2.5) {  // Filtre anti-bruit
    distTotal += d;
    lastPos = current;
    
   // enregistrerSD(current);
    afficherNextion(current);
  }
}

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(NEXTION_BAUD);   // Nextion
  gpsSerial.begin(GPS_BAUD);    // GPS

  nextionPrint("carte.txt=\"Init SD...\"");

 // if (!SD.begin(SD_CS)) {
   // nextionPrint("carte.txt=\"SD absente\"");
  //} else {
  //  nextionPrint("carte.txt=\"SD OK\"");
  //}

  delay(1000);
  nextionPrint("state.txt=\"Recherche GPS\"");
}

// ---------------- LOOP ----------------
void loop() {
  while (gpsSerial.available()) {
    if (gps.encode(gpsSerial.read())) {
      traiterGPS();
    }
  }

  // Détection GPS non câblé
  static unsigned long t0 = millis();
  if (millis() - t0 > 5000 && gps.charsProcessed() < 10) {
    nextionPrint("state.txt=\"GPS non detecte. Verifier cablage\"");
    delay(1000);
  }
}
