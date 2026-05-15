#include <SPI.h>

#define PIN_NSS   5
#define PIN_RST   25
#define PIN_DIO0  26

String SECRET_KEY = "SEMI_1009";
uint8_t MY_ID = 0x45; // Mon ID → j’accepte seulement les messages destinés à 0x45
String messageRecu="";

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

    packet="";
    messageRecu="";
  }
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