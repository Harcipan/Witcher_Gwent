# Card Play Animation & Sound — Implementation Plan

When a card is played, the game should trigger a sound effect and a visual animation,
both loaded from the SD card. This file tracks what needs to be sourced, converted,
and coded.

---

## Status

- [x] Source sound file
- [x] Convert sound to correct WAV format
- [x] Source/create animation frames
- [x] Convert frames to raw RGB565 binary at 320×240
- [ ] Copy assets to SD card
- [x] Add `displayAnimFrame()` helper in `display.ino`
- [x] Update `drawCardPlayedAnimation()` to use SD frames
- [x] Set `pendingTrack` in `game.ino` when a card is played

---

## Part 1 — Sound Effect

### Requirements
The existing audio pipeline (`audio.ino`) streams 16-bit PCM WAV from SD via I2S to the PCM5102 DAC.
Any WAV file must be:
- Format: PCM (uncompressed), `audioFormat = 1`
- Bit depth: 16-bit
- Channels: mono or stereo
- Sample rate: any (the driver is reconfigured per file via `i2s_set_clk`)

### Getting the file

**Option A — Witcher 3 game files** (requires owning the game):
1. Locate `.wem` audio files under `content\content0\soundbanks\`
2. Extract with [vgmstream](https://github.com/vgmstream/vgmstream) or [Wwise Unpacker](https://github.com/nicholasgasior/wwiseunpacker)
3. Convert to WAV (see below)

**Option B — Free sources:**
- [freesound.org](https://freesound.org) — search `"card whoosh"`, `"card slap"`, or `"card place"`
- [zapsplat.com](https://www.zapsplat.com) — search card/paper sound effects
- Download as MP3 or OGG, then convert (see below)

### Converting to PCM WAV

```sh
# Replace input.mp3 with your source file
ffmpeg -i input.mp3 -ar 22050 -ac 1 -acodec pcm_s16le card_play.wav
```

- `-ar 22050` — 22 kHz is sufficient quality and reduces file size vs 44.1 kHz
- `-ac 1` — mono keeps the file smaller; stereo also works
- `-acodec pcm_s16le` — signed 16-bit little-endian PCM, required by the parser

### SD card path

```
/sounds/card_play.wav
```

---

## Part 2 — Animation Frames

### Requirements
The ILI9341 display is 320×240. Every card-play animation frame uses the entire
screen, starting at `(0, 0)`.

Frames are stored as raw **RGB565 big-endian** binary files — no header, just pixels
in row-major order. Each frame = `320 × 240 × 2 = 153,600 bytes`.

The converted asset set in `Cards/converted_flip_magic_320x240` contains 12 frames
per animation. One animation occupies 1,843,200 bytes (about 1.76 MiB). The code
targets 15 fps, but actual speed is limited by SD and TFT throughput on the shared SPI bus.

### Getting frame images

**Option A — Gwent card art still + glow effect:**
1. Download card artwork from the [Witcher Wiki](https://witcher.fandom.com/wiki/Gwent)
2. In Photoshop/GIMP, create 10–15 frames showing a golden glow frame appearing, brightening, then fading
3. Export each frame as PNG at exactly 320×240

**Option B — Animated GIF (card flip / magic burst):**
1. Find or create a GIF on [Giphy](https://giphy.com) or similar
2. Extract frames using FFmpeg:
   ```sh
   ffmpeg -i animation.gif -vf "scale=320:240" frames_in/f%03d.png
   ```

**Option C — Simple in-engine effect** (no images needed): skip SD frames entirely and
enhance the existing code-drawn animation with more flash/slide steps. Only do this
if sourcing images proves impractical.

### Converting PNG frames to raw RGB565

Save and run `tools/convert_frames.py` (see below) with your PNG frames in `frames_in/`:

```python
# tools/convert_frames.py
from PIL import Image
import struct, os

W, H = 320, 240
os.makedirs('frames_out', exist_ok=True)

for i, name in enumerate(sorted(f for f in os.listdir('frames_in') if f.endswith('.png'))):
    img = Image.open(f'frames_in/{name}').resize((W, H)).convert('RGB')
    with open(f'frames_out/f{i:03d}.bin', 'wb') as out:
        for y in range(H):
            for x in range(W):
                r, g, b = img.getpixel((x, y))
                rgb565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)
                out.write(struct.pack('>H', rgb565))

print(f"Converted {i+1} frames.")
```

Requires Python 3 + Pillow: `pip install Pillow`

### SD card path

Copy the repository's `Cards/converted_flip_magic_320x240` directory to the SD
card without changing its relative path. Each card has its own frame directory:

```
/Cards/converted_flip_magic_320x240/Geralt of Rivia/f000.bin
/Cards/converted_flip_magic_320x240/Geralt of Rivia/f001.bin
...
/Cards/converted_flip_magic_320x240/Geralt of Rivia/f011.bin
```

---

## Part 3 — Code Changes

### 3a. `display.ino` — `displayAnimFrame()` helper

Implemented. The helper validates that a full-screen frame is 153,600 bytes,
reads it in row-aligned chunks, converts RGB565 big-endian data to native
`uint16_t` values, and writes each chunk at `(0, 0)` through `(319, 239)`.

The SD card and TFT share SPI. Each SD read is completed before the corresponding
TFT write transaction begins, so their chip-select lines are never active together.

### 3b. `display.ino` — update `drawCardPlayedAnimation()`

Implemented. `drawCardPlayedAnimation()` resolves a directory from `card.name`,
plays sequential `f000.bin`, `f001.bin`, and later frames full-screen, and stops at
the first missing frame (up to 30 frames). It supports:

- Exact card-name directories.
- Repeated-card directories such as `Arachas (1 of 3)`.
- FAT-safe names where `:` became `_`, such as `Crone_ Brewess`.
- Known aliases for shortened or unaccented game names.

If the card has no usable animation directory or its first frame cannot be displayed,
the existing code-drawn card animation remains the fallback. The resolver currently
matches 158 of 161 card definitions. The asset set has no matching directory for Roach,
Bovine Defense Force, or Hemdall, so those cards use the fallback.

### 3c. `game.ino` — queue the sound in `handleRFIDCardScanned()`

Find the line `drawCardPlayedAnimation(playerIndex, card);` and add the sound trigger before it:

```cpp
  pendingTrack = "/sounds/card_play.wav";
  drawCardPlayedAnimation(playerIndex, card);
```

`pendingTrack` is picked up by `serviceAudio()` after the current track finishes.
This preserves the existing audio queue behavior; truly synchronized animation and
audio will require non-blocking animation playback or a separate streaming task.

---

## SD Card Layout (final)

```
/w1.wav                       — existing background music track 1
/w2.wav                       — existing background music track 2
/sounds/
  card_play.wav               — card play sound effect
/Cards/
  converted_flip_magic_320x240/
    Geralt of Rivia/
      f000.bin                — animation frame 0 (320×240, raw RGB565 BE)
      f001.bin
      ...
      f011.bin
    ...                       — other card animation directories
```
