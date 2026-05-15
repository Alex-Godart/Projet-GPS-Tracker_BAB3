#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <SPI.h>
#include <SD.h>

// This is the pin that activates the SD card module.
// It is usually pin 10 on Uno/Nano boards.
const int chipSelect = 10;

// Broches RX, TX pour le GPS
static const int RXPin = 3, TXPin = 4;
static const uint32_t GPSBaud = 9600; // Baudrate du module NEO-6M

TinyGPSPlus gps;
SoftwareSerial gpsSerial(RXPin, TXPin);

void setup() {
  Serial.begin(9600);
  gpsSerial.begin(GPSBaud);
  Serial.println("Recherche de signal GPS...");
  Serial.print("Starting SD card...");

  // Try to start the SD card module
  if (!SD.begin(chipSelect)) {
    Serial.println("SD card failed or not present!");
    return; // Stop the program here if it fails
  }
  Serial.println("SD card connected.");
}

void loop() {
  // Lire les données du GPS
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial .read())) {
      displayInfo();
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
      // Open the file 'data.txt'. FILE_WRITE allows us to save new data.
  File dataFile = SD.open("GPS_data.txt", FILE_WRITE);
   if (dataFile) {
    Serial.print(gps.location.lat(), 4);
    Serial.print(",");
    dataFile.print(gps.location.lat(), 4);
    dataFile.print(",");
    Serial.print(gps.location.lng(), 4);
    Serial.print(",");
    dataFile.print(gps.location.lng(), 4);
    dataFile.print(",");
    if (gps.speed.isValid()){
      Serial.print(gps.speed.kmph());
      dataFile.print(gps.speed.kmph());
    }
    if(!gps.speed.isValid()){
      Serial.print("0");
      dataFile.print("0");
    }
    Serial.print(",");
    dataFile.print(",");
    if (gps.altitude.isValid()){
      Serial.print(gps.altitude.meters());
      dataFile.print(gps.altitude.meters());
    }
    if (!gps.altitude.isValid()){
      Serial.print("0");
      dataFile.print("0");
    }
    Serial.print(",0,0,");
    dataFile.print(",0,0,");
  //Voir direction + precision si possible
  if(gps.date.isValid() && gps.time.isValid()){
    Serial.print(gps.time.hour());
    Serial.print(":");
    Serial.print(gps.time.minute());
    Serial.print(":");
    Serial.println(gps.time.second());
    dataFile.print(gps.time.hour());
    dataFile.print(":");
    dataFile.print(gps.time.minute());
    dataFile.print(":");
    dataFile.println(gps.time.second());
  }
  dataFile.close();
 }
}
if(!gps.location.isValid()){
  Serial.println("En attente du signal GPS...");
  }
}
