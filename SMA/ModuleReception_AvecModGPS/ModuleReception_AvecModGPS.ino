#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "TinyGPSPlus.h"

#define PIN_NSS   5
#define PIN_RST   25
#define PIN_DIO0  26
#define PIN_ALIM_GPS  17
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 

String SECRET_KEY = "SEMI_1009";
uint8_t MY_ID = 0x45; // Mon ID → j’accepte seulement les messages destinés à 0x45
String messageRecu="";

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// The TinyGPSPlus object
HardwareSerial ss(1);
TinyGPSPlus gps;

// ------------------------ SETUP ------------------------
void setup() {  
  Serial.begin(115200);
  Serial.print("Initialisation: ");
  SPI.begin(18, 19, 23);

  pinMode(PIN_NSS, OUTPUT);
  pinMode(PIN_DIO0, INPUT);

  lora_init();
  delay(1000);
  //ScanAllRegistre();

  // Init module GPS TEL0094
  pinMode(PIN_ALIM_GPS, OUTPUT);
  digitalWrite(PIN_ALIM_GPS, HIGH);   // Alimentation du module GPS
  ss.begin(9600, SERIAL_8N1, 4, 16);  // RX=IO4, TX=IO16

  // Init écran oled
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(1000);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("Recep.LoRa");
  display.setCursor(0, 50);
  display.println("SEMI-UMONS");
  display.display(); 
  display.setTextSize(1);

  Serial.println("Terminée");
  delay(2000);
}

// ------------------------ LOOP ------------------------
void loop() {
  String packet = lora_read();
  if (packet != "") {
    MessageDecrypted(packet);

    Serial.print("Message crypté : "); Serial.println(packet);
    Serial.print("Message décrypté : "); Serial.println(messageRecu);

    int16_t pk_rssi = getPacketRSSI();
    float  pk_snr  = getPacketSNR();
    int16_t rssi   = getRSSI();

    Serial.print("Packet RSSI (registre 0x1A): "); Serial.print(pk_rssi); Serial.println(" dBm");
    Serial.print("RSSI canal  (Registre 0x1B): "); Serial.print(rssi);    Serial.println(" dBm");
    Serial.print("Packet SNR (Registre 0x19) : "); Serial.print(pk_snr);  Serial.println(" dB");
    Serial.println("");

    // Affichage du message sur écran oled
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(messageRecu);
    display.setCursor(0, 48);
    display.print("RSSI:");
    display.print(rssi);
    display.println("dBm");
    display.print("SNR:");
    display.print(pk_snr);
    display.print("dB");
    display.display(); 

    packet="";
    messageRecu="";
  }
  
  // Lecture trame module GPS TEL0094
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      if(gps.satellites.isValid()) {
        displayInfoGPS();

        display.setCursor(0, 20);
        display.print("Pos. GPS:");
        display.print("lat ");
        display.print(gps.location.lat(), 6);
        display.print("lgt ");
        display.println(gps.location.lng(), 6);
        display.display();
        break;
      }else{
        Serial.println("Pas de message GPS");
        display.setCursor(0, 20);
        display.println("Pas de message GPS");
        display.display();        
      }
  delay(1000);
}

// ------------------------ SPI SX1276 ------------------------
void writeReg(uint8_t a, uint8_t v) {
  digitalWrite(PIN_NSS, LOW);
  SPI.transfer(a | 0x80);
  SPI.transfer(v);
  digitalWrite(PIN_NSS, HIGH);
}

uint8_t readReg(uint8_t a) {
  digitalWrite(PIN_NSS, LOW);
  SPI.transfer(a & 0x7F);
  uint8_t v = SPI.transfer(0);
  digitalWrite(PIN_NSS, HIGH);
  return v;
}

// ------------------------ XOR Crypto ------------------------
String xorCipher(String data) {
  String out = data;
  for (int i = 0; i < data.length(); i++) {
    out[i] = data[i] ^ SECRET_KEY[i % SECRET_KEY.length()];
  }
  return out;
}

// ------------------------ CRC ------------------------
uint8_t makeCRC(String msg) {
  uint8_t crc = 0;
  for (int i = 0; i < msg.length(); i++) crc ^= msg[i];
  for (int i = 0; i < SECRET_KEY.length(); i++) crc ^= SECRET_KEY[i];
  return crc;
}

// ------------------------ Init LoRa ------------------------
void lora_init() {

  pinMode(PIN_RST, OUTPUT);
  digitalWrite(PIN_RST, LOW); delay(10);
  digitalWrite(PIN_RST, HIGH); delay(10);

  writeReg(0x01, 0x80);    // Forcer mode lora 
  delay(10);

  writeReg(0x06, 0xD9);   // MSB Fréquence 868MHz
  writeReg(0x07, 0x00);   // MSB Fréquence 868MHz
  writeReg(0x08, 0x00);   // LSB Fréquence 868MHz

  writeReg(0x1D, 0x72);   // Bande passante 125KHz  +  Error rate 4/5
  writeReg(0x1E, 0x74);   // SpreadingFactor 128 chips / symbol  +  Tx en mode normal  +  CRC Enable

  writeReg(0x40, 0x00);

  writeReg(0x01, 0x85);  // Forcer mode lora  +  Mettre mode RX CONTINUOUS
  writeReg(0x12, 0xFF);  // Reset registre des flag IRQ

}

// ------------------------ QUALITE SIGNAL ------------------------
int16_t getPacketRSSI() { return readReg(0x1A) - 157; }
float getPacketSNR() { return ((int8_t)readReg(0x19)) / 4.0; }
int16_t getRSSI() { return readReg(0x1B) - 157; }

// ------------------------ LECTURE PAQUET ------------------------
String lora_read() {

  if (!digitalRead(PIN_DIO0)) return "";

  uint8_t irq = readReg(0x12);
  if (!(irq & 0x40)) { writeReg(0x12, 0xFF); return ""; }

  uint8_t len = readReg(0x13);
  writeReg(0x0D, readReg(0x10));

  String packet = "";
  for (int i = 0; i < len; i++)
    packet += (char)readReg(0x00);

  writeReg(0x12, 0xFF);
  return packet;
}

// ------------------------ TRAITEMENT SECURISE ------------------------
void MessageDecrypted(String p) {

  if (p.length() < 4) return;

  uint8_t dest = p[0];
  uint8_t src  = p[1];
  uint8_t crc_rx = p[p.length()-2];

  if (dest != MY_ID) {
    Serial.println("Trame rejetée : pas pour moi");
    return;
  }

  String body = p.substring(0, p.length()-2);
  if (makeCRC(body) != crc_rx) {
    Serial.println("Trame rejetée : CRC invalide");
    return;
  }
  String encrypted = p.substring(2, p.length()-2);
  String decrypted = xorCipher(encrypted);
  messageRecu = decrypted;
}

// -----------------Lecture état des registres ----------------
void ScanAllRegistre(){
  int val;
  Serial.println("");
  for(int r=0x00; r<=0x70; r++){
    val = readReg(r);
    
    Serial.print("Registre 0x");
    if(r < 0x10) Serial.print("0");  // ? AJOUT pour l'adresse
    Serial.print(r, HEX);
    
    Serial.print(" : 0x");
    if(val < 0x10) Serial.print("0");  // Pour la valeur
    Serial.print(val, HEX);
    
    Serial.print("\t0b");
    for(int i=7; i>=0; i--) Serial.print((val>>i)&1);
    
    Serial.println("");
  }
}

void displayInfoGPS()
{
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}