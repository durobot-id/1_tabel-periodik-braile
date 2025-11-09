#include <Arduino.h>
#include <Wire.h>

// daftar alamat PCF8575 yang dipakai (0x20 s/d 0x25)
const uint8_t PCF_ADDR[] = {0x20, 0x21, 0x22, 0x23, 0x24, 0x25};
const uint8_t PCF_COUNT = sizeof(PCF_ADDR) / sizeof(PCF_ADDR[0]);

const unsigned long READ_INTERVAL = 200;
unsigned long lastRead = 0;

void handlePin(uint8_t modulAddr, uint8_t pin) {
  int modulIndex = (modulAddr - 0x20 )+ 1; // 1 untuk 0x20, 2 untuk 0x21, dst

  Serial.print(">>> Modul ");
  Serial.print(modulIndex);
  Serial.print(" (0x");
  Serial.print(modulAddr, HEX);
  Serial.print(") - P");
  Serial.print(pin);
  Serial.println(" ditekan -> jalankan fungsi terkait");
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

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }

  Wire.begin();         
  Wire.setClock(400000);

  // set semua pin semua modul ke input pullup
  for (uint8_t i = 0; i < PCF_COUNT; i++) {
    writePCF8575(PCF_ADDR[i], 0xFFFF);
  }

  Serial.println("ESP32 siap membaca semua PCF8575 (0x20 s/d 0x25)");
}

void loop() {
  unsigned long now = millis();
  if (now - lastRead >= READ_INTERVAL) {
    lastRead = now;

    // loop semua modul
    for (uint8_t m = 0; m < PCF_COUNT; m++) {
      uint8_t addr = PCF_ADDR[m];
      uint16_t val = readPCF8575(addr);

      // cek semua pin di modul
      for (uint8_t pin = 0; pin < 16; pin++) {
        if ((val & (1 << pin)) == 0) { // jika LOW (tombol ditekan)
          handlePin(addr, pin);
        }
      }
    }
  }
}
