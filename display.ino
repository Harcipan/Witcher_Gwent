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

void drawActionButton(const char *label) {
  uint16_t color = gameOver ? ILI9341_GREEN : ILI9341_ORANGE;
  tft.fillRoundRect(210, 198, 100, 34, 5, color);
  tft.drawRoundRect(210, 198, 100, 34, 5, ILI9341_WHITE);
  tft.setTextColor(ILI9341_BLACK);
  tft.setTextSize(2);
  int16_t x1, y1;
  uint16_t w, h;
  tft.getTextBounds((char*)label, 0, 0, &x1, &y1, &w, &h);
  tft.setCursor(210 + (100 - w) / 2, 208);
  tft.print(label);
}

void drawPlayerPanel(uint8_t playerIndex, int x, int y, uint16_t color) {
  bool isActive = (!gameOver && activePlayer == playerIndex);
  tft.drawRect(x, y, 145, 112, isActive ? ILI9341_YELLOW : color);
  if (isActive) tft.drawRect(x + 1, y + 1, 143, 110, ILI9341_YELLOW);

  tft.setTextSize(2);
  tft.setTextColor(color);
  tft.setCursor(x + 8, y + 8);
  tft.printf("P%d", playerIndex + 1);

  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(x + 58, y + 8);
  tft.printf("%d", getPlayerScore(playerIndex));

  tft.setTextSize(1);
  tft.setCursor(x + 8, y + 34);
  tft.setTextColor(playerPassed[playerIndex] ? ILI9341_ORANGE : ILI9341_GREEN);
  tft.print(playerPassed[playerIndex] ? "PASSED" : (isActive ? "ACTIVE" : "WAITING"));

  tft.setTextColor(ILI9341_LIGHTGREY);
  tft.setCursor(x + 8, y + 48);
  tft.printf("Cards: %d", playedCount[playerIndex]);

  for (uint8_t i = 0; i < playedCount[playerIndex] && i < 4; i++) {
    const PlayedCard &played = playedCards[playerIndex][i];
    const CardDef &card = cardPool[played.cardIndex];
    int lineY = y + 62 + (i * 11);

    tft.setCursor(x + 8, lineY);
    tft.setTextColor(played.destroyed ? ILI9341_DARKGREY : ILI9341_WHITE);
    tft.printf("%u %s", card.score, card.name);
  }
}

void drawGameScreen() {
  deselectAllSPIDevices();
  tft.fillScreen(ILI9341_BLACK);

  printCentered("GWENT TABLE", 6, ILI9341_YELLOW, 2);

  tft.setTextSize(1);
  tft.setTextColor(ILI9341_CYAN);
  tft.setCursor(10, 28);
  if (gameOver) {
    tft.print("Round complete");
  } else {
    tft.printf("Turn: Player %d    Cards left: %d", activePlayer + 1, countAvailableCards());
  }

  drawPlayerPanel(0, 10, 46, ILI9341_BLUE);
  drawPlayerPanel(1, 165, 46, ILI9341_RED);

  tft.drawRect(10, 166, 300, 24, ILI9341_CYAN);
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(16, 173);
  tft.print(gameMessage);

  tft.setCursor(10, 204);
  tft.setTextColor(ILI9341_LIGHTGREY);
  tft.print("Last: ");
  tft.setTextColor(ILI9341_WHITE);
  tft.print(lastPlayedCardName);

  drawActionButton(gameOver ? "NEW" : "PASS");
}

void drawCardPlayedAnimation(uint8_t playerIndex, const CardDef &card) {
  deselectAllSPIDevices();

  uint16_t frameColor = playerIndex == 0 ? ILI9341_BLUE : ILI9341_RED;
  for (uint8_t i = 0; i < 3; i++) {
    tft.fillRoundRect(82, 48, 156, 126, 8, ILI9341_BLACK);
    tft.drawRoundRect(82, 48, 156, 126, 8, frameColor);
    tft.drawRoundRect(86, 52, 148, 118, 8, ILI9341_WHITE);
    delay(80);

    tft.fillRoundRect(82, 48, 156, 126, 8, frameColor);
    delay(80);
  }

  tft.fillRoundRect(82, 48, 156, 126, 8, ILI9341_BLACK);
  tft.drawRoundRect(82, 48, 156, 126, 8, frameColor);
  tft.drawRoundRect(86, 52, 148, 118, 8, ILI9341_WHITE);

  tft.setTextColor(ILI9341_YELLOW);
  tft.setTextSize(2);
  tft.setCursor(98, 64);
  tft.printf("P%d PLAY", playerIndex + 1);

  tft.setTextSize(2);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(96, 92);
  tft.print(card.name);

  tft.setTextSize(2);
  tft.setTextColor(ILI9341_GREEN);
  tft.setCursor(110, 120);
  tft.printf("Power %u", card.score);

  tft.setTextSize(1);
  tft.setTextColor(ILI9341_CYAN);
  tft.setCursor(98, 150);
  tft.print(abilityName(card.ability));
  delay(800);
}

void drawDestroyAnimation(uint8_t playerIndex, const CardDef &card) {
  deselectAllSPIDevices();

  for (uint8_t i = 0; i < 4; i++) {
    tft.fillScreen(i % 2 == 0 ? ILI9341_RED : ILI9341_BLACK);
    delay(90);
  }

  tft.fillScreen(ILI9341_BLACK);
  printCentered("DESTROYED", 62, ILI9341_RED, 3);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.setCursor(46, 112);
  tft.printf("P%d: %s", playerIndex + 1, card.name);
  delay(800);
}

void drawVictoryAnimation(int winner) {
  deselectAllSPIDevices();

  for (uint8_t i = 0; i < 6; i++) {
    tft.fillScreen(i % 2 == 0 ? ILI9341_DARKGREEN : ILI9341_BLACK);
    delay(120);
  }

  tft.fillScreen(ILI9341_BLACK);
  if (winner < 0) {
    printCentered("DRAW", 52, ILI9341_YELLOW, 4);
  } else {
    char winnerText[20];
    snprintf(winnerText, sizeof(winnerText), "PLAYER %d", winner + 1);
    printCentered(winnerText, 42, ILI9341_YELLOW, 3);
    printCentered("WINS!", 86, ILI9341_GREEN, 4);
  }

  tft.setTextSize(2);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(62, 142);
  tft.printf("%d  -  %d", getPlayerScore(0), getPlayerScore(1));
  drawActionButton("NEW");
}