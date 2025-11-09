#include "DFRobotDFPlayerMini.h"

// Gunakan UART2 di ESP32
// Pin bisa diganti sesuai kebutuhan
#define RXD2 16   // ESP32 RX ke TX DFPlayer
#define TXD2 17   // ESP32 TX ke RX DFPlayer

HardwareSerial mySerial(2);  // gunakan UART2
DFRobotDFPlayerMini myDFPlayer;

boolean isPlaying = false;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600, SERIAL_8N1, RXD2, TXD2);  // UART2 inisialisasi

  delay(1000);

  Serial.println();
  Serial.println("DFPlayer Mini Demo");
  Serial.println("Initializing DFPlayer...");

  if (!myDFPlayer.begin(mySerial)) {
      Serial.println("Unable to begin:");
      Serial.println("1. Please recheck the connection!");
      Serial.println("2. Please insert the SD card!");
      while (true);
  }
  Serial.println("DFPlayer Mini online.");

  myDFPlayer.setTimeOut(500);

  //----Set volume----
  myDFPlayer.volume(30); //Set volume value (0~30).

  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  isPlaying = true;
  Serial.println("Playing..");

  myDFPlayer.play(1);  // langsung play file 0001.mp3
}

void loop() {
  // nanti bisa tambah kontrol tombol untuk next/prev
}
