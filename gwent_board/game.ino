extern const uint8_t PLAYER_COUNT = 2;
extern const uint8_t MAX_PLAYED_PER_PLAYER = 10;

PlayedCard playedCards[2][10];
uint8_t playedCount[2] = { 0, 0 };
uint8_t activePlayer = 0;
bool playerPassed[2] = { false, false };
bool gameOver = false;
String gameMessage = "";
String lastPlayedCardName = "None";

const uint8_t DRAWS_PER_ROUND[3] = {10, 1, 1};
GamePhase gamePhase = PHASE_DRAW;
uint8_t roundNumber = 0;
uint8_t roundWins[2] = {0, 0};
uint8_t handCards[2][MAX_HAND_SIZE];
uint8_t handCount[2] = {0, 0};
uint8_t drawsNeeded[2] = {0, 0};
bool pendingAdvanceTurn = false;
String lastDrawnCardName[2] = {"None", "None"};

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

bool cardInHand(uint8_t player, uint8_t cardIndex) {
  for (uint8_t i = 0; i < handCount[player]; i++) {
    if (handCards[player][i] == cardIndex) return true;
  }
  return false;
}

void removeFromHand(uint8_t player, uint8_t cardIndex) {
  for (uint8_t i = 0; i < handCount[player]; i++) {
    if (handCards[player][i] == cardIndex) {
      handCards[player][i] = handCards[player][handCount[player] - 1];
      handCount[player]--;
      return;
    }
  }
}

void startDrawPhase(uint8_t draws0, uint8_t draws1) {
  drawsNeeded[0] = draws0;
  drawsNeeded[1] = draws1;
  pendingAdvanceTurn = false;
  gamePhase = PHASE_DRAW;
  gameMessage = String("Draw: P1 needs ") + draws0 + ", P2 needs " + draws1;
  drawDrawScreen();
}

void checkDrawComplete() {
  if (drawsNeeded[0] > 0 || drawsNeeded[1] > 0) return;
  gamePhase = PHASE_PLAY;
  if (pendingAdvanceTurn) {
    pendingAdvanceTurn = false;
    advanceTurn();
  } else {
    gameMessage = String("P") + String(activePlayer + 1) + " plays first";
    drawGameScreen();
  }
}

void handleRFIDDraw(uint8_t playerIndex, const String &uid) {
  if (drawsNeeded[playerIndex] == 0) {
    gameMessage = String("P") + String(playerIndex + 1) + " done, wait for other";
    drawDrawScreen();
    return;
  }

  int cardIndex = findCardIndexByUID(uid);
  if (cardIndex < 0) {
    gameMessage = String("Unknown card: ") + uid;
    drawDrawScreen();
    return;
  }

  if (cardInHand(0, cardIndex) || cardInHand(1, cardIndex) || cardAlreadyPlayed(cardIndex)) {
    gameMessage = String(cardPool[cardIndex].name) + " already registered";
    drawDrawScreen();
    return;
  }

  if (handCount[playerIndex] >= MAX_HAND_SIZE) {
    gameMessage = String("P") + String(playerIndex + 1) + " hand full";
    drawDrawScreen();
    return;
  }

  handCards[playerIndex][handCount[playerIndex]++] = (uint8_t)cardIndex;
  drawsNeeded[playerIndex]--;

  const CardDef &card = cardPool[cardIndex];
  lastDrawnCardName[playerIndex] = card.name;
  gameMessage = String("P") + String(playerIndex + 1) + " drew " + card.name;

  Serial.printf("P%u drew %s (%u left)\n", playerIndex + 1, card.name, drawsNeeded[playerIndex]);

  drawDrawScreen();
  checkDrawComplete();
}

bool cardIsDestroyable(const CardDef &card) {
  return card.ability != ABILITY_HERO;
}

const char *abilityName(CardAbility ability) {
  switch (ability) {
    case ABILITY_HERO:             return "Hero";
    case ABILITY_SPY:              return "Spy";
    case ABILITY_MEDIC:            return "Medic";
    case ABILITY_MUSTER:           return "Muster";
    case ABILITY_TIGHT_BOND:       return "Tight Bond";
    case ABILITY_MORALE_BOOST:     return "Morale Boost";
    case ABILITY_AGILE:            return "Agile";
    case ABILITY_SCORCH:return "Scorch";
    case ABILITY_COMMANDERS_HORN:  return "Cmdr's Horn";
    case ABILITY_NONE:
    default:                       return "None";
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
  switch (card.ability) {
    case ABILITY_SCORCH: {
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
      break;
    }
    case ABILITY_SPY:
      drawsNeeded[playerIndex] += 2;
      pendingAdvanceTurn = true;
      gamePhase = PHASE_DRAW;
      gameMessage = String("P") + String(playerIndex + 1) + " spy: draw 2";
      drawDrawScreen();
      break;
    default:
      break;
  }
}

bool shouldEndRound() {
  bool p0Done = playerPassed[0] || handCount[0] == 0;
  bool p1Done = playerPassed[1] || handCount[1] == 0;
  return p0Done && p1Done;
}

void finishRound() {
  int p1Score = getPlayerScore(0);
  int p2Score = getPlayerScore(1);
  int winner = -1;

  if (p1Score > p2Score) winner = 0;
  else if (p2Score > p1Score) winner = 1;

  if (winner >= 0) {
    roundWins[winner]++;
    gameMessage = String("Round ") + String(roundNumber + 1) + " -> P" + String(winner + 1) + " wins";
  } else {
    roundWins[0]++;
    roundWins[1]++;
    gameMessage = String("Round ") + String(roundNumber + 1) + " -> draw (both lose gem)";
  }

  drawVictoryAnimation(winner);

  if (roundWins[0] >= 2 || roundWins[1] >= 2) {
    gameOver = true;
    int gameWinner = -1;
    if (roundWins[0] >= 2 && roundWins[1] < 2)      gameWinner = 0;
    else if (roundWins[1] >= 2 && roundWins[0] < 2) gameWinner = 1;
    gameMessage = gameWinner >= 0
      ? String("P") + String(gameWinner + 1) + " wins the game!"
      : "Game is a tie!";
    drawGameScreen();
    return;
  }

  roundNumber++;
  for (uint8_t p = 0; p < PLAYER_COUNT; p++) {
    playedCount[p] = 0;
    playerPassed[p] = false;
    for (uint8_t i = 0; i < MAX_PLAYED_PER_PLAYER; i++) {
      playedCards[p][i].cardIndex = 0;
      playedCards[p][i].destroyed = false;
    }
  }

  uint8_t draws = (roundNumber < 3) ? DRAWS_PER_ROUND[roundNumber] : 0;
  if (draws > 0) {
    startDrawPhase(draws, draws);
  } else {
    gamePhase = PHASE_PLAY;
    drawGameScreen();
  }
}

void advanceTurn() {
  if (shouldEndRound()) {
    finishRound();
    return;
  }

  uint8_t otherPlayer = 1 - activePlayer;
  if (!playerPassed[otherPlayer] && handCount[otherPlayer] > 0) {
    activePlayer = otherPlayer;
  }

  drawGameScreen();
}

void startNewGame() {
  randomSeed((uint32_t)esp_random());

  for (uint8_t p = 0; p < PLAYER_COUNT; p++) {
    playedCount[p] = 0;
    playerPassed[p] = false;
    handCount[p] = 0;
    roundWins[p] = 0;
    drawsNeeded[p] = 0;
    lastDrawnCardName[p] = "None";
    for (uint8_t i = 0; i < MAX_PLAYED_PER_PLAYER; i++) {
      playedCards[p][i].cardIndex = 0;
      playedCards[p][i].destroyed = false;
    }
  }

  roundNumber = 0;
  activePlayer = random(0, PLAYER_COUNT);
  gameOver = false;
  pendingAdvanceTurn = false;
  lastPlayedCardName = "None";
  gameMessage = String("Coin flip: P") + String(activePlayer + 1) + " starts";

  Serial.println("--- New game started ---");
  Serial.println(gameMessage);

  startDrawPhase(DRAWS_PER_ROUND[0], DRAWS_PER_ROUND[0]);
}

void handleRFIDCardScanned(uint8_t playerIndex, const String &uid) {
  if (gamePhase == PHASE_DRAW) {
    handleRFIDDraw(playerIndex, uid);
    return;
  }

  if (gameOver) {
    gameMessage = "Touch NEW to restart";
    drawGameScreen();
    return;
  }

  if (playerIndex >= PLAYER_COUNT) return;

  if (playerIndex != activePlayer) {
    gameMessage = String("Wait: P") + String(activePlayer + 1) + " turn";
    drawGameScreen();
    return;
  }

  if (playerPassed[playerIndex]) {
    gameMessage = String("P") + String(playerIndex + 1) + " already passed";
    drawGameScreen();
    return;
  }

  int cardIndex = findCardIndexByUID(uid);
  if (cardIndex < 0) {
    gameMessage = String("Unknown card: ") + uid;
    drawGameScreen();
    return;
  }

  if (!cardInHand(playerIndex, cardIndex)) {
    gameMessage = String(cardPool[cardIndex].name) + " not in your hand";
    drawGameScreen();
    return;
  }

  if (playedCount[playerIndex] >= MAX_PLAYED_PER_PLAYER) {
    gameMessage = String("P") + String(playerIndex + 1) + " board full";
    drawGameScreen();
    return;
  }

  removeFromHand(playerIndex, cardIndex);
  playedCards[playerIndex][playedCount[playerIndex]].cardIndex = cardIndex;
  playedCards[playerIndex][playedCount[playerIndex]].destroyed = false;
  playedCount[playerIndex]++;

  const CardDef &card = cardPool[cardIndex];
  lastPlayedCardName = card.name;
  gameMessage = String("P") + String(playerIndex + 1) + " played " + card.name;

  Serial.printf("P%u played %s UID %s\n", playerIndex + 1, card.name, uid.c_str());

  pendingTrack = "/sounds/card_play.wav";
  drawCardPlayedAnimation(playerIndex, card);
  applyCardAbility(playerIndex, card);

  if (gamePhase == PHASE_PLAY) advanceTurn();
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
    if (gameOver) {
      startNewGame();
    } else if (gamePhase == PHASE_DRAW) {
      drawsNeeded[0] = 0;
      drawsNeeded[1] = 0;
      checkDrawComplete();
    } else {
      handlePlayerPass();
    }
  }
}

void redrawGameScreen() {
  drawGameScreen();
}

void onAudioPlaybackFinished() {
  redrawGameScreen();
  playNextSoundtrack();
}