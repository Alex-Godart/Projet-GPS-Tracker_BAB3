#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#define GPS_EN 25 //pin pour desactiver gps

const char* ssid = "IPhone Alex";
const char* password = "Mot_de_passe";

String connexion = "Tentative de connexion";
int i = 0;
int timeout = 0;

void setup() {
  pinMode(GPS_EN, OUTPUT);
  digitalWrite(GPS_EN, LOW); // GPS OFF
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n");

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    while (i <= 3 && timeout < 200) {
      Serial.println(connexion);
      connexion += ".";
      i++;
      timeout++;
      delay(500);
    }
    i = 0;
    connexion = "Tentative de connexion";
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnexion etablie");
    Serial.print("Adresse IP : ");
    Serial.println(WiFi.localIP());
  } 
  
  else {
    Serial.println("\nEchec de la connexion WiFi");
  }


void loop() {
  //Recevoir les infos du LoRa
  sendToFirebase(//lat, lon, speed, altitude, timestamp);
  delay(10000);
}

void sendToFirebase(float lat, float lon, float speed, float altitude, String timestamp) {
  if (WiFi.status() != WL_CONNECTED) return;
  
  HTTPClient http;

  String url = "https://gps-tracker-80722-default-rtdb.europe-west1.firebasedatabase.app/data.json";

  http.begin(url);
  http.addHeader("Content-Type", "application/json");

  StaticJsonDocument<256> doc;

  doc["latitude"] = lat;
  doc["longitude"] = lon;
  doc["speed"] = speed;
  doc["altitude"] = altitude;
  doc["timestamp"] = timestamp;
  
  String body;
  serializeJson(doc, body);

  int code = http.POST(body);

  Serial.print("Firebase code: ");
  Serial.println(code);

  http.end();
}