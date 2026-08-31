bool initTouch() {
  deselectAllSPIDevices();
  delay(10);

  ts.begin(SPI);
  ts.setRotation(1);
  return true;
}

void initRFIDReaders() {
  deselectAllSPIDevices();
  delay(10);

  rfid1.PCD_Init();
  delay(50);
  byte v1 = rfid1.PCD_ReadRegister(MFRC522::VersionReg);
  rfid1Ok = validRC522Version(v1);

  deselectAllSPIDevices();
  delay(10);

  rfid2.PCD_Init();
  delay(50);
  byte v2 = rfid2.PCD_ReadRegister(MFRC522::VersionReg);
  rfid2Ok = validRC522Version(v2);

  Serial.printf("RFID1 VersionReg: 0x%02X -> %s\n", v1, rfid1Ok ? "OK" : "FAIL");
  Serial.printf("RFID2 VersionReg: 0x%02X -> %s\n", v2, rfid2Ok ? "OK" : "FAIL");
}

bool pollRFID(MFRC522 &reader, String &lastUID, const char *name) {
  deselectAllSPIDevices();

  if (!reader.PICC_IsNewCardPresent()) return false;
  if (!reader.PICC_ReadCardSerial())   return false;

  lastUID = uidToString(&reader.uid);

  Serial.print(name);
  Serial.print(" UID: ");
  Serial.println(lastUID);

  reader.PICC_HaltA();
  reader.PCD_StopCrypto1();
  return true;
}