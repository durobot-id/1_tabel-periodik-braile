#include <Arduino.h>
#include <Wire.h>

//
// ---- Konfigurasi PCF8575 ----
//
#define NUM_MODULES 6           // total 6 modul
#define BASE_ADDR  0x20         // alamat dasar 0x20
const unsigned long READ_INTERVAL = 200; // ms antar baca
unsigned long lastRead = 0;

uint16_t lastState[NUM_MODULES]; // simpan status sebelumnya

//
// ---- Fungsi Baca & Tulis PCF8575 ----
//
uint16_t readPCF8575(uint8_t addr) {
  uint16_t value = 0xFFFF;
  Wire.requestFrom(addr, (uint8_t)2);
  if (Wire.available() >= 2) {
    uint8_t low = Wire.read();
    uint8_t high = Wire.read();
    value = ((uint16_t)high << 8) | low;
  }
  return value;
}

bool writePCF8575(uint8_t addr, uint16_t value) {
  Wire.beginTransmission(addr);
  Wire.write(value & 0xFF);
  Wire.write((value >> 8) & 0xFF);
  return (Wire.endTransmission() == 0);
}

//
// ---- Setup ----
//
void setup() {
  Serial.begin(9600);
  while (!Serial) { delay(10); }

  Wire.begin();
  Wire.setClock(400000);

  // Set semua pin sebagai input pull-up dan inisialisasi status awal
  for (uint8_t i = 0; i < NUM_MODULES; i++) {
    uint8_t addr = BASE_ADDR + i;
    writePCF8575(addr, 0xFFFF);
    lastState[i] = readPCF8575(addr); // baca kondisi awal
  }

  Serial.println("=== MONITOR 128 TOMBOL PCF8575 ===");
  Serial.println("Format index: \"xxx\" = {m odul}{pin_asli}");
  Serial.println("Pin di modul: 00–07 dan 10–17");
  Serial.println("Pesan muncul saat tombol ditekan (transisi 0→1).");
  Serial.println("====================================");
}

//
// ---- Loop ----
//
void loop() {
  unsigned long now = millis();
  if (now - lastRead >= READ_INTERVAL) {
    lastRead = now;

    // Baca tiap modul
    for (uint8_t mod = 0; mod < NUM_MODULES; mod++) {
      uint8_t addr = BASE_ADDR + mod;
      uint16_t val = readPCF8575(addr);

      // Cek perubahan status dari 0 → 1 (tombol baru ditekan)
      for (uint8_t bit = 0; bit < 16; bit++) {
        int current = ((val & (1 << bit)) == 0) ? 1 : 0;     // 1 = ditekan
        int previous = ((lastState[mod] & (1 << bit)) == 0) ? 1 : 0;

        if (current == 1 && previous == 0) {
          // Tentukan label pin asli di modul (00–07, 10–17)
          int pinLabel;
          if (bit < 8)
            pinLabel = bit;       // 0–7 → P00–P07
          else
            pinLabel = bit + 2;   // 8–15 → P10–P17

          // Buat index "xxx"
          char index[4];
          snprintf(index, sizeof(index), "%d%02d", mod, pinLabel);

          Serial.println(index);
        }
      }

      // Simpan state terakhir
      lastState[mod] = val;
    }
  }
}
