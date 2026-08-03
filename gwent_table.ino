#include <SPI.h>
#include <SD.h>
#include <FS.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <XPT2046_Touchscreen.h>
#include <driver/i2s.h>
#include <esp_system.h>
#include <MFRC522.h>
#include "shared_defs.h"

// =========================
// Pin setup
// =========================

// Shared SPI bus (SD card + both MFRC522 readers)
#define SPI_SCK   18
#define SPI_MISO  23
#define SPI_MOSI  19

// TFT
#define TFT_CS    5
#define TFT_DC    2
#define TFT_RST   4

// SD
#define SD_CS     13

// Touch (XPT2046)
#define TOUCH_CS  15
#define TOUCH_IRQ 34

// Audio I2S -> PCM5102
#define I2S_BCK   26
#define I2S_WS    27  //RCK
#define I2S_DOUT  25  // PCM5102 DIN

// RFID MFRC522
#define RFID_RST  14
#define RFID1_SS  22  // Upper reader SDA/SS //top down 4th
#define RFID2_SS  17  // Lower reader SDA/SS //top down 6th
//RFID3 21 //top down 5th
//RFID4 16 //top down 3rd
//RFID5 33 //top-down 2nd
//RFID6 32 //top-down 1st


// =========================
// Objects
// =========================

Adafruit_ILI9341 tft(&SPI, TFT_DC, TFT_CS, TFT_RST);
XPT2046_Touchscreen ts(TOUCH_CS, TOUCH_IRQ);
MFRC522 rfid1(RFID1_SS, RFID_RST);
MFRC522 rfid2(RFID2_SS, RFID_RST);

// =========================
// Globals
// =========================

bool sdOk = false;
bool touchOk = false;
bool audioOk = false;
bool rfid1Ok = false;
bool rfid2Ok = false;

const uint32_t TOUCH_POLL_INTERVAL_IDLE_MS = 120;
const uint32_t TOUCH_POLL_INTERVAL_AUDIO_MS = 220;
const uint32_t TOUCH_DRAW_INTERVAL_IDLE_MS = 120;
const uint32_t TOUCH_DRAW_INTERVAL_AUDIO_MS = 300;
const uint32_t RFID_POLL_INTERVAL_IDLE_MS = 150;
const uint32_t RFID_POLL_INTERVAL_AUDIO_MS = 300;
const uint32_t RFID_DRAW_INTERVAL_IDLE_MS = 150;
const uint32_t RFID_DRAW_INTERVAL_AUDIO_MS = 300;

String lastUID1 = "NONE";
String lastUID2 = "NONE";
int lastTouchScreenX = -1;
int lastTouchScreenY = -1;
bool touchPressedLatched = false;

uint32_t lastTouchMs = 0;
uint32_t lastTouchDrawMs = 0;
uint32_t lastRFID1Ms = 0;
uint32_t lastRFID2Ms = 0;
uint32_t lastRFIDDrawMs = 0;

int touchMinX = 200;
int touchMaxX = 3900;
int touchMinY = 200;
int touchMaxY = 3900;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.println("Starting TFT + SD + Touch + Audio + 2x RFID test...");

  pinMode(TFT_CS, OUTPUT);
  pinMode(SD_CS, OUTPUT);
  pinMode(TOUCH_CS, OUTPUT);
  pinMode(TOUCH_IRQ, INPUT);

  pinMode(RFID_RST, OUTPUT);
  pinMode(RFID1_SS, OUTPUT);
  pinMode(RFID2_SS, OUTPUT);

  deselectAllSPIDevices();

  SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
  deselectAllSPIDevices();

  tft.begin(10000000);
  tft.setRotation(1);
  tft.fillScreen(ILI9341_BLACK);
  printCentered("Initializing...", 100, ILI9341_WHITE, 2);

  sdOk = initSDCard();
  touchOk = initTouch();
  audioOk = initI2S();
  initRFIDReaders();

  startNewGame();
  Serial.println("Setup complete.");

  if (sdOk && audioOk) {
    if (SD.exists("/w1.wav")) {
      if (SD.exists("/w2.wav")) pendingTrack = "/w2.wav";
      startWavFromSD("/w1.wav");
    } else if (SD.exists("/w2.wav")) {
      startWavFromSD("/w2.wav");
    }
  }
}

void loop() {
  serviceAudio();

  uint32_t now = millis();

  if (touchOk && digitalRead(TOUCH_IRQ) != LOW) {
    touchPressedLatched = false;
  }

  if (touchOk && (now - lastTouchMs >= currentTouchPollInterval()) && digitalRead(TOUCH_IRQ) == LOW) {
    deselectAllSPIDevices();
    TS_Point p = ts.getPoint();

    if (!touchPressedLatched && now - lastTouchDrawMs >= currentTouchDrawInterval()) {
      handleTouchScreenPressed(p.x, p.y);
      lastTouchDrawMs = now;
      touchPressedLatched = true;
    }

    Serial.printf("Touch raw -> X:%d Y:%d Z:%d\n", p.x, p.y, p.z);
    lastTouchMs = now;
    return;
  }

  if (rfid1Ok && (now - lastRFID1Ms >= currentRFIDPollInterval())) {
    lastRFID1Ms = now;
    if (pollRFID(rfid1, lastUID1, "RFID1") && (now - lastRFIDDrawMs >= currentRFIDDrawInterval())) {
      handleRFIDCardScanned(0, lastUID1);
      lastRFIDDrawMs = now;
      return;
    }
  }

  if (rfid2Ok && (now - lastRFID2Ms >= currentRFIDPollInterval())) {
    lastRFID2Ms = now;
    if (pollRFID(rfid2, lastUID2, "RFID2") && (now - lastRFIDDrawMs >= currentRFIDDrawInterval())) {
      handleRFIDCardScanned(1, lastUID2);
      lastRFIDDrawMs = now;
    }
  }
}
