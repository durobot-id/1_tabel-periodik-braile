#include "DFRobotDFPlayerMini.h"
#include <HardwareSerial.h>

HardwareSerial mySerial(2);
DFRobotDFPlayerMini df;

void printDetail(uint8_t type, int value);

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=== DIAGNOSA DFPLAYER MINI ===");

  mySerial.begin(9600, SERIAL_8N1, 16, 17);

  if (!df.begin(mySerial)) {
    Serial.println("❌ ERROR: DFPlayer tidak terdeteksi!");
    Serial.println("Kemungkinan:");
    Serial.println("- Wiring RX/TX salah");
    Serial.println("- Tidak ada power");
    Serial.println("- Modul rusak");
    while (true);
  }

  Serial.println("✅ DFPlayer terdeteksi");

  df.volume(20);
  delay(500);

  // Cek SD Card
  int totalFile = df.readFileCounts();

  if (totalFile == -1) {
    Serial.println("❌ ERROR: SD Card tidak terbaca!");
    Serial.println("Periksa:");
    Serial.println("- Format harus FAT32");
    Serial.println("- SD card rusak");
    Serial.println("- Tidak terpasang");
  } else {
    Serial.print("✅ SD Card OK, Total file: ");
    Serial.println(totalFile);
  }

  Serial.println("Tes play file 1...");
  df.play(1);
}

void loop() {
  if (df.available()) {
    printDetail(df.readType(), df.read());
  }
}

void printDetail(uint8_t type, int value) {
  switch (type) {

    case TimeOut:
      Serial.println("❌ Error: Komunikasi Timeout");
      break;

    case WrongStack:
      Serial.println("❌ Error: Stack Salah");
      break;

    case DFPlayerCardInserted:
      Serial.println("SD Card terpasang");
      break;

    case DFPlayerCardRemoved:
      Serial.println("SD Card dilepas");
      break;

    case DFPlayerCardOnline:
      Serial.println("SD Card siap digunakan");
      break;

    case DFPlayerPlayFinished:
      Serial.print("File selesai diputar: ");
      Serial.println(value);
      break;

    case DFPlayerError:
      Serial.print("❌ DFPlayer Error: ");
      switch (value) {
        case Busy:
          Serial.println("Card tidak ditemukan");
          break;
        case Sleeping:
          Serial.println("Modul dalam mode sleep");
          break;
        case SerialWrongStack:
          Serial.println("Kesalahan komunikasi serial");
          break;
        case CheckSumNotMatch:
          Serial.println("Checksum tidak cocok");
          break;
        case FileIndexOut:
          Serial.println("File tidak ditemukan");
          break;
        case FileMismatch:
          Serial.println("File tidak cocok");
          break;
        case Advertise:
          Serial.println("Error advertise");
          break;
        default:
          Serial.println("Error tidak diketahui");
          break;
      }
      break;

    default:
      break;
  }
}
