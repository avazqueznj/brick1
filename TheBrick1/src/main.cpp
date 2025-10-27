/********************************************************************************************
 * CONFIDENTIAL AND PROPRIETARY
 * 
 * The Brick 1.0 
 * © [2025] [Alejandro Vazquez]. All rights reserved.
 * 
 * Portions of this software are based on LVGL (https://lvgl.io),
 * which is licensed under the MIT License.
 *
 ********************************************************************************************/

#include <Arduino.h>

#include "mbed.h"
#include <SDRAM.h>
#include <WiFi.h>

// configure lvgl
#include "Arduino_H7_Video.h"
#include "lvgl.h"
#include "Arduino_GigaDisplayTouch.h"
#include "src/ui.h"

#include <SPI.h>
#include <MFRC522.h>
#include "RTClib.h"

//-------------------------------

  #include "util.hpp"
  #include "comms.hpp"
  #include "domain.hpp"

      #include "screenClass.hpp"
      #include "loginScreen.hpp"
      #include "settingsScreen.hpp"

#include "state.hpp"

//-------------------------------


// RFID Pins
#define SS_PIN 10  // SDA pin on RC522
#define RST_PIN 9  // Back to D9 for RST

// managers
Arduino_H7_Video* Display = nullptr;
Arduino_GigaDisplayTouch* TouchDetector = nullptr;
RTC_DS3231* rtc = nullptr;
MFRC522* mfrc522 = nullptr;
stateManagerClass* stateManager = nullptr;  

// Misc
bool rtcUp = false;
bool startedUp = false;

// keypad config
const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {A0, A1, A2, A3};
byte colPins[COLS] = {A4, A5, A6, A7};
const int ANALOG_THRESHOLD = 900;
const int DEBOUNCE_MS = 300;
unsigned long lastPressTime = 0;


//=================================================================================


// LVGL pool guard (global)
static void* lvgl_sdram_pool = nullptr;
extern "C" void* lvgl_get_sdram_pool() {
  return lvgl_sdram_pool;  // Return whatever we reserved at setup
}

void getInternalHeapFreeBytes() {
  mbed_stats_heap_t heap_stats;
  mbed_stats_heap_get(&heap_stats);    
  const int total_internal_ram = 491520;  
  int heap_used = heap_stats.current_size;
  int heap_free = total_internal_ram - heap_used;
  if (heap_free < 0) heap_free = 0;
  Serial.print( "Heap free: " );
  Serial.print( heap_free );
  Serial.print( "   LVGL pool at 0x") ;
  Serial.println((uintptr_t)lvgl_sdram_pool, HEX);  
}


void setup() {

  

  // setup serial
  Serial.begin(9600);
  int serialWait = 0;
  while (!Serial) {    
    serialWait += 100;
    if (serialWait > 10) break;
    delay( 100 );
  } 


  Serial.println("Coming UP----------------->");

  // SDRAM -----------------------------
  Serial.println("SDRAM: begin...");
  bool sdram_ok = SDRAM.begin();
  Serial.print("SDRAM.begin() = ");
  Serial.println(sdram_ok ? "OK" : "FAIL");
  if (!sdram_ok) {
    Serial.println("SDRAM init failed! Will halt.");
    sosBlink();
  }

  // Let SDRAM settle
  delay(500);

  // Try to allocate big pool
  const size_t POOL_SIZE = 3U * 1024U * 1024U; // 3 MB
  Serial.print("Allocating LVGL pool of ");
  Serial.print(POOL_SIZE);
  Serial.println(" bytes...");
  for (int tries = 0; tries < 3; ++tries) {
    lvgl_sdram_pool = SDRAM.malloc(POOL_SIZE);
    if (lvgl_sdram_pool) break;
    Serial.println("SDRAM.malloc failed, retrying...");
    delay(100);
  }
  if (!lvgl_sdram_pool) {
    Serial.println("SDRAM.malloc failed after retries! HALT.");
    sosBlink();
  }
  Serial.print("LVGL pool at 0x");
  Serial.println((uintptr_t)lvgl_sdram_pool, HEX);
  // /SDRAM -----------------------------

  // screen setup
  pinMode(LED_BUILTIN, OUTPUT);   
  Serial.println("Disp");
  Display = new Arduino_H7_Video(800, 480, GigaDisplayShield);
  Display->begin();

  // touch sensor setup
  Serial.println("Touch");
  TouchDetector = new Arduino_GigaDisplayTouch();
  TouchDetector->begin();

  // start spi bus
  Serial.println("SPI");
  SPI.begin();

  //check the rfid reader on spi
  Serial.println("RFID");
  mfrc522 = new MFRC522(SS_PIN, RST_PIN);
  mfrc522->PCD_Init();
  mfrc522->PCD_DumpVersionToSerial();

  // start i2c clock
  Serial.println("RTC");
  rtc = new RTC_DS3231();
  if (!rtc->begin()) {
    Serial.println("Couldn't find RTC clock!!!!");
  } else {
    if (rtc->lostPower()) {
      Serial.println("RTC lost power, setting default time ...");
      rtc->adjust(DateTime(2025, 7, 4, 12, 0, 0));
    }
    rtcUp = true;
  }

  // Init keypad pins
  for (byte i = 0; i < ROWS; i++) {
    pinMode(rowPins[i], OUTPUT);
    digitalWrite(rowPins[i], LOW);
  }
  for (byte i = 0; i < COLS; i++) {
    pinMode(colPins[i], INPUT);
  }

  // done!!!!
  startedUp = true;
  Serial.println("Coming UP-----------------> Done!");

  delay(100);

  Serial.println("Start screens  ...");
  create_screens();           

  stateManager = new stateManagerClass();  
  stateManager->init();
  stateManager->setOrGetPendingScreenId( SCREEN_ID_LOGIN_SCREEN );

  try{
      std::vector<String> config = loadFromKVStore( "/kv/config" );
      domainManagerClass::getInstance()->parse( &config );
  }catch( const std::runtime_error& error ){
      Serial.println( error.what() );            
  }  


  Serial.println("Started !!!!");
}


//================================================================================================================================================
//================================================================================================================================================
//================================================================================================================================================

// ---- tunable cadences (ms) ----
const unsigned long RFID_MS = 500;     // RFID poll
const unsigned long RTC_MS  = 250;     // clock tick 250
const unsigned long KEYS_MS = 50;      // keypad scan
const unsigned long MEM_MS  = 3000;    // mem stats

// ---- timer state ----
unsigned long lastRfidAt = 0;
unsigned long lastRtcAt  = 0;
unsigned long lastKeysAt = 0;
unsigned long lastMemAt  = 0;

byte currentCardUID[20];
byte currentCardLength = 0;

void navigateTo(int screenId) {
    stateManagerClass::setOrGetPendingScreenId(screenId);
}


void loop() {

  // wait for start
  if (!startedUp) {
    delay(100);
    return;
  }

  stateManager->processPendingScreenTransition();

  delayBlink();  // 50MSEC *********************
  lv_timer_handler();
  ui_tick();

  unsigned long now = millis();

  // ---------------- mem stats ----------------
  if (now - lastMemAt >= MEM_MS) {
    getInternalHeapFreeBytes();
    lastMemAt = now;
  }

  // ---------------- RFID ----------------
  if (mfrc522 && (now - lastRfidAt >= RFID_MS)) {
    lastRfidAt = now;

    static unsigned long lastCardReadTime = 0;
    static byte lastUID[10];
    static byte lastUIDLength = 0;

    mfrc522->PCD_WriteRegister(mfrc522->TxControlReg, 0x83); // Field ON

    if (mfrc522->PICC_IsNewCardPresent() && mfrc522->PICC_ReadCardSerial()) {

      bool isSameCard = false;
      if (mfrc522->uid.size == lastUIDLength) {
        if (memcmp(mfrc522->uid.uidByte, lastUID, lastUIDLength) == 0) {
          if ((now - lastCardReadTime) < 3000UL) {
            isSameCard = true;
          }
        }
      }

      if (!isSameCard) {
        lastCardReadTime = now;
        lastUIDLength = mfrc522->uid.size;
        if (lastUIDLength > sizeof(lastUID)) lastUIDLength = sizeof(lastUID);
        memcpy(lastUID, mfrc522->uid.uidByte, lastUIDLength);

        stateManager->rfidEvent(mfrc522->uid.uidByte, mfrc522->uid.size);
      }

      mfrc522->PICC_HaltA();
      mfrc522->PCD_StopCrypto1();
    }

    mfrc522->PCD_WriteRegister(mfrc522->TxControlReg, 0x00); // Field OFF
  }

  // ---------------- RTC ----------------
  if (rtcUp && rtc && (now - lastRtcAt >= RTC_MS)) {
    lastRtcAt = now;    
    stateManager->clockTic( rtc->now() ); // make copy
  }

  // ---------------- keypad ----------------
  if (now - lastKeysAt >= KEYS_MS) {
    lastKeysAt = now;

    for (byte row = 0; row < ROWS; row++) {
      digitalWrite(rowPins[row], HIGH);
      for (byte col = 0; col < COLS; col++) {
        int val = analogRead(colPins[col]);
        if (val > ANALOG_THRESHOLD) {
          if (now - lastPressTime > DEBOUNCE_MS) {
            char key = hexaKeys[row][col];
            stateManager->keyboardEvent(String(key));
            lastPressTime = now;
          }
        }
      }
      digitalWrite(rowPins[row], LOW);
    }
  }

  
  
}