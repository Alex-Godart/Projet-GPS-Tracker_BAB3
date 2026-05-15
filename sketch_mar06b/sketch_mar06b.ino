#include <Wire.h>
#include <LedControl.h>

#define LSM303D 0x1D
#define SPEAKER_PIN 9

// MAX7219
LedControl lc = LedControl(11, 13, 10, 1);

unsigned long last_flip_us = 0;
bool speaker_state;

// filtre
float fc = 5.0f;
float alpha = 0.2f;

int16_t mx_raw, my_raw, mz_raw;

float mx_f = NAN, my_f = NAN, mz_f = NAN;
float mx_f_init = NAN, my_f_init = NAN, mz_f_init = NAN;

float mx_f_var = NAN, my_f_var = NAN, mz_f_var = NAN;

unsigned long t_prev_us = 0;

void writeReg(byte reg, byte value);
void readMag(int16_t &mx, int16_t &my, int16_t &mz);
void afficherBarre(int volume);

void setup() {

  Serial.begin(9600);
  Wire.begin();

  lc.shutdown(0,false);
  lc.setIntensity(0,8);
  lc.clearDisplay(0);

  writeReg(0x24, 0b11110000);
  writeReg(0x25, 0b00100000);
  writeReg(0x26, 0b00000000);

  t_prev_us = micros();
}

void loop() {

  readMag(mx_raw, my_raw, mz_raw);

  unsigned long now_us = micros();
  float dt = (now_us - t_prev_us) / 1e6f;
  t_prev_us = now_us;

  if (dt <= 0.0f || dt > 0.2f) dt = 0.02f;

  const float pi = 3.14159265359f;
  float RC = 1.0f / (2.0f * pi * fc);
  alpha = dt / (RC + dt);

  if (isnan(mx_f)) {

    mx_f = mx_raw;
    my_f = my_raw;
    mz_f = mz_raw;

    mx_f_init = abs(mx_f);
    my_f_init = abs(my_f);
    mz_f_init = abs(mz_f);

  } else {

    mx_f += alpha * (mx_raw - mx_f);
    my_f += alpha * (my_raw - my_f);
    mz_f += alpha * (mz_raw - mz_f);
  }

  mx_f_var = abs(abs(mx_f) - mx_f_init);
  my_f_var = abs(abs(my_f) - my_f_init);
  mz_f_var = abs(abs(mz_f) - mz_f_init);

  // --- PRINT (on garde tout)
  Serial.print(mx_f_var,1); Serial.print(",");
  Serial.print(my_f_var,1); Serial.print(",");
  Serial.println(mz_f_var,1);

  delay(20);

  // fréquence
  int midi_note = map(mx_f_var,20,100,48,72);
  midi_note = constrain(midi_note,48,72);

  int freq = 440 * pow(2.0,(midi_note-69)/12.0);

  // volume
  int volume = map(my_f_var,0,500,0,255);
  volume = constrain(volume,0,255);

  // affichage barre
  afficherBarre(volume);

  // génération son
  if(freq > 20){

    unsigned long period_us = 1000000UL / freq;

    if (micros() - last_flip_us >= period_us/2){

      last_flip_us = micros();
      speaker_state = !speaker_state;

      if (speaker_state)
        analogWrite(SPEAKER_PIN, volume);
      else
        analogWrite(SPEAKER_PIN, 0);
    }
  }
}

// -------- BARRE VOLUME --------

void afficherBarre(int volume){

  lc.clearDisplay(0);

  int leds = map(volume,0,255,0,8);

  for(int i=0;i<leds;i++){
    lc.setLed(0,7-i,0,true);
  }
}

// -------- LSM303D --------

void writeReg(byte reg, byte value){
  Wire.beginTransmission(LSM303D);
  Wire.write(reg);
  Wire.write(value);
  Wire.endTransmission();
}

void readMag(int16_t &mx, int16_t &my, int16_t &mz){

  Wire.beginTransmission(LSM303D);
  Wire.write(0x08 | 0x80);
  Wire.endTransmission(false);
  Wire.requestFrom(LSM303D,6);

  if(Wire.available() < 6) return;

  uint8_t xL = Wire.read();
  uint8_t xH = Wire.read();
  uint8_t yL = Wire.read();
  uint8_t yH = Wire.read();
  uint8_t zL = Wire.read();
  uint8_t zH = Wire.read();

  mx = (int16_t)(xH<<8 | xL);
  my = (int16_t)(yH<<8 | yL);
  mz = (int16_t)(zH<<8 | zL);
}
