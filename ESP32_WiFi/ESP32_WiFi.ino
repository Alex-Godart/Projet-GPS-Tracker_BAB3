#include <WiFi.h>
#include <HTTPClient.h>

const char* ssid= "IPhone Alex";
const char* password = "Mot_de_passe";
String connexion = "Tentative de connexion";
int i = 0;
int timeout = 0;     

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n");

  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED){
    while(i <= 3 && timeout < 20){
      Serial.println(connexion);
      connexion += ".";
      i ++;
      timeout ++;
      delay(500);
    }
    i = 0;
    connexion = "Tentative de connexion";
  }
  if (WiFi.status() == WL_CONNECTED){
    Serial.println("\n");
    Serial.println("Connexion etablie!");
    Serial.print("Adresse IP : ");
    Serial.println(WiFi.localIP());
  }
  else{
    Serial.println("\nEchec connexion WiFi");
  }
}

void loop() {
}

