#define GPS_EN 25 //pin pour dés/activer gps

void setup() {
  pinMode(GPS_EN,OUTPUT);

}

void loop() {
  digitalWrite(GPS_EN,HIGH);
  Serial.println("GPS fonctionne");
  delay(5000);
  digitalWrite(GPS_EN, LOW);
  Serial.println("GPS fonctionne plus");
  delay(5000);
}
