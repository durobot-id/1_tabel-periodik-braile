#include <Arduino.h>
#include <Wire.h>

#define PCF1_ADDR 0x20  // modul 1
#define PCF2_ADDR 0x21  // modul 2

const unsigned long READ_INTERVAL = 200;
unsigned long lastRead = 0;

void handlePin(uint8_t modul, uint8_t pin) {
  int modulIndex = modul - 0x20; // 0 untuk 0x20, 1 untuk 0x21, ... 7 untuk 0x27

  Serial.print(">>> Modul ");
  Serial.print(modulIndex);
  Serial.print(" - P");
  Serial.print(pin);
  Serial.println(" ditekan -> jalankan fungsi terkait");
}

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }

  Wire.begin();         
  Wire.setClock(400000);

  // set semua pin ke input pullup
  writePCF8575(PCF1_ADDR, 0xFFFF);
  writePCF8575(PCF2_ADDR, 0xFFFF);

  Serial.println("ESP32 siap membaca PCF8575 (0x20 & 0x21) semua pin dengan if else");
}

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

void loop() {
  unsigned long now = millis();
  if (now - lastRead >= READ_INTERVAL) {
    lastRead = now;

    uint16_t val1 = readPCF8575(PCF1_ADDR);
    uint16_t val2 = readPCF8575(PCF2_ADDR);

    // cek semua pin di modul 1 (0x20)
    for (uint8_t pin = 0; pin < 16; pin++) {
      if ((val1 & (1 << pin)) == 0) { // jika LOW (tombol ditekan)
        handlePin(PCF1_ADDR, pin);
      }
    }

    // cek semua pin di modul 2 (0x21)
    for (uint8_t pin = 0; pin < 16; pin++) {
      if ((val2 & (1 << pin)) == 0) { // jika LOW (tombol ditekan)
        handlePin(PCF2_ADDR, pin);
      }
    }
  }
}
