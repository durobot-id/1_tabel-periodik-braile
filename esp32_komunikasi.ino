// === ESP32 Manual Sender ===
// Ketik string di Serial Monitor (contoh: 003) → dikirim ke ESP32-C3

String inputData = "";

void setup() {
  Serial.begin(9600);                       // Debug dan input via USB
  // Serial2.begin(9600, SERIAL_8N1, 16, 17);    // UART ke ESP32-C3 (RX=16, TX=17)
  delay(1000);

  Serial.println("=== ESP32 Periodic Table Sender ===");
  Serial.println("Ketik kode index (mis. 003) lalu tekan [ENTER]...");
}

void loop() {
  // Cek apakah ada input dari Serial Monitor
  if (Serial.available()) {
    inputData = Serial.readStringUntil('\n');  // baca sampai enter ditekan
    inputData.trim();                          // hilangkan spasi dan newline

    // Jika string tidak kosong
    if (inputData.length() > 0) {
      // Kirim ke ESP32-C3 lewat UART
      Serial.println(inputData);
      
      // Serial.print("📤 Dikirim ke ESP32-C3: ");
      // Serial.println(inputData);
    }
  }
}
