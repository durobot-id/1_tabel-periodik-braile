#include <Arduino.h>
#include <Wire.h>
#include "DFRobotDFPlayerMini.h"

//
// ---- Konfigurasi PCF8575 ----
//
#define NUM_MODULES 8
#define BASE_ADDR  0x20
const unsigned long READ_INTERVAL = 50; // ms antar baca
unsigned long lastRead = 0;

uint16_t lastState[NUM_MODULES];

// Status tiap tombol (apakah sedang ditekan)
bool isPressed[NUM_MODULES][16] = {false};

//
// ---- DFPlayer Mini ----
//
HardwareSerial mySerial(1);
DFRobotDFPlayerMini myDFPlayer;

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

int computeIndexFromString(const char* nilai) {
  int len = strlen(nilai);
  if (len < 3) return -1; // error: input terlalu pendek

  // ambil dua digit terakhir sebagai integer (mirip int(nilai[-2:]) di Python)
  int lastTwo = atoi(nilai + len - 2); // "03" -> 3, "12" -> 12

  // aturan: jika > 7 maka dikurangi 2
  int angka;
  if (lastTwo > 7) {
    angka = lastTwo - 2;
  } else {
    angka = lastTwo;
  }

  // ambil karakter ke-3 dari belakang sebagai digit tunggal (mirip int(nilai[-3]))
  char ch = nilai[len - 3];
  int digitTunggal = ch - '0'; // pastikan ch adalah '0'..'9'
  if (digitTunggal < 0 || digitTunggal > 9) return -1; // error: bukan digit

  // hasil akhir
  int index = (digitTunggal * 16) + angka + 1;
  return index;
}

//
// ---- Setup ----
//
void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }

  Wire.begin();
  Wire.setClock(400000);

  // Inisialisasi PCF8575
  for (uint8_t i = 0; i < NUM_MODULES; i++) {
    uint8_t addr = BASE_ADDR + i;
    writePCF8575(addr, 0xFFFF); // input pull-up
    lastState[i] = readPCF8575(addr);
  }

  Serial.println("=== MONITOR MULTI-TOMBOL + DFPlayer (Proteksi Per Tombol) ===");

  // Inisialisasi DFPlayer Mini
  mySerial.begin(9600, SERIAL_8N1, 16, 17); // RX=16, TX=17
  Serial.println("Menghubungkan ke DFPlayer...");

  if (!myDFPlayer.begin(mySerial)) {
    Serial.println("Gagal: Tidak ada respon dari DFPlayer!");
    while (true);
  }

  Serial.println("DFPlayer berhasil terhubung!");
  myDFPlayer.volume(30);
}

//
// ---- Loop ----
//
void loop() {
  unsigned long now = millis();
  if (now - lastRead >= READ_INTERVAL) {
    lastRead = now;

    for (uint8_t mod = 0; mod < NUM_MODULES; mod++) {
      uint8_t addr = BASE_ADDR + mod;
      uint16_t val = readPCF8575(addr);

      for (uint8_t bit = 0; bit < 16; bit++) {
        bool pressedNow = ((val & (1 << bit)) == 0); // aktif low

        // 🔹 Jika baru ditekan (dan sebelumnya belum ditekan)
        if (pressedNow && !isPressed[mod][bit]) {
          isPressed[mod][bit] = true;

          int pinLabel = (bit < 8) ? bit : bit + 2;
          char index[4];
          snprintf(index, sizeof(index), "%d%02d", mod, pinLabel);

          // Serial.print("Tombol baru ditekan: ");
          Serial.println(index);

          // Konversi ke integer untuk nomor lagu
          // int songNumber = atoi(index);
          int idx = computeIndexFromString(index);
          // Serial.println(idx);

          // Serial.print("Memutar lagu nomor ");
          // Serial.println(songNumber);
          myDFPlayer.play(idx);
        }
        // 🔹 Jika tombol dilepas, reset flag
        else if (!pressedNow && isPressed[mod][bit]) {
          isPressed[mod][bit] = false;
        }
      }

      lastState[mod] = val;
    }
  }

  // // Opsional: pantau respon DFPlayer
  // if (myDFPlayer.available()) {
  //   uint8_t type = myDFPlayer.readType();
  //   int value = myDFPlayer.read();
  //   Serial.print("Respon DFPlayer -> Type: ");
  //   Serial.print(type);
  //   Serial.print(" Value: ");
  //   Serial.println(value);
  // }
}
