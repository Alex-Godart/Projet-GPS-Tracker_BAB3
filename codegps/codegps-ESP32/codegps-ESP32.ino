#include <TinyGPS++.h>
#define GPS_EN 25

HardwareSerial gpsSerial(1); //UART 1
static const uint32_t GPSBaud = 9600; // Baudrate du module NEO-6M

TinyGPSPlus gps;

void setup() {
  pinMode(GPS_EN, OUTPUT);
  digitalWrite(GPS_EN, HIGH);
  Serial.begin(115200);
  gpsSerial.begin(GPSBaud, SERIAL_8N1, 16, 17);
  Serial.println("Recherche de signal GPS...");
}

void loop() { 
  // Lire les données du GPS
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial .read())) {
      displayInfo();
      delay(1000);
      digitalWrite(GPS_EN, LOW);
      Serial.print("GPS OFF");
      return;
    }
  }

  // Si aucune donnée reçue après 5 secondes
  if (gpsSerial.available() < 0) {
    Serial.println("Erreur: Signal GPS non trouve");
    delay(5000);
  }
}

void displayInfo() {
  if (gps.location.isValid()) {
    Serial.print("Latitude: ");
    Serial.println(gps.location.lat(),6);
    Serial.print("Longitude: ");
    Serial.println(gps.location.lng(), 6);
    Serial.print("Altitude: ");
    if (gps.altitude.isValid()){
      Serial.println(gps.altitude.meters());
    }
    if(!gps.altitude.isValid()){
      Serial.println("invalide (fix en cours)");
    }
    Serial.print("Speed: ");
    if (gps.speed.isValid()){
      Serial.print(gps.speed.kmph());
      Serial.println(" km/h");
    }
    if(!gps.speed.isValid()){
      Serial.println("invalide (fix en cours)");
    }
  } 
  else {
    Serial.println("Position non valide (fix en cours...)");
  }
  Serial.print("GPS date&time: ");
  if(gps.date.isValid() && gps.time.isValid()){
    Serial.print(gps.date.year());
    Serial.print("-");
    Serial.print(gps.date.month());
    Serial.print("-");
    Serial.print(gps.date.day());
    Serial.print(" ");
    Serial.print(gps.time.hour());
    Serial.print(":");
    Serial.print(gps.time.minute());
    Serial.print(":");
    Serial.println(gps.time.second());
  }
   if(!gps.date.isValid() && !gps.time.isValid()){
    Serial.println("invalide (fix en cours)");
  }
  Serial.println();
}
