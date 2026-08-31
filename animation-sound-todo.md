# Card Play Animation & Sound — Implementation Plan

When a card is played, the game should trigger a sound effect and a visual animation,
both loaded from the SD card. This file tracks what needs to be sourced, converted,
and coded.

---

## Status

- [ ] Source sound file
- [ ] Convert sound to correct WAV format
- [ ] Source/create animation frames
- [ ] Convert frames to raw RGB565 binary
- [ ] Copy assets to SD card
- [ ] Add `displayAnimFrame()` helper in `display.ino`
- [ ] Update `drawCardPlayedAnimation()` to use SD frames
- [ ] Set `pendingTrack` in `game.ino` when a card is played

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
- `-acodec pcm_s16le` — unsigned 16-bit little-endian PCM, required by the parser

### SD card path

```
/sounds/card_play.wav
```

---

## Part 2 — Animation Frames

### Requirements
The ILI9341 display is 320×240. The existing card popup occupies a **156×126 px** region
at screen coordinates `(82, 48)`. Animation frames should match this size.

Frames are stored as raw **RGB565 big-endian** binary files — no header, just pixels
in row-major order. Each frame = `156 × 126 × 2 = 39,312 bytes`.

Target: **10–15 frames** at ~15 fps → ~600 KB total on SD.

### Getting frame images

**Option A — Gwent card art still + glow effect:**
1. Download card artwork from the [Witcher Wiki](https://witcher.fandom.com/wiki/Gwent)
2. In Photoshop/GIMP, create 10–15 frames showing a golden glow frame appearing, brightening, then fading
3. Export each frame as PNG at exactly 156×126

**Option B — Animated GIF (card flip / magic burst):**
1. Find or create a GIF on [Giphy](https://giphy.com) or similar
2. Extract frames using FFmpeg:
   ```sh
   ffmpeg -i animation.gif -vf "scale=156:126" frames_in/f%03d.png
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

W, H = 156, 126
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

```
/anim/card_play/f000.bin
/anim/card_play/f001.bin
...
/anim/card_play/f014.bin
```

---

## Part 3 — Code Changes

### 3a. `display.ino` — add `displayAnimFrame()` helper

Add above `drawCardPlayedAnimation()`:

```cpp
// Stream one raw RGB565 frame from SD directly into the display window
static void displayAnimFrame(const char* path, int x, int y, int w, int h) {
  File f = SD.open(path);
  if (!f) return;
  uint8_t buf[512];
  deselectAllSPIDevices();
  tft.startWrite();
  tft.setAddrWindow(x, y, w, h);
  while (f.available()) {
    size_t n = f.read(buf, sizeof(buf));
    tft.writePixels((uint16_t*)buf, n / 2);
  }
  tft.endWrite();
  f.close();
}
```

### 3b. `display.ino` — update `drawCardPlayedAnimation()`

Replace the flashing loop with SD frame playback, falling back to the
existing code-drawn animation if no frame files are found:

```cpp
void drawCardPlayedAnimation(uint8_t playerIndex, const CardDef &card) {
  deselectAllSPIDevices();

  // Count available frames (stops at first missing file)
  uint8_t frameCount = 0;
  while (frameCount < 30) {
    char path[40];
    snprintf(path, sizeof(path), "/anim/card_play/f%03d.bin", frameCount);
    if (!SD.exists(path)) break;
    frameCount++;
  }

  if (frameCount > 0) {
    for (uint8_t i = 0; i < frameCount; i++) {
      char path[40];
      snprintf(path, sizeof(path), "/anim/card_play/f%03d.bin", i);
      displayAnimFrame(path, 82, 48, 156, 126);
      delay(66); // ~15 fps
    }
  } else {
    // Fallback: existing code-drawn flash
    uint16_t frameColor = playerIndex == 0 ? ILI9341_BLUE : ILI9341_RED;
    for (uint8_t i = 0; i < 3; i++) {
      tft.fillRoundRect(82, 48, 156, 126, 8, ILI9341_BLACK);
      tft.drawRoundRect(82, 48, 156, 126, 8, frameColor);
      tft.drawRoundRect(86, 52, 148, 118, 8, ILI9341_WHITE);
      delay(80);
      tft.fillRoundRect(82, 48, 156, 126, 8, frameColor);
      delay(80);
    }
    // ...rest of text overlay unchanged
  }
}
```

### 3c. `game.ino` — queue the sound in `handleRFIDCardScanned()`

Find the line `drawCardPlayedAnimation(playerIndex, card);` and add the sound trigger before it:

```cpp
  pendingTrack = "/sounds/card_play.wav";
  drawCardPlayedAnimation(playerIndex, card);
```

`pendingTrack` is picked up by `serviceAudio()` in the main loop after the current track
finishes (or immediately if nothing is playing), so the sound begins as the animation starts.

---

## SD Card Layout (final)

```
/w1.wav                       — existing background music track 1
/w2.wav                       — existing background music track 2
/sounds/
  card_play.wav               — card play sound effect
/anim/
  card_play/
    f000.bin                  — animation frame 0  (156×126 px, raw RGB565 BE)
    f001.bin
    ...
    f014.bin
```
