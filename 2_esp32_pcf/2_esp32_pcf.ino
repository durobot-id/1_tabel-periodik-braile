#include <Arduino.h>
#include <Wire.h>

#define NUM_MODULES 8
#define BASE_ADDR 0x20
#define SPECIAL_ADDR 0x24   // modul yang dikendalikan via ESP32-C3
const unsigned long READ_INTERVAL = 500;
unsigned long lastRead = 0;

// UART1 untuk display/debug
HardwareSerial DisplaySerial(1);
#define DEBUG_TX 17
#define DEBUG_RX 16

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
  // UART0 untuk komunikasi dengan ESP32-C3
  Serial.begin(9600);

  // UART1 untuk tampilan/debug ke PC
  DisplaySerial.begin(115200, SERIAL_8N1, DEBUG_RX, DEBUG_TX);

  Wire.begin();
  Wire.setClock(400000);

  DisplaySerial.println("ESP32 utama siap (UART0=C3, UART1=Display)");

  for (uint8_t i = 0; i < NUM_MODULES; i++) {
    uint8_t addr = BASE_ADDR + i;
    if (addr != SPECIAL_ADDR) {
      writePCF8575(addr, 0xFFFF);
    }
  }
}

void loop() {
  unsigned long now = millis();
  if (now - lastRead >= READ_INTERVAL) {
    lastRead = now;

    DisplaySerial.println("==== Pembacaan PCF8575 ====");
    for (uint8_t mod = 0; mod < NUM_MODULES; mod++) {
      uint8_t addr = BASE_ADDR + mod;
      uint16_t val = 0xFFFF;

      if (addr == SPECIAL_ADDR) {
        // minta data dari ESP32-C3 lewat UART0
        Serial.println("REQ");
        unsigned long start = millis();
        while (Serial.available() < 2 && millis() - start < 200) {
          delay(1);
        }
        if (Serial.available() >= 2) {
          uint8_t low = Serial.read();
          uint8_t high = Serial.read();
          val = ((uint16_t)high << 8) | low;
        } else {
          DisplaySerial.println("⚠️ Timeout baca dari C3!");
        }
      } else {
        val = readPCF8575(addr);
      }

      DisplaySerial.print("Modul ");
      DisplaySerial.print(addr, HEX);
      DisplaySerial.print(": ");
      for (int b = 15; b >= 0; b--) {
        DisplaySerial.print((val & (1 << b)) ? '0' : '1'); // 1 = tombol ditekan
      }
      DisplaySerial.println();
    }
    DisplaySerial.println("===========================\n");
  }
}
