#include <Arduino.h>
#include <Wire.h>

#define PCF_ADDR 0x24

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
  Serial.begin(9600);      // UART0 komunikasi ke ESP32 utama
  Wire.begin();
  Wire.setClock(400000);
  writePCF8575(PCF_ADDR, 0xFFFF);
  Serial.println("ESP32-C3 siap (PCF8575 0x24).");
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd == "REQ") {
      uint16_t val = readPCF8575(PCF_ADDR);
      uint8_t low = val & 0xFF;
      uint8_t high = (val >> 8) & 0xFF;
      Serial.write(low);
      Serial.write(high);
    }
  }
}
