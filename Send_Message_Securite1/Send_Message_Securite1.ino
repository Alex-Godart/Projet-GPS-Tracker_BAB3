#include <SPI.h>

#define PIN_NSS   5
#define PIN_RST   25
#define PIN_DIO0  26

String SECRET_KEY = "SEMI_1009";
uint8_t SRC_ID  = 0x01; // Qui envoie
uint8_t DEST_ID = 0x45; // À qui on envoie

// ------------------------ SETUP ------------------------
void setup() {
  Serial.begin(115200);
  SPI.begin(18, 19, 23);
  pinMode(PIN_NSS, OUTPUT);
  pinMode(PIN_DIO0, INPUT);

  lora_init();

  delay(1000);
  //ScanAllRegistre();
}

// ------------------------ LOOP ------------------------
void loop() {
  String message;
  message = String("Hello LoRa! Avec sécurité") + "\t" + String(millis());
  //message="UMONS";
  sendSecure(message);
  delay(2000);
}

// ------------------------ SPI SX1276 ------------------------
void writeReg(uint8_t addr, uint8_t val) {
  digitalWrite(PIN_NSS, LOW);
  SPI.transfer(addr | 0x80);
  SPI.transfer(val);
  digitalWrite(PIN_NSS, HIGH);
}

uint8_t readReg(uint8_t addr) {
  digitalWrite(PIN_NSS, LOW);
  SPI.transfer(addr & 0x7F);
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

// ------------------------ CRC Privé ------------------------
uint8_t makeCRC(String msg) {
  uint8_t crc = 0;
  for (int i = 0; i < msg.length(); i++) crc ^= msg[i];
  for (int i = 0; i < SECRET_KEY.length(); i++) crc ^= SECRET_KEY[i];
  return crc;
}

// ------------------------ Init LoRa SX1276 ------------------------
void lora_init() {

  pinMode(PIN_RST, OUTPUT);
  digitalWrite(PIN_RST, LOW); delay(10);
  digitalWrite(PIN_RST, HIGH); delay(10);

  writeReg(0x01, 0x80);     // Forcer mode lora
  delay(10);

  writeReg(0x06, 0xD9);     // MSB Fréquence 868MHz
  writeReg(0x07, 0x00);     // MSB Fréquence 868MHz
  writeReg(0x08, 0x00);     // LSB Fréquence 868MHz

  writeReg(0x09, 0x8F);     // Activer +20dBm
  writeReg(0x4D, 0x87);     // Activer +20dBm
  writeReg(0x0B, 0x2B);     // Activer +20dBm

  writeReg(0x1D, 0x72);     // Bande passante 125KHz  +  Error rate 4/5
  writeReg(0x1E, 0x74);     // SpreadingFactor 128 chips / symbol  +  Tx en mode normal  +  CRC Enable
  writeReg(0x40, 0x40);
  writeReg(0x01, 0x81);     // Forcer mode lora  +  Mettre en stanby
  writeReg(0x12, 0xFF);     // Reset registre des flag IRQ

  //writeReg(0x39, 0x24);  
}

// ------------------------ Envoi SX1276 ------------------------
void lora_send(String msg) {

  writeReg(0x01, 0x81);
  writeReg(0x0E, 0x00);
  writeReg(0x0D, 0x00);

  for (int i = 0; i < msg.length(); i++)
    writeReg(0x00, msg[i]);

  writeReg(0x22, msg.length());
  writeReg(0x12, 0xFF);
  writeReg(0x01, 0x83);

  while (digitalRead(PIN_DIO0) == 0);
  writeReg(0x12, 0xFF);
}

// ------------------------ MESSAGE SECURISE ------------------------
void sendSecure(String text) {

  String dataEnc = xorCipher(text);

  String frame = "";
  frame += (char)DEST_ID;
  frame += (char)SRC_ID;
  frame += dataEnc;

  uint8_t crc = makeCRC(frame);
  frame += (char)crc;
  frame += (char)0x00;

  lora_send(frame);

  //Serial.println("[TX] Message envoyé (sécurisé)");
  Serial.print("[TX] : \t\t"); Serial.println(text);
  Serial.print("[TX cryt] : \t"); Serial.println(frame);
  Serial.println("");
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
