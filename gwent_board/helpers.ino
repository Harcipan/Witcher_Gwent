void deselectAllSPIDevices() {
  digitalWrite(TFT_CS, HIGH);
  digitalWrite(SD_CS, HIGH);
  digitalWrite(TOUCH_CS, HIGH);
  digitalWrite(RFID1_SS, HIGH);
  digitalWrite(RFID2_SS, HIGH);
}

bool validRC522Version(byte v) {
  return (v == 0x91 || v == 0x92 || v == 0x90 || v == 0x88 || v == 0xB2);
}

void printCentered(const char *text, int y, uint16_t color, uint8_t size) {
  int16_t x1, y1;
  uint16_t w, h;
  tft.setTextSize(size);
  tft.getTextBounds((char*)text, 0, y, &x1, &y1, &w, &h);
  int x = (tft.width() - w) / 2;
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.print(text);
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

String uidToString(MFRC522::Uid *uid) {
  String s = "";
  for (byte i = 0; i < uid->size; i++) {
    if (uid->uidByte[i] < 0x10) s += "0";
    s += String(uid->uidByte[i], HEX);
    if (i < uid->size - 1) s += ":";
  }
  s.toUpperCase();
  return s;
}

uint32_t currentTouchPollInterval() {
  return audioPlaying ? TOUCH_POLL_INTERVAL_AUDIO_MS : TOUCH_POLL_INTERVAL_IDLE_MS;
}

uint32_t currentTouchDrawInterval() {
  return audioPlaying ? TOUCH_DRAW_INTERVAL_AUDIO_MS : TOUCH_DRAW_INTERVAL_IDLE_MS;
}

uint32_t currentRFIDPollInterval() {
  return audioPlaying ? RFID_POLL_INTERVAL_AUDIO_MS : RFID_POLL_INTERVAL_IDLE_MS;
}

uint32_t currentRFIDDrawInterval() {
  return audioPlaying ? RFID_DRAW_INTERVAL_AUDIO_MS : RFID_DRAW_INTERVAL_IDLE_MS;
}