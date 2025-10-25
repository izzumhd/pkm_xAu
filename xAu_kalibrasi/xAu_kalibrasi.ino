#include <Wire.h>
#include "Adafruit_TCS34725.h"

// program untuk kalibrasi warna uangnya

#define BUTTON_PIN 7

// Inisialisasi sensor TCS34725 (integration time, gain)
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  if (tcs.begin()) {
    Serial.println("TCS34725 terdeteksi!");
  } else {
    Serial.println("Sensor TCS34725 tidak ditemukan! Cek kabel SDA/SCL.");
    while (1);
  }
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) { // tombol ditekan
    delay(200); // debounce kecil
    ambilWarna();
    while (digitalRead(BUTTON_PIN) == LOW); // tunggu sampai tombol dilepas
  }
}

void ambilWarna() {
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);

  // Normalisasi biar gampang dibanding
  float r_norm = (float)r / c * 255.0;
  float g_norm = (float)g / c * 255.0;
  float b_norm = (float)b / c * 255.0;

  Serial.println("=== Data Warna Terdeteksi ===");
  Serial.print("Raw R: "); Serial.print(r);
  Serial.print(" G: "); Serial.print(g);
  Serial.print(" B: "); Serial.println(b);

  Serial.print("RGB Normalized: ");
  Serial.print((int)r_norm); Serial.print(", ");
  Serial.print((int)g_norm); Serial.print(", ");
  Serial.println((int)b_norm);
  Serial.println("==============================\n");
}
