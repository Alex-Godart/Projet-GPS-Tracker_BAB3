#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <time.h>

const char* ssid = "IPhone Alex";
const char* password = "Mot_de_passe";

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600; // Belgique = UTC+1
const int daylightOffset_sec = 3600; // heure d'été

String connexion = "Tentative de connexion";
int i = 0;
int timeout = 0;

void setup() {
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
  } else {
    Serial.println("\nEchec de la connexion WiFi");
  }

  struct tm timeinfo;

  while (!getLocalTime(&timeinfo)) {
    Serial.println("Attente synchronisation NTP...");
    delay(500);
  }

  Serial.println("Heure synchronisée !");
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop() {
  sendToFirebase(50.8503, 4.3517, 12.5, 120.0);
  delay(10000);
}

void sendToFirebase(float lat, float lon, float speed, float altitude) {
  if (WiFi.status() != WL_CONNECTED) return;

  // timestamp HH:MM:SS
  struct tm timeinfo;
  char timestamp[9];
  strftime(timestamp, sizeof(timestamp), "%H:%M:%S", &timeinfo);

  Serial.print("{");
  Serial.print("\"timestamp\":\"");
  Serial.print(timestamp);
  Serial.println("\"}");

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

  if (!getLocalTime(&timeinfo)) {
    Serial.println("Erreur temps");
    return;
  }

  String body;
  serializeJson(doc, body);

  int code = http.POST(body);

  Serial.print("Firebase code: ");
  Serial.println(code);

  http.end();
}
