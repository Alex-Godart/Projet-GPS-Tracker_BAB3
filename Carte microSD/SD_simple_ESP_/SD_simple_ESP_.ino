#include <SPI.h>
#include <SD.h>

const int chipSelect = D8; // CS SD

void setup() {
Serial.begin(115200);
  delay(5000);
}

void loop() {  
  Serial.println("Test SD seule");

  if (!SD.begin(chipSelect)) {
    Serial.println("Erreur : SD non détectée !");
    return;
  }
  Serial.println("SD détectée !");
  File f = SD.open("/test.txt", FILE_WRITE);
  if (f) {
    f.println("Hello SD");
    f.close();
    Serial.println("Ecriture OK");
  }}
