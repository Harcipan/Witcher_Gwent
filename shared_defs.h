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
  ABILITY_HERO,             // immune to weather and special cards
  ABILITY_SPY,              // place on opponent's side, draw 2 cards
  ABILITY_MEDIC,            // revive a card from the discard pile
  ABILITY_MUSTER,           // play all same-named cards from deck instantly
  ABILITY_TIGHT_BOND,       // doubles strength with adjacent same-named card
  ABILITY_MORALE_BOOST,     // adds +1 to all units in the row
  ABILITY_AGILE,            // can be placed in close or ranged row
  ABILITY_DESTROY_STRONGEST,// scorch: kills the strongest card(s) on the field
  ABILITY_COMMANDERS_HORN,  // doubles the strength of all units in the row
};

const uint8_t MAX_HAND_SIZE = 20;

enum GamePhase : uint8_t {
  PHASE_DRAW,
  PHASE_PLAY,
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
extern GamePhase gamePhase;
extern uint8_t roundNumber;
extern uint8_t roundWins[2];
extern uint8_t handCards[2][MAX_HAND_SIZE];
extern uint8_t handCount[2];
extern uint8_t drawsNeeded[2];
extern String lastDrawnCardName[2];

void drawPlayMessage(const char *msg, uint16_t color = ILI9341_WHITE);
void drawDrawScreen();

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