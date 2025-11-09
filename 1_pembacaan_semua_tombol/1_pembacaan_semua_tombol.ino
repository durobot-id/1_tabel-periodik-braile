#include <Arduino.h>
#include <Wire.h>

//
// ---- Konfigurasi PCF8575 ----
//
#define NUM_MODULES 8           // total 8 modul
#define BASE_ADDR  0x20         // mulai dari alamat 0x20
const unsigned long READ_INTERVAL = 300; // ms
unsigned long lastRead = 0;

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
  Serial.begin(115200);
  while (!Serial) { delay(10); }

  Wire.begin();
  Wire.setClock(400000);

  // Set semua pin di tiap modul sebagai input pull-up
  for (uint8_t i = 0; i < NUM_MODULES; i++) {
    uint8_t addr = BASE_ADDR + i;
    writePCF8575(addr, 0xFFFF);
  }

  Serial.println("=== MONITOR 128 TOMBOL PCF8575 ===");
  Serial.println("Format: 0 = tidak ditekan, 1 = ditekan");
  Serial.println("====================================");
}

//
// ---- Loop ----
//
void loop() {
  unsigned long now = millis();
  if (now - lastRead >= READ_INTERVAL) {
    lastRead = now;

    Serial.println();
    // Loop untuk 8 modul
    for (uint8_t mod = 0; mod < NUM_MODULES; mod++) {
      uint8_t addr = BASE_ADDR + mod;
      uint16_t val = readPCF8575(addr);

      Serial.print("Status Tombol (Modul ");
      Serial.print(mod + 1);
      Serial.print(" - PCF8575 @0x");
      Serial.print(addr, HEX);
      Serial.println("):");

      // tampilkan 16 pin di tiap modul
      for (uint8_t pin = 0; pin < 16; pin++) {
        int state = ((val & (1 << pin)) == 0) ? 1 : 0; // LOW = ditekan
        int globalPin = mod * 16 + pin; // penomoran global 0–127

        Serial.print("P");
        if (globalPin < 10) Serial.print("0");
        if (globalPin < 100 && globalPin >= 10) Serial.print("");
        Serial.print(globalPin);
        Serial.print(":");
        Serial.print(state);
        Serial.print("  ");
        if ((pin + 1) % 8 == 0) Serial.println();
      }
      Serial.println();
    }

    Serial.println("------------------------------------");
  }
}
