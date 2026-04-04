static String displayedUID1 = "";
static String displayedUID2 = "";
static bool displayedRFID1Ok = false;
static bool displayedRFID2Ok = false;
static bool rfidStatusFrameDrawn = false;

static int displayedTouchRawX = -1;
static int displayedTouchRawY = -1;
static int displayedTouchZ = -1;

void drawRFIDStatus() {
  deselectAllSPIDevices();

  if (!rfidStatusFrameDrawn) {
    tft.fillRect(10, 156, 300, 74, ILI9341_BLACK);
    tft.drawRect(10, 156, 300, 74, ILI9341_BLUE);
    rfidStatusFrameDrawn = true;
  }

  tft.setTextSize(1);

  if (displayedRFID1Ok != rfid1Ok || displayedUID1 != lastUID1) {
    tft.fillRect(18, 164, 284, 24, ILI9341_BLACK);

    tft.setCursor(18, 164);
    tft.setTextColor(ILI9341_WHITE);
    tft.print("RFID1: ");
    tft.setTextColor(rfid1Ok ? ILI9341_GREEN : ILI9341_RED);
    tft.print(rfid1Ok ? "OK" : "FAIL");

    tft.setCursor(18, 176);
    tft.setTextColor(ILI9341_WHITE);
    tft.print("UID1: ");
    tft.print(lastUID1);
  }

  if (displayedRFID2Ok != rfid2Ok || displayedUID2 != lastUID2) {
    tft.fillRect(18, 196, 284, 24, ILI9341_BLACK);

    tft.setCursor(18, 196);
    tft.setTextColor(ILI9341_WHITE);
    tft.print("RFID2: ");
    tft.setTextColor(rfid2Ok ? ILI9341_GREEN : ILI9341_RED);
    tft.print(rfid2Ok ? "OK" : "FAIL");

    tft.setCursor(18, 208);
    tft.setTextColor(ILI9341_WHITE);
    tft.print("UID2: ");
    tft.print(lastUID2);
  }

  displayedRFID1Ok = rfid1Ok;
  displayedRFID2Ok = rfid2Ok;
  displayedUID1 = lastUID1;
  displayedUID2 = lastUID2;
}

void drawStatusScreen() {
  deselectAllSPIDevices();
  tft.fillScreen(ILI9341_BLACK);

  rfidStatusFrameDrawn = false;
  displayedRFID1Ok = !rfid1Ok;
  displayedRFID2Ok = !rfid2Ok;
  displayedUID1 = "";
  displayedUID2 = "";
  displayedTouchRawX = -1;
  displayedTouchRawY = -1;
  displayedTouchZ = -1;

  printCentered("GWENT TABLE TEST", 8, ILI9341_YELLOW, 2);

  tft.drawRect(10, 34, 300, 70, ILI9341_WHITE);
  tft.setTextSize(2);

  tft.setCursor(18, 42);
  tft.setTextColor(sdOk ? ILI9341_GREEN : ILI9341_RED);
  tft.print("SD: ");
  tft.print(sdOk ? "OK" : "FAIL");

  tft.setCursor(18, 62);
  tft.setTextColor(touchOk ? ILI9341_GREEN : ILI9341_RED);
  tft.print("TOUCH: ");
  tft.print(touchOk ? "OK" : "FAIL");

  tft.setCursor(18, 82);
  tft.setTextColor(audioOk ? ILI9341_GREEN : ILI9341_RED);
  tft.print("AUDIO: ");
  tft.print(audioOk ? "OK" : "FAIL");

  tft.drawRect(10, 112, 300, 34, ILI9341_CYAN);
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(18, 122);
  tft.print("Touch screen or present tags to RFID reader 1 / 2");

  drawRFIDStatus();
}

void showTouchData(int rawX, int rawY, int z) {
  int screenX = map(rawX, touchMinX, touchMaxX, tft.width() - 1, 0);
  int screenY = map(rawY, touchMinY, touchMaxY, tft.height() - 1, 0);

  screenX = constrain(screenX, 0, tft.width() - 1);
  screenY = constrain(screenY, 0, tft.height() - 1);

  if (screenX == lastTouchScreenX &&
      screenY == lastTouchScreenY &&
      rawX == displayedTouchRawX &&
      rawY == displayedTouchRawY &&
      z == displayedTouchZ) {
    return;
  }

  deselectAllSPIDevices();
  if (rawX != displayedTouchRawX || rawY != displayedTouchRawY || z != displayedTouchZ) {
    tft.fillRect(150, 112, 155, 20, ILI9341_BLACK);
    tft.setTextColor(ILI9341_GREEN);
    tft.setTextSize(1);
    tft.setCursor(155, 118);
    tft.printf("X:%d Y:%d Z:%d", rawX, rawY, z);
  }

  if ((screenX != lastTouchScreenX || screenY != lastTouchScreenY) &&
      lastTouchScreenX >= 0 && lastTouchScreenY >= 0) {
    tft.fillCircle(lastTouchScreenX, lastTouchScreenY, 2, ILI9341_BLACK);
  }
  if (screenX != lastTouchScreenX || screenY != lastTouchScreenY) {
    tft.fillCircle(screenX, screenY, 2, ILI9341_RED);
  }

  lastTouchScreenX = screenX;
  lastTouchScreenY = screenY;
  displayedTouchRawX = rawX;
  displayedTouchRawY = rawY;
  displayedTouchZ = z;
}

void drawPlayMessage(const char* msg, uint16_t color) {
  deselectAllSPIDevices();
  tft.fillRect(10, 112, 300, 34, ILI9341_BLACK);
  tft.drawRect(10, 112, 300, 34, ILI9341_CYAN);
  tft.setTextColor(color);
  tft.setTextSize(2);
  tft.setCursor(18, 120);
  tft.print(msg);
}