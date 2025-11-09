/*
  ESP32 — Membaca dan menulis ke PCF8575 (I2C I/O expander)
  Alamat I2C: 0x20

  Koneksi contoh:
    PCF8575 VCC -> 3.3V
    PCF8575 GND -> GND
    PCF8575 SDA -> ESP32 SDA (default GPIO21)
    PCF8575 SCL -> ESP32 SCL (default GPIO22)
    (pastikan pull-up 10k pada SDA dan SCL jika diperlukan)

  Cara kerja singkat:
    - Fungsi readPCF8575() meminta 2 byte dari alamat 0x20 dan menggabungkannya
      menjadi uint16_t (bit 0 = P0, bit 15 = P15).
    - Fungsi writePCF8575(value) menulis 2 byte ke device untuk mengatur output.
      Pada PCF8575, menulis bit=1 membiarkan pin dalam keadaan high (input/quasi-hi),
      menulis bit=0 mendorong pin ke low (output low).
    - Untuk setting semua pin menjadi INPUT PULLUP: tulis 0xFFFF (semua bit = 1).

  Compile: board ESP32 (Arduino core) atau platform yang kompatibel.
*/

#include <Arduino.h>
#include <Wire.h>

#define PCF_ADDR 0x20 // alamat I2C PCF8575 (sesuaikan jika A0..A2 tersetting lain)

// Waktu antar pembacaan (ms)
const unsigned long READ_INTERVAL = 200;
unsigned long lastRead = 0;

// Nilai yang terakhir ditulis ke PCF8575 (semua HIGH = input pullup)
uint16_t lastWriteValue = 0xFFFF;

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }

  Wire.begin();         // gunakan SDA=21, SCL=22 secara default pada ESP32
  Wire.setClock(400000); // I2C 400kHz (opsional)

  Serial.println("ESP32 -> PCF8575 (0x20) semua pin INPUT PULLUP");

  // Set semua pin ke input pullup (bit=1)
  if (!writePCF8575(0xFFFF)) {
    Serial.println("Gagal inisialisasi PCF8575");
  }
}

// Membaca 16-bit dari PCF8575
uint16_t readPCF8575() {
  uint16_t value = 0xFFFF; // default jika gagal

  Wire.requestFrom(PCF_ADDR, (uint8_t)2);
  if (Wire.available() >= 2) {
    uint8_t low = Wire.read();
    uint8_t high = Wire.read();
    value = ((uint16_t)high << 8) | low; // low byte dulu, kemudian high byte
  } else {
    // Jika gagal, bisa ditangani di caller
  }
  return value;
}

// Menulis 16-bit ke PCF8575
bool writePCF8575(uint16_t value) {
  uint8_t low = value & 0xFF;
  uint8_t high = (value >> 8) & 0xFF;

  Wire.beginTransmission(PCF_ADDR);
  Wire.write(low);
  Wire.write(high);
  uint8_t err = Wire.endTransmission();

  return (err == 0);
}

// Cetak nilai 16-bit sebagai biner (P15..P0)
void printBits16(uint16_t v) {
  for (int i = 15; i >= 0; --i) {
    Serial.print((v >> i) & 1);
    if (i % 4 == 0 && i != 0) Serial.print(' '); // pemisah setiap 4 bit
  }
}

void loop() {
  unsigned long now = millis();
  if (now - lastRead >= READ_INTERVAL) {
    lastRead = now;

    uint16_t val = readPCF8575();
    Serial.print("PCF8575 raw (P15..P0): ");
    printBits16(val);
    Serial.print("   (hex=0x");
    Serial.print(val, HEX);
    Serial.println(")");

    delay(1); // jeda singkat sebelum pembacaan berikutnya
  }
}
