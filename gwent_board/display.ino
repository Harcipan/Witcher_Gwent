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
  tft.printf("Hand:%u Played:%u", handCount[playerIndex], playedCount[playerIndex]);

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
    tft.print("Game over");
  } else {
    tft.printf("P%d turn  Round %d  Wins:%d-%d", activePlayer + 1, roundNumber + 1, roundWins[0], roundWins[1]);
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

void drawDrawScreen() {
  deselectAllSPIDevices();
  tft.fillScreen(ILI9341_BLACK);

  char title[28];
  snprintf(title, sizeof(title), "DRAW - Round %u of 3", roundNumber + 1);
  printCentered(title, 6, ILI9341_YELLOW, 2);

  for (uint8_t p = 0; p < 2; p++) {
    int x = (p == 0) ? 10 : 165;
    uint16_t col = (p == 0) ? ILI9341_BLUE : ILI9341_RED;
    if (drawsNeeded[p] == 0) col = ILI9341_DARKGREY;

    tft.drawRect(x, 36, 145, 112, col);
    tft.setTextColor(col);
    tft.setTextSize(2);
    tft.setCursor(x + 8, 44);
    tft.printf("P%u", p + 1);

    tft.setTextSize(1);
    tft.setCursor(x + 8, 68);
    if (drawsNeeded[p] > 0) {
      tft.setTextColor(ILI9341_WHITE);
      tft.printf("Scan %u more", drawsNeeded[p]);
    } else {
      tft.setTextColor(ILI9341_GREEN);
      tft.print("Done!");
    }

    tft.setTextColor(ILI9341_LIGHTGREY);
    tft.setCursor(x + 8, 82);
    tft.printf("Hand: %u cards", handCount[p]);

    tft.setTextColor(ILI9341_CYAN);
    tft.setCursor(x + 8, 96);
    char nameBuf[18];
    strncpy(nameBuf, lastDrawnCardName[p].c_str(), 17);
    nameBuf[17] = '\0';
    tft.print(nameBuf);
  }

  tft.drawRect(10, 156, 300, 34, ILI9341_CYAN);
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(16, 168);
  tft.print(gameMessage);

  drawActionButton("SKIP");
}

static const char *CARD_ANIMATION_ROOT = "/Cards/converted_flip_magic_320x240";
static const uint16_t ANIMATION_WIDTH = 320;
static const uint16_t ANIMATION_HEIGHT = 240;
static const uint16_t ANIMATION_FRAME_INTERVAL_MS = 66;
static const uint8_t MAX_ANIMATION_FRAMES = 30;

// Some names used by the game are shorter or omit accents found in the asset folders.
static const char *animationAssetAlias(const char *cardName) {
  if (strcmp(cardName, "Ciri") == 0) return "Cirilla Fiona Elen Riannon";
  if (strcmp(cardName, "Emiel Regis") == 0) return "Emiel Regis Rohellec Terzieff";
  if (strcmp(cardName, "Sile de Tansarville") == 0) return "S\xC3\xADle de Tansarville";
  if (strcmp(cardName, "Eithne") == 0) return "Eithn\xC3\xA9";
  if (strcmp(cardName, "Schirru") == 0) return "Schirr\xC3\xBA";
  return nullptr;
}

static bool animationDirectoryExists(const String &directory) {
  String firstFrame = directory + "/f000.bin";
  deselectAllSPIDevices();
  bool exists = SD.exists(firstFrame.c_str());
  deselectAllSPIDevices();
  return exists;
}

static bool tryAnimationAssetName(const String &assetName, String &directory) {
  String candidate = String(CARD_ANIMATION_ROOT) + "/" + assetName;
  if (animationDirectoryExists(candidate)) {
    directory = candidate;
    return true;
  }

  // Repeated cards are stored as "Name (1 of N)". Use the first copy's animation.
  for (uint8_t copyCount = 2; copyCount <= 5; copyCount++) {
    candidate = String(CARD_ANIMATION_ROOT) + "/" + assetName
              + " (1 of " + String(copyCount) + ")";
    if (animationDirectoryExists(candidate)) {
      directory = candidate;
      return true;
    }
  }

  return false;
}

static bool findCardAnimationDirectory(const CardDef &card, String &directory) {
  if (!sdOk) return false;
  if (tryAnimationAssetName(card.name, directory)) return true;

  // FAT filenames cannot contain ':', so converted folders use '_' instead.
  String normalizedName = card.name;
  normalizedName.replace(':', '_');
  if (normalizedName != card.name && tryAnimationAssetName(normalizedName, directory)) {
    return true;
  }

  const char *alias = animationAssetAlias(card.name);
  return alias != nullptr && tryAnimationAssetName(alias, directory);
}

// Stream one raw RGB565 big-endian frame from SD to the display. SD and TFT
// share SPI, so each chunk is read before its separate TFT write transaction.
static bool displayAnimFrame(const char *path, int x, int y, int w, int h) {
  if (!sdOk || w <= 0 || h <= 0 || x < 0 || y < 0
      || x + w > tft.width() || y + h > tft.height()) {
    return false;
  }

  deselectAllSPIDevices();
  File frameFile = SD.open(path, FILE_READ);
  if (!frameFile) {
    Serial.print("Animation frame not found: ");
    Serial.println(path);
    return false;
  }

  const uint32_t expectedBytes = (uint32_t)w * (uint32_t)h * 2U;
  if ((uint32_t)frameFile.size() != expectedBytes) {
    Serial.printf("Invalid animation frame size: %s (%u, expected %u)\n",
                  path, (unsigned int)frameFile.size(), (unsigned int)expectedBytes);
    frameFile.close();
    return false;
  }

  const size_t PIXEL_BUFFER_SIZE = ANIMATION_WIDTH * 8;
  static uint16_t pixelBuffer[PIXEL_BUFFER_SIZE];
  const int rowsPerChunk = (int)(PIXEL_BUFFER_SIZE / (size_t)w);

  for (int row = 0; row < h;) {
    int rows = h - row;
    if (rows > rowsPerChunk) rows = rowsPerChunk;
    const size_t pixelCount = (size_t)w * (size_t)rows;
    const size_t byteCount = pixelCount * 2U;

    deselectAllSPIDevices();
    size_t bytesRead = frameFile.read((uint8_t *)pixelBuffer, byteCount);
    if (bytesRead != byteCount) {
      Serial.print("Animation frame read failed: ");
      Serial.println(path);
      frameFile.close();
      return false;
    }

    // Convert the file's big-endian bytes to native uint16_t RGB565 values.
    for (size_t i = 0; i < pixelCount; i++) {
      pixelBuffer[i] = (pixelBuffer[i] >> 8) | (pixelBuffer[i] << 8);
    }

    deselectAllSPIDevices();
    tft.startWrite();
    tft.setAddrWindow(x, y + row, w, rows);
    tft.writePixels(pixelBuffer, pixelCount);
    tft.endWrite();
    row += rows;
  }

  deselectAllSPIDevices();
  frameFile.close();
  deselectAllSPIDevices();
  return true;
}

void drawCardPlayedAnimation(uint8_t playerIndex, const CardDef &card) {
  deselectAllSPIDevices();

  String animationDirectory;
  if (findCardAnimationDirectory(card, animationDirectory)) {
    bool displayedAnyFrame = false;

    for (uint8_t frameIndex = 0; frameIndex < MAX_ANIMATION_FRAMES; frameIndex++) {
      char path[192];
      snprintf(path, sizeof(path), "%s/f%03u.bin",
               animationDirectory.c_str(), frameIndex);

      deselectAllSPIDevices();
      if (!SD.exists(path)) break;

      uint32_t frameStartedAt = millis();
      if (!displayAnimFrame(path, 0, 0, ANIMATION_WIDTH, ANIMATION_HEIGHT)) break;
      displayedAnyFrame = true;

      uint32_t elapsed = millis() - frameStartedAt;
      if (elapsed < ANIMATION_FRAME_INTERVAL_MS) {
        delay(ANIMATION_FRAME_INTERVAL_MS - elapsed);
      }
    }

    if (displayedAnyFrame) return;
  }

  // Keep the original code-drawn animation as a fallback for missing assets.
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