#include <TinyGPS++.h>
#include <SPI.h>
#include <SD.h>
#define GPS_EN 25 //pin pour activer gps

// This is the pin that activates the SD card module.
const int chipSelect = 27; // 

HardwareSerial GPSserial(1); // UART1
TinyGPSPlus gps;

void setup() {
  pinMode(GPS_EN, OUTPUT);
  digitalWrite(GPS_EN, HIGH); // GPS ON
  Serial.begin(115200); // debug
  GPSserial.begin(9600, SERIAL_8N1, 16, 17); 
// RX = 16, TX = 17 (modifiable selon ton câblage)
  Serial.println("Recherche de signal GPS...");
  Serial.print("Starting SD card...");

  // Try to start the SD card module
  if (!SD.begin(chipSelect)) {
    Serial.println("SD card failed or not present!");
    return; // Stop the program here if it fails
  }
  Serial.println("SD card connected.");
  delay(2000);
}

void loop() {
  // Lire les données du GPS
  Serial.println("Lecture des données en cours...");
  while (GPSserial.available() > 0) {
  if (gps.encode(GPSserial.read())) {
    displayInfo();
  }
}
}

void displayInfo() {
  if (gps.location.isValid()) {
      // Open the file 'data.txt'. FILE_WRITE allows us to save new data.
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
  Serial.println("Données enregistrées");
 }
}
if(!gps.location.isValid()){
  Serial.println("En attente du signal GPS...");
  }
}
