#include <Arduino.h>
#include <Wire.h>
#include "DFRobotDFPlayerMini.h"

//
// ---- Konfigurasi PCF8575 ----
//
#define PCF1_ADDR 0x20  // modul 1
#define PCF2_ADDR 0x25  // modul 2

const unsigned long READ_INTERVAL = 200;
unsigned long lastRead = 0;

//
// ---- Konfigurasi DFPlayer ----
//
#define RXD2 16   // ESP32 RX -> TX DFPlayer
#define TXD2 17   // ESP32 TX -> RX DFPlayer

HardwareSerial mySerial(2);
DFRobotDFPlayerMini myDFPlayer;

//
// ---- Fungsi PCF8575 ----
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
// ---- Handle Tombol ----
//
void handlePin(uint8_t modul, uint8_t pin) {
  int modulIndex = modul - 0x20; // 0 untuk 0x20, 1 untuk 0x21, dst.

  Serial.print(">>> Modul ");
  Serial.print(modulIndex);
  Serial.print(" - P");
  Serial.print(pin);
  Serial.println(" ditekan.");

  myDFPlayer.playFolder(modulIndex, pin);  //  /01/0001.mp3
}

//
// ---- Setup ----
//
void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }

  // I2C untuk PCF8575
  Wire.begin();
  Wire.setClock(400000);
  writePCF8575(PCF1_ADDR, 0xFFFF); // semua input pullup
  writePCF8575(PCF2_ADDR, 0xFFFF);

  Serial.println("ESP32 siap membaca PCF8575 dan mengontrol DFPlayer");

  // UART untuk DFPlayer
  mySerial.begin(9600, SERIAL_8N1, RXD2, TXD2);
  delay(1000);

  Serial.println("Inisialisasi DFPlayer...");
  if (!myDFPlayer.begin(mySerial)) {
    Serial.println("❌ DFPlayer tidak terhubung!");
    while (true);
  }
  Serial.println("✅ DFPlayer online.");

  myDFPlayer.volume(25); // Volume 0–30
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
}

//
// ---- Loop ----
//
void loop() {
  unsigned long now = millis();
  if (now - lastRead >= READ_INTERVAL) {
    lastRead = now;

    uint16_t val1 = readPCF8575(PCF1_ADDR);
    uint16_t val2 = readPCF8575(PCF2_ADDR);

    // cek semua pin di modul 1 (0x20)
    for (uint8_t pin = 0; pin < 16; pin++) {
      if ((val1 & (1 << pin)) == 0) { // LOW = tombol ditekan
        handlePin(PCF1_ADDR, pin);
      }
    }

    // cek semua pin di modul 2 (0x21)
    for (uint8_t pin = 0; pin < 16; pin++) {
      if ((val2 & (1 << pin)) == 0) { // LOW = tombol ditekan
        handlePin(PCF2_ADDR, pin);
      }
    }
  }

  // Cek event DFPlayer (opsional)
  if (myDFPlayer.available()) {
    uint8_t type = myDFPlayer.readType();
    int value = myDFPlayer.read();
    if (type == DFPlayerPlayFinished) {
      Serial.print("Track selesai: ");
      Serial.println(value);
    }
  }
}
