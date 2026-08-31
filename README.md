# Witcher Gwent Table

A physical implementation of the Gwent card game from *The Witcher* universe, built on an
ESP32 microcontroller. Players use real NFC-tagged cards scanned by RFID readers; game
state, scores, and animations are rendered on a colour touchscreen with background music
played through an I2S DAC.

---

## Hardware

| Component | Part | Notes |
|---|---|---|
| Microcontroller | ESP32 | All logic, SPI bus master |
| Display | ILI9341 TFT (320×240) | Landscape orientation |
| Touchscreen | XPT2046 | Overlaid on TFT |
| Audio DAC | PCM5102 | Connected via I2S |
| Storage | MicroSD card | WAV audio + animation assets |
| Card readers | 2× MFRC522 RFID | One per player |

All SPI peripherals share the same SCK/MISO/MOSI lines. CS pins are managed manually
(all deselected before switching devices).

### Pin Map

| Signal | GPIO |
|---|---|
| SPI SCK | 18 |
| SPI MISO | 23 |
| SPI MOSI | 19 |
| TFT CS | 5 |
| TFT DC | 2 |
| TFT RST | 4 |
| SD CS | 13 |
| Touch CS | 15 |
| Touch IRQ | 34 |
| I2S BCK | 26 |
| I2S WS (RCK) | 27 |
| I2S DOUT | 25 |
| RFID RST | 14 |
| RFID1 SS | 22 |
| RFID2 SS | 17 |

---

## Libraries Required

Install via Arduino Library Manager:

- `Adafruit GFX Library`
- `Adafruit ILI9341`
- `XPT2046_Touchscreen`
- `MFRC522`
- `SD` (ESP32 built-in)
- `driver/i2s` (ESP32 built-in)

---

## SD Card Setup

Format the SD card as FAT32. Place files at these paths:

```
/w1.wav          — background music, played at startup
/w2.wav          — background music, queued after w1
```

WAV files must be 16-bit PCM (uncompressed). Sample rate and mono/stereo are
auto-detected. See [DOCS.md](DOCS.md) for conversion instructions.

---

## Card Setup

Each physical card needs an NFC sticker/tag attached. The UID of each tag must be
registered in `cards.ino`. On first run, scan a card and read its UID from the Serial
Monitor (115200 baud), then update the matching entry in `cards.ino`.

The current database contains placeholder UIDs in the format `00:00:00:00:00:00:XX`.

---

## Gameplay

Gwent is a 2-player card game. The implementation follows the original Witcher 3 rules:

1. **Draw phase** — both players scan cards from their deck to draw a hand (10 cards in
   round 1, 1 card in subsequent rounds)
2. **Play phase** — players alternate scanning a card from their hand to play it, or press
   **Skip** on the touchscreen to pass
3. The player with the higher total power at the end of a round wins it
4. First to win 2 rounds wins the match

Card abilities implemented: Hero, Spy, Medic, Muster, Tight Bond, Morale Boost, Agile,
Scorch, Commander's Horn.

---