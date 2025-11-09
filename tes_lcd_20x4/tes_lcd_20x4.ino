#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Alamat LCD & ukuran 20x4
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Definisi bentuk hati (5x8 pixel)
// 1 = nyala, 0 = mati
byte heart[8] = {
  0b00000,
  0b01010,
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
  0b00000
};

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("=== ESP32-C3 LCD Custom Character Test ===");

  // Inisialisasi I2C (SDA=8, SCL=9)
  Wire.begin(8, 9);

  // Inisialisasi LCD
  lcd.init();
  lcd.backlight();

  // Buat karakter custom di slot 0
  lcd.createChar(0, heart);

  // Tampilkan teks
  lcd.setCursor(0, 0);
  lcd.print("ESP32-C3 Mini");
  lcd.setCursor(0, 1);
  lcd.print("Custom Char Test");

  // Tampilkan bentuk hati
  lcd.setCursor(0, 2);
  lcd.write(byte(0));   // menampilkan karakter hati
}

void loop() {
  // Animasi kedip hati
  lcd.setCursor(2, 2);
  lcd.write(byte(0));  // tampilkan hati
  delay(500);

  lcd.setCursor(2, 2);
  lcd.print(" ");      // hapus hati
  delay(500);
}
