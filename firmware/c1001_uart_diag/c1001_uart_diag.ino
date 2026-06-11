/**
 * C1001 (SEN0623) UART diagnostic for ESP32-S3
 * --------------------------------------------------------------
 * Purpose: figure out WHY hu.begin() fails ("init error").
 * It sends the sensor's "query work mode" frame and dumps any reply,
 * automatically trying BOTH TX/RX wirings so you don't have to guess.
 *
 * Wiring under test:
 *   C1001 VCC -> ESP32-S3 5V   (MUST be 5V, not 3V3!)
 *   C1001 GND -> ESP32-S3 GND
 *   C1001 TX  -> one of GPIO18 / GPIO17
 *   C1001 RX  -> the other one
 *
 * Expected good reply starts with 0x53 0x59 ... ends 0x54 0x43.
 */

// Query-work-mode frame: 53 59 02 A8 00 01 0F [checksum=66] 54 43
const uint8_t QUERY[] = {0x53, 0x59, 0x02, 0xA8, 0x00, 0x01, 0x0F, 0x66, 0x54, 0x43};

void dumpReply(int rxPin, int txPin) {
  Serial1.end();
  delay(50);
  Serial1.begin(115200, SERIAL_8N1, rxPin, txPin);
  delay(100);
  while (Serial1.available()) Serial1.read();   // flush

  Serial.printf("\n--- Testing RX=GPIO%d  TX=GPIO%d ---\n", rxPin, txPin);
  int totalBytes = 0;
  for (int attempt = 0; attempt < 4; attempt++) {
    Serial1.write(QUERY, sizeof(QUERY));
    Serial1.flush();
    unsigned long t0 = millis();
    while (millis() - t0 < 500) {
      while (Serial1.available()) {
        uint8_t b = Serial1.read();
        Serial.printf("%02X ", b);
        totalBytes++;
      }
    }
  }
  if (totalBytes == 0)
    Serial.println("  -> NO bytes received on this orientation.");
  else
    Serial.printf("\n  -> Received %d bytes. If it starts with 53 59, THIS wiring is correct!\n", totalBytes);
}

void setup() {
  Serial.begin(115200);
  delay(12000);  // give the C1001 its ~10s internal boot time + margin
  Serial.println("\n===== C1001 UART DIAGNOSTIC =====");
  Serial.println("If BOTH orientations show 0 bytes -> power/ground/wrong-pin problem,");
  Serial.println("not a TX/RX swap. Check: sensor on 5V, common GND, correct GPIOs.\n");
}

void loop() {
  dumpReply(18, 17);   // orientation A
  delay(500);
  dumpReply(17, 18);   // orientation B (swapped)
  Serial.println("\n=================================\n");
  delay(3000);
}
