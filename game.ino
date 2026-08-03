extern const uint8_t PLAYER_COUNT = 2;
extern const uint8_t CARD_POOL_SIZE = 10;
extern const uint8_t MAX_PLAYED_PER_PLAYER = 10;

// Replace these placeholder UIDs with the real UIDs printed in Serial Monitor.
extern const CardDef cardPool[10] = {
  { "04:9B:27:3D:C1:2A:81", "Geralt",        15, ABILITY_HERO },
  { "04:92:27:3D:C1:2A:81", "Yennefer",       7, ABILITY_NONE },
  { "04:93:27:3D:C1:2A:81", "Ciri",          10, ABILITY_HERO },
  { "04:94:27:3D:C1:2A:81", "Triss",          6, ABILITY_NONE },
  { "04:95:27:3D:C1:2A:81", "Vesemir",        6, ABILITY_NONE },
  { "04:96:27:3D:C1:2A:81", "Scorch",         0, ABILITY_DESTROY_STRONGEST },
  { "04:97:27:3D:C1:2A:81", "Zoltan",         5, ABILITY_NONE },
  { "04:98:27:3D:C1:2A:81", "Dandelion",      2, ABILITY_NONE },
  { "04:99:27:3D:C1:2A:81", "Imlerith",      10, ABILITY_NONE },
  { "04:9A:27:3D:C1:2A:81", "Villentreten",   7, ABILITY_DESTROY_STRONGEST }
};

PlayedCard playedCards[2][10];
uint8_t playedCount[2] = { 0, 0 };
uint8_t activePlayer = 0;
bool playerPassed[2] = { false, false };
bool gameOver = false;
String gameMessage = "";
String lastPlayedCardName = "None";

int findCardIndexByUID(const String &uid) {
  String normalizedUID = uid;
  normalizedUID.toUpperCase();

  for (uint8_t i = 0; i < CARD_POOL_SIZE; i++) {
    if (normalizedUID == cardPool[i].uid) return i;
  }
  return -1;
}

bool cardAlreadyPlayed(uint8_t cardIndex) {
  for (uint8_t player = 0; player < PLAYER_COUNT; player++) {
    for (uint8_t i = 0; i < playedCount[player]; i++) {
      if (playedCards[player][i].cardIndex == cardIndex) return true;
    }
  }
  return false;
}

bool cardIsDestroyable(const CardDef &card) {
  return card.ability != ABILITY_HERO;
}

const char *abilityName(CardAbility ability) {
  switch (ability) {
    case ABILITY_DESTROY_STRONGEST: return "Destroy strongest";
    case ABILITY_HERO:              return "Hero";
    case ABILITY_NONE:
    default:                        return "None";
  }
}

int getPlayerScore(uint8_t playerIndex) {
  if (playerIndex >= PLAYER_COUNT) return 0;

  int score = 0;
  for (uint8_t i = 0; i < playedCount[playerIndex]; i++) {
    PlayedCard &played = playedCards[playerIndex][i];
    if (!played.destroyed) score += cardPool[played.cardIndex].score;
  }
  return score;
}

uint8_t countAvailableCards() {
  uint8_t used = 0;
  for (uint8_t player = 0; player < PLAYER_COUNT; player++) {
    used += playedCount[player];
  }
  return CARD_POOL_SIZE - used;
}

int findStrongestDestroyableCard(uint8_t playerIndex) {
  if (playerIndex >= PLAYER_COUNT) return -1;

  int strongestSlot = -1;
  int strongestScore = -1;
  for (uint8_t i = 0; i < playedCount[playerIndex]; i++) {
    PlayedCard &played = playedCards[playerIndex][i];
    const CardDef &card = cardPool[played.cardIndex];

    if (played.destroyed || !cardIsDestroyable(card)) continue;
    if ((int)card.score > strongestScore) {
      strongestScore = card.score;
      strongestSlot = i;
    }
  }
  return strongestSlot;
}

void applyCardAbility(uint8_t playerIndex, const CardDef &card) {
  if (card.ability != ABILITY_DESTROY_STRONGEST) return;

  uint8_t opponent = 1 - playerIndex;
  int slot = findStrongestDestroyableCard(opponent);

  if (slot < 0) {
    gameMessage = String(card.name) + " found no target";
    return;
  }

  playedCards[opponent][slot].destroyed = true;
  const CardDef &destroyedCard = cardPool[playedCards[opponent][slot].cardIndex];
  gameMessage = String(card.name) + " destroyed " + destroyedCard.name;
  drawDestroyAnimation(opponent, destroyedCard);
}

bool shouldEndRound() {
  return countAvailableCards() == 0 || (playerPassed[0] && playerPassed[1]);
}

void finishRound() {
  gameOver = true;

  int p1Score = getPlayerScore(0);
  int p2Score = getPlayerScore(1);
  int winner = -1;

  if (p1Score > p2Score) winner = 0;
  else if (p2Score > p1Score) winner = 1;

  if (winner < 0) gameMessage = "Round ended: draw";
  else gameMessage = String("Round ended: P") + String(winner + 1) + " wins";

  drawGameScreen();
  drawVictoryAnimation(winner);
}

void advanceTurn() {
  if (shouldEndRound()) {
    finishRound();
    return;
  }

  uint8_t otherPlayer = 1 - activePlayer;
  if (!playerPassed[otherPlayer]) activePlayer = otherPlayer;

  drawGameScreen();
}

void startNewGame() {
  randomSeed((uint32_t)esp_random());

  for (uint8_t player = 0; player < PLAYER_COUNT; player++) {
    playedCount[player] = 0;
    playerPassed[player] = false;

    for (uint8_t i = 0; i < MAX_PLAYED_PER_PLAYER; i++) {
      playedCards[player][i].cardIndex = 0;
      playedCards[player][i].destroyed = false;
    }
  }

  activePlayer = random(0, PLAYER_COUNT);
  gameOver = false;
  lastPlayedCardName = "None";
  gameMessage = String("Coin flip: Player ") + String(activePlayer + 1) + " starts";

  Serial.println("--- New game started ---");
  Serial.println(gameMessage);
  drawGameScreen();
}

void handleRFIDCardScanned(uint8_t playerIndex, const String &uid) {
  if (gameOver) {
    gameMessage = "Touch NEW GAME to restart";
    drawGameScreen();
    return;
  }

  if (playerIndex >= PLAYER_COUNT) return;

  if (playerIndex != activePlayer) {
    gameMessage = String("Wait: Player ") + String(activePlayer + 1) + " turn";
    drawGameScreen();
    return;
  }

  if (playerPassed[playerIndex]) {
    gameMessage = String("Player ") + String(playerIndex + 1) + " already passed";
    drawGameScreen();
    return;
  }

  int cardIndex = findCardIndexByUID(uid);
  if (cardIndex < 0) {
    gameMessage = String("Unknown card: ") + uid;
    drawGameScreen();
    return;
  }

  if (cardAlreadyPlayed(cardIndex)) {
    gameMessage = String(cardPool[cardIndex].name) + " already played";
    drawGameScreen();
    return;
  }

  if (playedCount[playerIndex] >= MAX_PLAYED_PER_PLAYER) {
    gameMessage = String("Player ") + String(playerIndex + 1) + " board full";
    drawGameScreen();
    return;
  }

  playedCards[playerIndex][playedCount[playerIndex]].cardIndex = cardIndex;
  playedCards[playerIndex][playedCount[playerIndex]].destroyed = false;
  playedCount[playerIndex]++;

  const CardDef &card = cardPool[cardIndex];
  lastPlayedCardName = card.name;
  gameMessage = String("P") + String(playerIndex + 1) + " played " + card.name;

  Serial.print("Player ");
  Serial.print(playerIndex + 1);
  Serial.print(" played ");
  Serial.print(card.name);
  Serial.print(" UID ");
  Serial.println(uid);

  drawCardPlayedAnimation(playerIndex, card);
  applyCardAbility(playerIndex, card);
  advanceTurn();
}

void handlePlayerPass() {
  if (gameOver) return;

  playerPassed[activePlayer] = true;
  gameMessage = String("Player ") + String(activePlayer + 1) + " passed";
  Serial.println(gameMessage);
  advanceTurn();
}

void handleTouchScreenPressed(int rawX, int rawY) {
  int screenX = map(rawX, touchMinX, touchMaxX, tft.width() - 1, 0);
  int screenY = map(rawY, touchMinY, touchMaxY, tft.height() - 1, 0);
  screenX = constrain(screenX, 0, tft.width() - 1);
  screenY = constrain(screenY, 0, tft.height() - 1);

  Serial.printf("Touch screen -> X:%d Y:%d\n", screenX, screenY);

  if (screenX >= 210 && screenX <= 310 && screenY >= 198 && screenY <= 232) {
    if (gameOver) startNewGame();
    else handlePlayerPass();
  }
}

void redrawGameScreen() {
  drawGameScreen();
}

void onAudioPlaybackFinished() {
  redrawGameScreen();
}