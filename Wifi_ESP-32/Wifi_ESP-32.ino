#include <WiFi.h>

const char* ssid = "IPhone Alex";
const char* password = "Mot_de_passe";
String connexion = "Tentative de connexion";
int i = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n");

  WiFi.begin(ssid, password);
  
  while(WiFi.status() != WL_CONNECTED){
    while(i!=3){
      Serial.println(connexion);
      connexion += ".";
      i ++;
      Serial.println(i);
      delay(100);   
    }
    i = 0;
    connexion = "Tentative de connexion";
  }
  Serial.println("\n");
  Serial.println("Connexion etablie!");
  Serial.print("Adresse IP : ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // put your main code here, to run repeatedly:

}
