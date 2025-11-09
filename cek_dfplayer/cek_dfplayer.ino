#include "Arduino.h"
#include "DFRobotDFPlayerMini.h"

HardwareSerial mySerial(1);  // Gunakan UART1 di ESP32
DFRobotDFPlayerMini myDFPlayer;

void setup()
{
  Serial.begin(115200);     // Monitor Serial
  mySerial.begin(9600, SERIAL_8N1, 16, 17);  
  // format: baudrate, mode, RX, TX
  // Misal: ESP32 RX=16, TX=17 -> sesuaikan dengan rangkaianmu

  Serial.println("Mencoba koneksi ke DFPlayer...");

  if (!myDFPlayer.begin(mySerial)) {  
    Serial.println("Gagal: Tidak ada respon dari DFPlayer!");
    while(true);
  }
  
  Serial.println("DFPlayer berhasil terhubung!");
  myDFPlayer.volume(30);  // Set volume 0-30
  myDFPlayer.play(1);     // Coba play file pertama di SD card
}

void loop()
{
  // Bisa dipakai untuk cek status
  if (myDFPlayer.available()) {
    uint8_t type = myDFPlayer.readType();
    int value = myDFPlayer.read();
    Serial.print("Respon DFPlayer -> Type: ");
    Serial.print(type);
    Serial.print(" Value: ");
    Serial.println(value);
  }
}
