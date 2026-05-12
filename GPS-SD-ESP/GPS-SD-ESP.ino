#include <TinyGPS++.h>
#include <SPI.h>
#include <SD.h>

static const int RXPin = 16;
static const int TXPin = 17;
static const uint32_t GPSBaud = 9600;

HardwareSerial GPS(1); // UART1
TinyGPSPlus gps;

const int chipSelect = 25;

void setup() {
  Serial.begin(115200); // moniteur série
  GPS.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin);

  Serial.println("Recherche de signal GPS...");
  Serial.print("Starting SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("SD card failed or not present!");
    return;
  }

  Serial.println("SD card connected.");
  delay(2000);
}

void loop() {
  Serial.println("Lecture des données en cours...");
  delay(10000);

  while (GPS.available() > 0) {
    if (gps.encode(GPS.read())) {
      displayInfo();
      delay(10000);
    }
  }
}

void displayInfo() {
  if (gps.location.isValid()) {

    // Ouvre le fichier
    File dataFile = SD.open("/GPS_data.txt", FILE_WRITE);

    if (dataFile) {
      Serial.print(gps.location.lat(), 4);
      Serial.print(",");
      dataFile.print(gps.location.lat(), 4);
      dataFile.print(",");

      Serial.print(gps.location.lng(), 4);
      Serial.print(",");
      dataFile.print(gps.location.lng(), 4);
      dataFile.print(",");

      if (gps.speed.isValid()) {
        Serial.print(gps.speed.kmph());
        dataFile.print(gps.speed.kmph());
      } else {
        Serial.print("0");
        dataFile.print("0");
      }

      Serial.print(",");
      dataFile.print(",");

      if (gps.altitude.isValid()) {
        Serial.print(gps.altitude.meters());
        dataFile.print(gps.altitude.meters());
      } else {
        Serial.print("0");
        dataFile.print("0");
      }

      // Heure
      if (gps.date.isValid() && gps.time.isValid()) {
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
      Serial.println("Données enregistrées");
    }

  } 
  else {
    Serial.println("En attente du signal GPS...");
  }
}