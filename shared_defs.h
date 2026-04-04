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

void drawPlayMessage(const char *msg, uint16_t color = ILI9341_WHITE);