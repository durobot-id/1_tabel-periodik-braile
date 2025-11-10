#include <Wire.h>
#include <PCF8575.h>

#define MAX_MODULES 8   // batas maksimal jumlah modul yang bisa dibaca
PCF8575* modules[MAX_MODULES];
uint8_t addresses[MAX_MODULES];
uint8_t moduleCount = 0;
bool initialized = false;

void scanI2C() {
  Serial.println("\n=== I2C Scanner ===");
  byte error, address;
  int nDevices = 0;

  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Device ditemukan pada alamat 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println(" !");
      nDevices++;
    }
  }

  if (nDevices == 0)
    Serial.println("Tidak ada device I2C ditemukan");
  Serial.println("====================\n");
}

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); // SDA, SCL

  Serial.println();
  Serial.println("=== Tes Multi Modul PCF8575 ===");
  Serial.println("Ketik alamat modul, contoh:");
  Serial.println("  0x20;0x21;0x22;");
  Serial.println("Atau ketik: scan");
  Serial.print("> ");
}

void parseAddresses(String input) {
  moduleCount = 0;
  input.trim();

  while (input.length() > 0 && moduleCount < MAX_MODULES) {
    int sep = input.indexOf(';');
    String addrStr = (sep >= 0) ? input.substring(0, sep) : input;

    addrStr.trim();
    if (addrStr.length() > 0) {
      uint8_t addr;
      if (addrStr.startsWith("0x") || addrStr.startsWith("0X"))
        addr = strtol(addrStr.c_str(), NULL, 16);
      else
        addr = strtol(addrStr.c_str(), NULL, 10);

      addresses[moduleCount++] = addr;
    }

    if (sep < 0) break;
    input = input.substring(sep + 1);
  }
}

void initModules() {
  for (int i = 0; i < moduleCount; i++) {
    modules[i] = new PCF8575(addresses[i]);
    Serial.print("Inisialisasi PCF8575 @ 0x");
    Serial.print(addresses[i], HEX);
    Serial.print(" ... ");

    if (!modules[i]->begin()) {
      Serial.println("❌ Gagal!");
    } else {
      Serial.println("✅ OK");
      modules[i]->write16(0xFFFF); // set semua pin input
    }
  }

  Serial.println("\nSemua modul siap.\n");
}

void loop() {
  if (!initialized) {
    if (Serial.available()) {
      String input = Serial.readStringUntil('\n');
      input.trim();

      if (input.equalsIgnoreCase("scan")) {
        scanI2C();
        Serial.print("> Masukkan alamat modul: ");
        return;
      }

      parseAddresses(input);

      if (moduleCount == 0) {
        Serial.println("⚠️  Tidak ada alamat valid!");
        Serial.print("> ");
        return;
      }

      Serial.print("Ditemukan ");
      Serial.print(moduleCount);
      Serial.println(" alamat:");

      for (int i = 0; i < moduleCount; i++) {
        Serial.print("  - 0x");
        Serial.println(addresses[i], HEX);
      }

      initModules();
      initialized = true;
      Serial.println("Mulai monitoring...\n");
    }
    return;
  }

  // === MODE OPERASI ===
  for (int i = 0; i < moduleCount; i++) {
    uint16_t val = modules[i]->read16();

    Serial.print("[0x");
    Serial.print(addresses[i], HEX);
    Serial.print("] P15..P0: ");
    for (int b = 15; b >= 0; b--) {
      Serial.print(bitRead(val, b));
    }
    Serial.print(" (0x");
    Serial.print(val, HEX);
    Serial.println(")");
  }

  Serial.println("------------------------");
  delay(1000);
}
