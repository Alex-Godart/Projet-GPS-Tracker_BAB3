#include <SoftwareSerial.h>  
SoftwareSerial gps(4,3); // 12 pour le Tx du recepteur et 13 pour le Rx du recepteur 

void setup() {
  Serial.begin(9600);
  gps.begin(9600);
}

void loop() {
 while (gps.available()){
  char c = gps.read();
  Serial.print (c);
 }
}
