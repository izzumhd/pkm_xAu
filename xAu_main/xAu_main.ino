#include <WiFi.h>
#include <HTTPClient.h>
#include <SPIFFS.h>
#include "Adafruit_TCS34725.h"

// === WiFi Config ===
const char* ssid = "Nama_WiFi";
const char* password = "Password_WiFi";

// === Google Script URL ===
const char* scriptURL = "https://script.google.com/macros/s/AKfycxxxxxxx/exec"; // ganti dengan URL milikmu

// === Sensor Setup ===
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_154MS, TCS34725_GAIN_4X);

void setup() {
  Serial.begin(115200);

  // Inisialisasi SPIFFS
  if (!SPIFFS.begin(true)) {
    Serial.println("Gagal mount SPIFFS!");
  }

  // Koneksi ke WiFi
  WiFi.begin(ssid, password);
  Serial.print("Menyambung ke WiFi");
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 10) {
    delay(1000);
    Serial.print(".");
    tries++;
  }

  if (WiFi.status() == WL_CONNECTED)
    Serial.println("\nWiFi tersambung!");
  else
    Serial.println("\nWiFi gagal, mode offline aktif.");

  // Inisialisasi sensor warna
  if (tcs.begin()) {
    Serial.println("Sensor TCS34725 terdeteksi!");
  } else {
    Serial.println("Sensor tidak terdeteksi :(");
    while (1);
  }
}

void loop() {
  uint16_t r, g, b, c;
  tcs.setInterrupt(false); // nyalain LED sensor
  delay(60);
  tcs.getRawData(&r, &g, &b, &c);
  tcs.setInterrupt(true); // matiin LED biar hemat

  float sum = c;
  float r_norm = (r / sum) * 255.0;
  float g_norm = (g / sum) * 255.0;
  float b_norm = (b / sum) * 255.0;

  Serial.printf("R: %.0f, G: %.0f, B: %.0f\n", r_norm, g_norm, b_norm);

  // Kirim atau simpan data
  if (WiFi.status() == WL_CONNECTED) {
    sendToGoogleSheets(r_norm, g_norm, b_norm);
    uploadSavedData(); // kirim backlog yang disimpan sebelumnya
  } else {
    saveOfflineData(r_norm, g_norm, b_norm);
  }

  delay(5000);
}

// === Fungsi simpan ke SPIFFS ===
void saveOfflineData(float r, float g, float b) {
  File file = SPIFFS.open("/data.txt", FILE_APPEND);
  if (!file) return;
  file.printf("%.0f,%.0f,%.0f\n", r, g, b);
  file.close();
  Serial.println("WiFi OFF - data disimpan lokal");
}

// === Fungsi kirim ke Google Sheets ===
void sendToGoogleSheets(float r, float g, float b) {
  HTTPClient http;
  String url = String(scriptURL) + "?r=" + r + "&g=" + g + "&b=" + b;
  http.begin(url);
  int code = http.GET();
  if (code == 200) Serial.println("Data terkirim ke Google Sheets!");
  else Serial.println("Gagal kirim data : " + String(code));
  http.end();
}

// === Fungsi upload backlog saat online ===
void uploadSavedData() {
  File file = SPIFFS.open("/data.txt");
  if (!file) return;

  while (file.available()) {
    String line = file.readStringUntil('\n');
    float r, g, b;
    sscanf(line.c_str(), "%f,%f,%f", &r, &g, &b);
    sendToGoogleSheets(r, g, b);
    delay(500);
  }

  file.close();
  SPIFFS.remove("/data.txt");
  Serial.println("Semua data offline sudah dikirim.");
}
