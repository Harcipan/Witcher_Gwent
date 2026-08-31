# Technical Documentation

---

## Table of Contents

### A. - Code Docs
1. [Architecture Overview](#architecture-overview)
2. [SPI Bus Management](#spi-bus-management)
3. [Audio System](#audio-system)
4. [Display & Animations](#display--animations)
5. [Game Logic](#game-logic)
6. [Card Database](#card-database)
7. [RFID Input](#rfid-input)
8. [Touch Input](#touch-input)
9. [Timing & Main Loop](#timing--main-loop)
10. [Adding / Registering Cards](#adding--registering-cards)
11. [WAV Audio Preparation](#wav-audio-preparation)
12. [Data Structures & Enums](#data-structures--enums)

#### B. - Electronics design

TODO

#### C. - Board 3D design

TODO

#### D. - Manufacturing

TODO
---

## Architecture Overview

The firmware is split across multiple `.ino` files which the Arduino IDE compiles into
a single translation unit. `shared_defs.h` is the single header included by all files;
it declares all shared types, enums, extern globals, and function prototypes.

```
gwent_board.ino   setup() / loop() — hardware init, main polling loop
      │
      ├── audio.ino      SD init, WAV parsing, I2S streaming
      ├── input.ino      Touch and RFID init, polling helpers
      ├── helpers.ino    SPI deselect, UID formatting, text utilities
      ├── display.ino    All TFT drawing functions
      ├── game.ino       Game state, RFID card handling, round/turn logic
      └── cards.ino      Card definitions array (cardPool[])
```

---

## SPI Bus Management

All SPI peripherals share one bus (MOSI 19, MISO 23, SCK 18). Manual CS management
is required because the ILI9341 TFT driver and the Adafruit GFX library hold the bus
between calls.

`deselectAllSPIDevices()` in `helpers.ino` drives all CS/SS pins HIGH:

```
TFT_CS   5    SD_CS   13    TOUCH_CS  15
RFID1_SS 22   RFID2_SS 17
```

**Rule:** call `deselectAllSPIDevices()` before accessing any SPI device. All driver
`begin()` calls and polling functions follow this pattern.

---

## Audio System

### Initialisation

`initSDCard()` initialises the SD library on `SD_CS` (GPIO 13).  
`initI2S()` installs the I2S driver on `I2S_NUM_0` with default settings (44.1 kHz,
16-bit stereo). The clock is reconfigured per file by `configureI2SForWav()` after the
WAV header is parsed.

### Playback flow

```
startWavFromSD(path)
  ├── SD.open(path)
  ├── parseWavHeader()       — reads RIFF/WAVE/fmt/data chunks
  ├── configureI2SForWav()   — calls i2s_set_clk() with file's actual sample rate
  └── sets audioPlaying = true

loop() calls serviceAudio() every iteration
  └── reads AUDIO_CHUNK_SIZE (4096) bytes from SD → writes to I2S DMA
      when data exhausted → stopWavPlayback()
          if pendingTrack set → startWavFromSD(pendingTrack)
          else               → onAudioPlaybackFinished()
```

### Queuing a track

Set `pendingTrack` to a path string at any point. `serviceAudio()` will start it
automatically when the current track finishes. Only one track can be queued at a time.

### Supported WAV format

| Property | Requirement |
|---|---|
| Audio format | PCM (uncompressed, `audioFormat = 1`) |
| Bit depth | 16-bit only |
| Channels | Mono or stereo |
| Sample rate | Any — auto-detected and applied via `i2s_set_clk` |

### Poll-rate throttling

RFID and touch polling intervals are doubled while audio is playing to reduce SPI bus
contention. See `currentTouchPollInterval()` / `currentRFIDPollInterval()` in
`helpers.ino`.

---

## Display & Animations

The ILI9341 runs at 10 MHz SPI, landscape orientation (rotation 1 = 320×240 px).

### Screen states

| Function | When shown |
|---|---|
| `drawStatusScreen()` | Boot diagnostics |
| `drawDrawScreen()` | Draw phase — shows draws remaining per player |
| `drawGameScreen()` | Main gameplay screen — scores, hands, round wins, message |
| `drawPlayMessage()` | Overlays a temporary status line |

### Animations

| Function | Trigger |
|---|---|
| `drawCardPlayedAnimation(player, card)` | Card scanned & played successfully |
| `drawDestroyAnimation(player, card)` | Scorch / Destroy Strongest ability fires |
| `drawVictoryAnimation(winner)` | Round ends |

`drawCardPlayedAnimation` draws a flashing coloured card box (blue = player 1,
red = player 2) then overlays the card name, power, and ability for ~800 ms before
returning to the game screen.

---

## Game Logic

### Game phases

```
PHASE_DRAW  →  both players scan cards to fill their hand
PHASE_PLAY  →  players alternate scanning a card (or pressing Skip)
```

Draw card counts per round: `{10, 1, 1}` (round 1 draws 10; rounds 2 and 3 draw 1).

### Turn flow

```
handleRFIDCardScanned(playerIndex, uid)
  ├── PHASE_DRAW  → handleRFIDDraw()
  │     ├── validates uid, checks not duplicate
  │     ├── adds to hand, decrements drawsNeeded[player]
  │     └── checkDrawComplete() → transitions to PHASE_PLAY when both done
  │
  └── PHASE_PLAY  → validates ownership, plays card
        ├── removeFromHand()
        ├── adds to playedCards[]
        ├── drawCardPlayedAnimation()
        ├── applyCardAbility()
        └── advanceTurn()
              └── shouldEndRound() → finishRound() if both players passed or out of cards
```

### Round resolution

`finishRound()` compares `getPlayerScore()` for both players. Higher score wins;
ties award a round win to both players (both lose a gem). The match ends when a player
reaches 2 round wins. On a tie at 2–2 the game is declared a draw.

### Scoring

`getPlayerScore()` sums the `score` field of all non-destroyed played cards.
Abilities that modify score at resolution (Tight Bond, Morale Boost, Commander's Horn,
weather cards) are **not yet implemented** in the score calculation — the field value
is used directly.

### Implemented abilities

| Ability | Effect |
|---|---|
| `ABILITY_HERO` | No effect in code beyond marking; immune to Scorch |
| `ABILITY_SCORCH` (Scorch) | Destroys the highest-power non-Hero card on the opponent's side |
| `ABILITY_SPY` | Awards 2 draw credits to the playing player; defers the turn advance until draws complete |

All other abilities are defined in the enum but have no runtime effect yet.

---

## Card Database

`cards.ino` contains `cardPool[]`, a `const CardDef[]` array. Each entry is:

```cpp
{ "UID_STRING", "Display Name", baseScore, ABILITY_ENUM }
```

UIDs are uppercase hex bytes separated by colons, e.g. `"A1:B2:C3:D4"`.
The current file uses sequential placeholder UIDs (`00:00:00:00:00:00:01` etc.).

`CARD_POOL_SIZE` is derived automatically in `shared_defs.h`:

```cpp
extern const uint8_t CARD_POOL_SIZE;
// defined in cards.ino as:
const uint8_t CARD_POOL_SIZE = sizeof(cardPool) / sizeof(cardPool[0]);
```

### Factions in the database

- Neutral (cards 0x01–0x18)
- Northern Realms (0x19–0x31)
- Nilfgaardian Empire (0x32–0x4E)
- Scoia'tael (0x4F–…)

---

## RFID Input

Two MFRC522 readers are polled in `loop()`. Each reader shares the SPI bus but has its
own SS pin (RFID1: GPIO 22, RFID2: GPIO 17) and a shared RST line (GPIO 14).

`pollRFID(reader, lastUID, name)`:
1. Calls `PICC_IsNewCardPresent()` + `PICC_ReadCardSerial()`.
2. Formats the UID using `uidToString()` (uppercase hex with colons).
3. Halts the card and stops crypto.
4. Returns `true` if a new card was read.

On a successful read, `handleRFIDCardScanned(playerIndex, uid)` is called.
Player 1 = RFID1 (index 0), Player 2 = RFID2 (index 1).

---

## Touch Input

XPT2046 returns raw ADC values (0–4095). These are mapped to screen coordinates using
`touchMinX/MaxX/Y` calibration constants in `gwent_board.ino`:

```cpp
int touchMinX = 200;  int touchMaxX = 3900;
int touchMinY = 200;  int touchMaxY = 3900;
```

Adjust these values if touch accuracy is off. The mapping inverts both axes:

```cpp
screenX = map(rawX, touchMinX, touchMaxX, tft.width() - 1, 0);
screenY = map(rawY, touchMinY, touchMaxY, tft.height() - 1, 0);
```

A latch flag (`touchPressedLatched`) prevents repeated triggers from a single press.
It resets when the IRQ pin goes HIGH (finger lifted).

The only active touch zone is the **Skip / New Game button** drawn by `drawActionButton()`
in the lower-right corner (approximately x: 210–310, y: 198–232).

---

## Timing & Main Loop

```
loop()
  serviceAudio()             — always first; streams next audio chunk to I2S
  
  touch IRQ reset (latch)
  
  if touchOk && IRQ LOW && poll interval elapsed:
    getPoint() → handleTouchScreenPressed()
    return early (skip RFID this iteration)
  
  if rfid1Ok && poll interval elapsed:
    pollRFID() → handleRFIDCardScanned(0, uid)
    return early
  
  if rfid2Ok && poll interval elapsed:
    pollRFID() → handleRFIDCardScanned(1, uid)
```

Poll intervals (milliseconds):

| Condition | Touch poll | Touch draw | RFID poll | RFID draw |
|---|---|---|---|---|
| Idle | 120 | 120 | 150 | 150 |
| Audio playing | 220 | 300 | 300 | 300 |

---

## Adding / Registering Cards

1. Power on the device and open the Arduino Serial Monitor at **115200 baud**.
2. Scan the physical card against **RFID1** (player 1 reader).
3. The monitor prints: `RFID1 UID: XX:XX:XX:XX`
4. In `cards.ino`, find the entry for that card and replace the placeholder UID with
   the printed value, preserving the uppercase format.
5. Re-flash the firmware.

Example — registering Geralt of Rivia:

```cpp
// Before:
{ "00:00:00:00:00:00:01", "Geralt of Rivia", 15, ABILITY_HERO },

// After (with real tag UID):
{ "A3:F1:04:2B",          "Geralt of Rivia", 15, ABILITY_HERO },
```

---

## WAV Audio Preparation

Convert any audio file to the required format using FFmpeg:

```sh
ffmpeg -i input.mp3 -ar 22050 -ac 1 -acodec pcm_s16le output.wav
```

| Flag | Meaning |
|---|---|
| `-ar 22050` | 22 kHz sample rate (good quality, smaller file than 44.1 kHz) |
| `-ac 1` | Mono (use `-ac 2` for stereo) |
| `-acodec pcm_s16le` | 16-bit little-endian PCM — required |

Place the resulting `.wav` file on the SD card and reference the path in code.

---

## Data Structures & Enums

### `CardDef`

```cpp
struct CardDef {
  const char *uid;      // RFID UID string, uppercase hex with colons
  const char *name;     // Display name
  uint8_t     score;    // Base power value
  CardAbility ability;
};
```

### `PlayedCard`

```cpp
struct PlayedCard {
  uint8_t cardIndex;  // Index into cardPool[]
  bool    destroyed;  // Set by Scorch; excluded from score
};
```

### `CardAbility`

```cpp
enum CardAbility : uint8_t {
  ABILITY_NONE,
  ABILITY_HERO,               // Immune to Scorch
  ABILITY_SPY,                // Play on opponent's side, draw 2
  ABILITY_MEDIC,              // (not implemented) Revive from discard
  ABILITY_MUSTER,             // (not implemented) Play all same-named cards
  ABILITY_TIGHT_BOND,         // (not implemented) Double strength with neighbour
  ABILITY_MORALE_BOOST,       // (not implemented) +1 to all in row
  ABILITY_AGILE,              // (not implemented) Melee or Ranged row
  ABILITY_SCORCH,  // Scorch: destroy opponent's strongest non-Hero
  ABILITY_COMMANDERS_HORN,    // (not implemented) Double all units in row
};
```

### `GamePhase`

```cpp
enum GamePhase : uint8_t {
  PHASE_DRAW,   // Both players building hand
  PHASE_PLAY,   // Alternating card play
};
```

### `WavInfo`

```cpp
struct WavInfo {
  uint16_t audioFormat;   // Must be 1 (PCM)
  uint16_t channels;      // 1 = mono, 2 = stereo
  uint32_t sampleRate;
  uint16_t bitsPerSample; // Must be 16
  uint32_t dataOffset;    // Byte offset of PCM data in file
  uint32_t dataSize;      // Byte length of PCM data
};
```
