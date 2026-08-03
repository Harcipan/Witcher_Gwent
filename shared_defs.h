#pragma once

#include <Arduino.h>

struct WavInfo {
  uint16_t audioFormat;
  uint16_t channels;
  uint32_t sampleRate;
  uint16_t bitsPerSample;
  uint32_t dataOffset;
  uint32_t dataSize;
};

extern String pendingTrack;

enum CardAbility : uint8_t {
  ABILITY_NONE,
  ABILITY_DESTROY_STRONGEST,
  ABILITY_HERO
};

struct CardDef {
  const char *uid;
  const char *name;
  uint8_t score;
  CardAbility ability;
};

struct PlayedCard {
  uint8_t cardIndex;
  bool destroyed;
};

extern const uint8_t PLAYER_COUNT;
extern const uint8_t CARD_POOL_SIZE;
extern const uint8_t MAX_PLAYED_PER_PLAYER;

extern const CardDef cardPool[];
extern PlayedCard playedCards[][10];
extern uint8_t playedCount[];
extern uint8_t activePlayer;
extern bool playerPassed[];
extern bool gameOver;
extern String gameMessage;
extern String lastPlayedCardName;

void drawPlayMessage(const char *msg, uint16_t color = ILI9341_WHITE);

void startNewGame();
void handleRFIDCardScanned(uint8_t playerIndex, const String &uid);
void handleTouchScreenPressed(int rawX, int rawY);
void redrawGameScreen();
void onAudioPlaybackFinished();
int getPlayerScore(uint8_t playerIndex);
uint8_t countAvailableCards();
const char *abilityName(CardAbility ability);
bool cardIsDestroyable(const CardDef &card);

void drawGameScreen();
void drawCardPlayedAnimation(uint8_t playerIndex, const CardDef &card);
void drawDestroyAnimation(uint8_t playerIndex, const CardDef &card);
void drawVictoryAnimation(int winner);