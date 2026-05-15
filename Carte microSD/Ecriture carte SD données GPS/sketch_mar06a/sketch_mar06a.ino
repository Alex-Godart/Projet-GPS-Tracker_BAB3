#include <Wire.h>


#define LSM303D 0x1D
#define SPEAKER_PIN 9

//Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
unsigned long last_flip_us = 0;
bool speaker_state;

// --- Config filtre
float fc = 5.0f;  // fréquence de coupure en Hz (3 à 10 Hz est un bon départ)
float alpha = 0.2f;  // sera recalculé dynamiquement à partir de fc et dt

// Mesures brutes
int16_t mx_raw, my_raw, mz_raw;

// Mesures filtrées
float mx_f = NAN, my_f = NAN, mz_f = NAN;

float mx_f_init = NAN, my_f_init = NAN, mz_f_init = NAN;
float mx_f_var = NAN, my_f_var = NAN, mz_f_var = NAN;

// Pour calculer dt
unsigned long t_prev_us = 0;
void writeReg(byte reg, byte value);//Owen/
void readMag(int16_t &mx, int16_t &my, int16_t &mz);//Owen/

void setup() {
  Serial.begin(9600);
  Wire.begin();
  //pinMode(SPEAKER_PIN, OUTPUT);

 

  // Config CTRL5 : résolution + ODR 50 Hz (mag)
  writeReg(0x24, 0b11110000);
  // Config CTRL6 : ±4 gauss
  writeReg(0x25, 0b00100000);
  // Config CTRL7 : magnétomètre en mode continu
  writeReg(0x26, 0b00000000);

  t_prev_us = micros();
}

void loop() {

  // 1) Lire le magnétomètre
  readMag(mx_raw, my_raw, mz_raw);

  // 2) Calcul du dt (en secondes)
  unsigned long now_us = micros();
  float dt = (now_us - t_prev_us) / 1e6f;
  t_prev_us = now_us;

  // Sécuriser dt en cas de glitch
  if (dt <= 0.0f || dt > 0.2f) dt = 0.02f; // fallback à 20 ms (= 50 Hz)

  // 3) Calculer alpha depuis fc et dt
  // RC = 1 / (2*pi*fc); alpha = dt / (RC + dt)
  const float pi = 3.14159265359f;
  float RC = 1.0f / (2.0f * pi * fc);
  alpha = dt / (RC + dt);

  // 4) Initialiser le filtre au premier passage (pour éviter un transitoire)
  if (isnan(mx_f)) {
    mx_f = mx_raw; my_f = my_raw; mz_f = mz_raw;

    mx_f_init=abs(mx_f);
    my_f_init=abs(my_f);
    mz_f_init=abs(mz_f);  
  } else {
    // 5) Appliquer le filtre EMA par axe
    mx_f += alpha * (mx_raw - mx_f);
    my_f += alpha * (my_raw - my_f);
    mz_f += alpha * (mz_raw - mz_f);
    
  }

  mx_f_var= abs(abs(mx_f)-mx_f_init);
  my_f_var= abs(abs(my_f)-my_f_init);
  mz_f_var= abs(abs(mz_f)-mz_f_init);


  

  Serial.print(mx_f_var, 1); Serial.print(",");
  Serial.print(my_f_var, 1); Serial.print(",");
  Serial.println(mz_f_var, 1);
  delay(20); // 

//Mapping sur la plage réelle
int midi_note = map(mx_f_var, 20, 100, 48, 72);
midi_note = constrain(midi_note, 48, 72);

//Calcul fréquence (Axe X) et volume (Axe Y)
int freq = 440 * pow(2.0, (midi_note - 69)/12.0);
int volume = map(my_f_var,0, 500, 0, 255);
volume = constrain(volume, 0, 255);

if(freq > 20) { //Seuil fréquence audible
  unsigned long period_us = 1000000UL / freq; //Période la note en µs
  if (micros() - last_flip_us >= (period_us / 2)) {
    last_flip_us = micros();
    speaker_state = !speaker_state;
    
    if (speaker_state) {
      analogWrite(SPEAKER_PIN, volume);
      } //On applique le volume
    else {
      analogWrite(SPEAKER_PIN, 0); //Pas de son
      }
  }
  else {
    analogWrite(SPEAKER_PIN, 0);} 
  }

 // Jouer uniquement si la fréquence change
static int last_freq = 0;
if(freq != last_freq){
    last_freq = freq;
}
}

void writeReg(byte reg, byte value) {
  Wire.beginTransmission(LSM303D);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

void readMag(int16_t &mx, int16_t &my, int16_t &mz) {
  Wire.beginTransmission(LSM303D);
  Wire.write(0x08 | 0x80); // auto-incrément depuis OUT_X_L_M
  Wire.endTransmission(false); // repeated start, plus propre
  Wire.requestFrom(LSM303D, 6);

  if (Wire.available() < 6) { return; }

  uint8_t xL = Wire.read();
  uint8_t xH = Wire.read();
  uint8_t yL = Wire.read();
  uint8_t yH = Wire.read();
  uint8_t zL = Wire.read();
  uint8_t zH = Wire.read();

  mx = (int16_t)(xH << 8 | xL);
  my = (int16_t)(yH << 8 | yL);
  mz = (int16_t)(zH << 8 | zL);
}
