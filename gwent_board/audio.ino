File wavFile;
WavInfo currentWav;
bool audioPlaying = false;
String currentTrack = "";
String pendingTrack = "";

const char *SOUNDTRACK_DIRECTORY = "/Soundtracks";
String lastSoundtrackPath = "";
bool soundtrackPlaylistEnabled = false;

const size_t AUDIO_CHUNK_SIZE = 4096;
uint8_t audioBuf[AUDIO_CHUNK_SIZE];

bool initSDCard() {
  deselectAllSPIDevices();
  delay(10);

  bool ok = SD.begin(SD_CS, SPI);
  if (!ok) {
    Serial.println("SD init failed.");
    return false;
  }

  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached.");
    return false;
  }

  Serial.print("SD card type: ");
  switch (cardType) {
    case CARD_MMC:  Serial.println("MMC"); break;
    case CARD_SD:   Serial.println("SDSC"); break;
    case CARD_SDHC: Serial.println("SDHC"); break;
    default:        Serial.println("UNKNOWN"); break;
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD card size: %llu MB\n", cardSize);

  listDir(SD, "/", 2);
  return true;
}

bool initI2S() {
  i2s_config_t i2s_config = {};
  i2s_config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
  i2s_config.sample_rate = 44100;
  i2s_config.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
  i2s_config.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
  i2s_config.communication_format = I2S_COMM_FORMAT_STAND_I2S;
  i2s_config.intr_alloc_flags = 0;
  i2s_config.dma_buf_count = 8;
  i2s_config.dma_buf_len = 256;
  i2s_config.use_apll = false;
  i2s_config.tx_desc_auto_clear = true;
  i2s_config.fixed_mclk = 0;

  i2s_pin_config_t pin_config = {};
  pin_config.bck_io_num = I2S_BCK;
  pin_config.ws_io_num = I2S_WS;
  pin_config.data_out_num = I2S_DOUT;
  pin_config.data_in_num = I2S_PIN_NO_CHANGE;

  esp_err_t err = i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  if (err != ESP_OK) {
    Serial.printf("i2s_driver_install failed: %d\n", err);
    return false;
  }

  err = i2s_set_pin(I2S_NUM_0, &pin_config);
  if (err != ESP_OK) {
    Serial.printf("i2s_set_pin failed: %d\n", err);
    return false;
  }

  i2s_zero_dma_buffer(I2S_NUM_0);
  return true;
}

bool read16(File &f, uint16_t &value) {
  uint8_t b[2];
  if (f.read(b, 2) != 2) return false;
  value = (uint16_t)b[0] | ((uint16_t)b[1] << 8);
  return true;
}

bool read32(File &f, uint32_t &value) {
  uint8_t b[4];
  if (f.read(b, 4) != 4) return false;
  value = (uint32_t)b[0] |
          ((uint32_t)b[1] << 8) |
          ((uint32_t)b[2] << 16) |
          ((uint32_t)b[3] << 24);
  return true;
}

bool parseWavHeader(File &file, WavInfo &info) {
  info = {};

  file.seek(0);

  char riff[4];
  char wave[4];
  if (file.read((uint8_t*)riff, 4) != 4) return false;

  uint32_t chunkSize;
  if (!read32(file, chunkSize)) return false;

  if (file.read((uint8_t*)wave, 4) != 4) return false;

  if (strncmp(riff, "RIFF", 4) != 0 || strncmp(wave, "WAVE", 4) != 0) {
    Serial.println("Not a valid RIFF/WAVE file");
    return false;
  }

  bool foundFmt = false;
  bool foundData = false;

  while (file.available()) {
    char chunkId[4];
    uint32_t chunkLen;

    if (file.read((uint8_t*)chunkId, 4) != 4) return false;
    if (!read32(file, chunkLen)) return false;

    if (strncmp(chunkId, "fmt ", 4) == 0) {
      if (!read16(file, info.audioFormat)) return false;
      if (!read16(file, info.channels)) return false;
      if (!read32(file, info.sampleRate)) return false;

      uint32_t byteRate;
      uint16_t blockAlign;
      if (!read32(file, byteRate)) return false;
      if (!read16(file, blockAlign)) return false;
      if (!read16(file, info.bitsPerSample)) return false;

      if (chunkLen > 16) {
        file.seek(file.position() + (chunkLen - 16));
      }

      foundFmt = true;
    }
    else if (strncmp(chunkId, "data", 4) == 0) {
      info.dataOffset = file.position();
      info.dataSize = chunkLen;
      file.seek(info.dataOffset);
      foundData = true;
      break;
    }
    else {
      file.seek(file.position() + chunkLen);
    }

    if (chunkLen & 1) {
      file.seek(file.position() + 1);
    }
  }

  if (!foundFmt || !foundData) {
    Serial.println("Missing fmt or data chunk");
    return false;
  }

  Serial.println("WAV info:");
  Serial.printf("  audioFormat   : %u\n", info.audioFormat);
  Serial.printf("  channels      : %u\n", info.channels);
  Serial.printf("  sampleRate    : %lu\n", (unsigned long)info.sampleRate);
  Serial.printf("  bitsPerSample : %u\n", info.bitsPerSample);
  Serial.printf("  dataOffset    : %lu\n", (unsigned long)info.dataOffset);
  Serial.printf("  dataSize      : %lu\n", (unsigned long)info.dataSize);

  if (info.audioFormat != 1) {
    Serial.println("Only PCM WAV is supported");
    return false;
  }

  if (info.bitsPerSample != 16) {
    Serial.println("Only 16-bit WAV is supported");
    return false;
  }

  if (info.channels != 1 && info.channels != 2) {
    Serial.println("Only mono or stereo WAV supported");
    return false;
  }

  return true;
}

bool configureI2SForWav(const WavInfo &info) {
  esp_err_t err = i2s_set_clk(
    I2S_NUM_0,
    info.sampleRate,
    I2S_BITS_PER_SAMPLE_16BIT,
    (info.channels == 2) ? I2S_CHANNEL_STEREO : I2S_CHANNEL_MONO
  );

  if (err != ESP_OK) {
    Serial.printf("i2s_set_clk failed: %d\n", err);
    return false;
  }

  return true;
}

void stopWavPlayback() {
  if (wavFile) wavFile.close();
  i2s_zero_dma_buffer(I2S_NUM_0);
  audioPlaying = false;
  currentTrack = "";
}

bool startWavFromSD(const char *path) {
  if (!sdOk || !audioOk) {
    Serial.println("Cannot play WAV: SD or audio not ready");
    return false;
  }

  if (audioPlaying) {
    Serial.println("Audio already playing");
    return false;
  }

  deselectAllSPIDevices();
  wavFile = SD.open(path, FILE_READ);
  if (!wavFile) {
    Serial.print("Failed to open WAV: ");
    Serial.println(path);
    return false;
  }

  if (!parseWavHeader(wavFile, currentWav)) {
    Serial.println("WAV parse failed");
    wavFile.close();
    return false;
  }

  if (!configureI2SForWav(currentWav)) {
    wavFile.close();
    return false;
  }

  wavFile.seek(currentWav.dataOffset);
  currentTrack = path;
  audioPlaying = true;

  Serial.print("Started WAV: ");
  Serial.println(path);

  drawPlayMessage(path, ILI9341_YELLOW);
  return true;
}

int comparePathsIgnoreCase(const String &left, const String &right) {
  size_t commonLength = min(left.length(), right.length());

  for (size_t i = 0; i < commonLength; i++) {
    char leftChar = (char)tolower((unsigned char)left.charAt(i));
    char rightChar = (char)tolower((unsigned char)right.charAt(i));

    if (leftChar < rightChar) return -1;
    if (leftChar > rightChar) return 1;
  }

  if (left.length() < right.length()) return -1;
  if (left.length() > right.length()) return 1;
  return 0;
}

bool findNextSoundtrackPath(const String &afterPath, String &nextPath) {
  deselectAllSPIDevices();
  File directory = SD.open(SOUNDTRACK_DIRECTORY);
  if (!directory || !directory.isDirectory()) {
    if (directory) directory.close();
    return false;
  }

  String firstPath = "";
  String pathAfterCurrent = "";

  File entry = directory.openNextFile();
  while (entry) {
    if (!entry.isDirectory()) {
      String filename = entry.name();
      int lastSlash = filename.lastIndexOf('/');
      if (lastSlash >= 0) filename = filename.substring(lastSlash + 1);

      String lowercaseFilename = filename;
      lowercaseFilename.toLowerCase();

      if (filename.length() > 0 && lowercaseFilename.endsWith(".wav")) {
        String path = String(SOUNDTRACK_DIRECTORY) + "/" + filename;

        if (firstPath.length() == 0 || comparePathsIgnoreCase(path, firstPath) < 0) {
          firstPath = path;
        }

        if ((afterPath.length() == 0 || comparePathsIgnoreCase(path, afterPath) > 0) &&
            (pathAfterCurrent.length() == 0 || comparePathsIgnoreCase(path, pathAfterCurrent) < 0)) {
          pathAfterCurrent = path;
        }
      }
    }

    entry.close();
    entry = directory.openNextFile();
  }

  directory.close();

  // When there is nothing after the current track, wrap to the first track.
  nextPath = (pathAfterCurrent.length() > 0) ? pathAfterCurrent : firstPath;
  return nextPath.length() > 0;
}

bool playNextSoundtrack() {
  if (!soundtrackPlaylistEnabled || audioPlaying) return false;

  String firstAttempt = "";

  while (true) {
    String nextPath;
    if (!findNextSoundtrackPath(lastSoundtrackPath, nextPath)) {
      Serial.print("No WAV files found in ");
      Serial.println(SOUNDTRACK_DIRECTORY);
      soundtrackPlaylistEnabled = false;
      return false;
    }

    // Stop after one complete pass if every WAV is unreadable or unsupported.
    if (firstAttempt.length() == 0) {
      firstAttempt = nextPath;
    } else if (nextPath.equalsIgnoreCase(firstAttempt)) {
      Serial.println("No playable soundtrack WAV files found");
      soundtrackPlaylistEnabled = false;
      return false;
    }

    lastSoundtrackPath = nextPath;
    if (startWavFromSD(nextPath.c_str())) return true;

    Serial.print("Skipping unplayable soundtrack: ");
    Serial.println(nextPath);
  }
}

bool startSoundtrackPlaylist() {
  lastSoundtrackPath = "";
  soundtrackPlaylistEnabled = true;
  return playNextSoundtrack();
}

void serviceAudio() {
  if (!audioPlaying) return;
  if (!wavFile) {
    stopWavPlayback();
    return;
  }

  uint32_t playedBytes = (uint32_t)(wavFile.position() - currentWav.dataOffset);
  if (playedBytes >= currentWav.dataSize) {
    Serial.print("Finished: ");
    Serial.println(currentTrack);
    stopWavPlayback();

    if (pendingTrack.length() > 0) {
      String next = pendingTrack;
      pendingTrack = "";
      if (!startWavFromSD(next.c_str())) onAudioPlaybackFinished();
    } else {
      onAudioPlaybackFinished();
    }
    return;
  }

  uint32_t bytesRemaining = currentWav.dataSize - playedBytes;
  size_t toRead = (bytesRemaining > AUDIO_CHUNK_SIZE) ? AUDIO_CHUNK_SIZE : bytesRemaining;

  deselectAllSPIDevices();
  int actuallyRead = wavFile.read(audioBuf, toRead);
  if (actuallyRead <= 0) {
    Serial.println("Read error during playback");
    stopWavPlayback();
    onAudioPlaybackFinished();
    return;
  }

  size_t bytesWritten = 0;
  i2s_write(I2S_NUM_0, audioBuf, actuallyRead, &bytesWritten, portMAX_DELAY);
}