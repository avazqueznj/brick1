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


// main switches 
#define FEATURE_RFID   1  

#include <Arduino.h>
#include "mbed.h"
#include <SDRAM.h>
#include <WiFi.h>
#include "Arduino_H7_Video.h" // configure lvgl
#include "lvgl.h"
#include "Arduino_GigaDisplayTouch.h"
#include "src/ui.h"
#include <SPI.h>
#if FEATURE_RFID
  #include <MFRC522.h>
#endif
#include "RTClib.h"
#include <TJpg_Decoder.h>

// jpeg decoding stuff
static uint16_t* jpg_fb = NULL;
static lv_img_dsc_t jpg_dsc;
static lv_obj_t* jpg_holder = NULL;
static const int JPG_W = 800;
static const int JPG_H = 480;
bool jpg_to_fb(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap) {
    if (jpg_fb == NULL) return 0;

    if (x < 0 || y < 0) return 0;
    if (x >= JPG_W || y >= JPG_H) return 0;

    int16_t w_i = (int16_t)w;
    int16_t h_i = (int16_t)h;

    if (x + w_i > JPG_W) w_i = JPG_W - x;
    if (y + h_i > JPG_H) h_i = JPG_H - y;

    for (int16_t row = 0; row < h_i; row++) {
        uint16_t* dst = jpg_fb + (y + row) * JPG_W + x;
        uint16_t* src = bitmap + row * w_i;
        memcpy(dst, src, (size_t)w_i * sizeof(uint16_t));
    }
    return 1;
}
static const size_t JPG_IO_BUF_SIZE = 400000; // ~400 KB
static uint8_t* jpg_io_buf = NULL;

//-------------------------------

#include <vector>
#include <deque>

// SDRAM
void* sdram_malloc(size_t size) {
    return SDRAM.malloc(size);
}
void sdram_free(void* ptr) {
    SDRAM.free(ptr);
}
template <typename T>
struct SDRAMAllocator {
    using value_type = T;

    SDRAMAllocator() noexcept {}

    template <class U> SDRAMAllocator(const SDRAMAllocator<U>&) noexcept {}

    T* allocate(std::size_t n) {
        void* p = sdram_malloc(n * sizeof(T));
        if (!p) {
            throw std::runtime_error("Vector SDRAM allocator failed!!!");
        }
        return static_cast<T*>(p);
    }

    void deallocate(T* p, std::size_t) noexcept {
        sdram_free(p);
    }
};

typedef std::vector<String, SDRAMAllocator<String>> SDRAMVector;

//----------------------------------

#include "QSPIFBlockDevice.h"
#include "FATFileSystem.h"
#include <cstdio> // For C file I/O
QSPIFBlockDevice qspi;
mbed::FATFileSystem fs("qspi"); // Mount point is "/qspi/"

//-------------------------------
#define NUM_INSPECTION_SLOTS 10

  #include "util.hpp"
  #include "disk.hpp"
  #include "comms.hpp"
  #include "domain.hpp"
  #include "camera.hpp"
  #include "screenClass.hpp"

      #include "loginScreen.hpp"
      #include "settingsScreen.hpp"
      #include "mainScreen.hpp"
      #include "selectAssetScreen.hpp"      
      #include "selectInspectionTypeScreen.hpp"  
      #include "inspectionFormScreen.hpp"    
      #include "inspectionZonesScreen.hpp"
      #include "inspectionHistoryScreen.hpp"

#include "stateManager.hpp"




// machine to machin token
String BEARER_TOKEN = "";

// RFID Pins
#define SS_PIN 10  // SDA pin on RC522
#define RST_PIN 9  // Back to D9 for RST

// managers
Arduino_H7_Video* Display = nullptr;
Arduino_GigaDisplayTouch* TouchDetector = nullptr;
RTC_DS3231* rtc = nullptr;
#if FEATURE_RFID
  MFRC522* mfrc522 = nullptr;
#endif
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
    sosHALT("SDRAM init failed! Will halt.");
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
    sosHALT("SDRAM.malloc failed after retries! HALT.");
  }
  Serial.print("LVGL pool at 0x");
  Serial.println((uintptr_t)lvgl_sdram_pool, HEX);
  // /SDRAM -----------------------------

  // screen setup
  pinMode(LED_BUILTIN, OUTPUT);   
  Serial.println("Disp");
  Display = new Arduino_H7_Video(800, 480, GigaDisplayShield);
  if( Display->begin() != 0 ){
    sosHALT( "Display did not initialize!!!" );
  };

  // touch sensor setup
  Serial.println("Touch");
  TouchDetector = new Arduino_GigaDisplayTouch();  
  if( !TouchDetector->begin() ){
    sosHALT( "Touch sensor did not initialize!!!" );
  };


  // start spi bus
  Serial.println("SPI");  
  SPI.begin();

  //check the rfid reader on spi

  #if FEATURE_RFID
    Serial.println("RFID");
    mfrc522 = new MFRC522(SS_PIN, RST_PIN);
    mfrc522->PCD_Init();
    mfrc522->PCD_DumpVersionToSerial();
  #endif

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

    // seed random generator with RTC time
    DateTime now = rtc->now();
    uint32_t seed = now.unixtime(); 
    seed ^= millis();
    randomSeed(seed);

    Serial.print("Random seeded with: ");
    Serial.println(seed);    
  }

  // Init keypad pins
  for (byte i = 0; i < ROWS; i++) {
    pinMode(rowPins[i], OUTPUT);
    digitalWrite(rowPins[i], LOW);
  }
  for (byte i = 0; i < COLS; i++) {
    pinMode(colPins[i], INPUT);
  }

  // JPEG decoder memory
  jpg_io_buf = (uint8_t*) SDRAM.malloc(JPG_IO_BUF_SIZE);
  if (!jpg_io_buf) {
      sosHALT("[PIC] FATAL: cannot allocate jpg_io_buf in SDRAM");      
  }
  Serial.print("[PIC] Allocated jpg_io_buf, size = ");
  Serial.println(JPG_IO_BUF_SIZE);

  // JPEG framebuffer in SDRAM
  Serial.println("Allocating JPEG framebuffer...");
  size_t jpg_bytes = (size_t)JPG_W * (size_t)JPG_H * 2;
  jpg_fb = (uint16_t*)SDRAM.malloc(jpg_bytes);
  if (jpg_fb == NULL) {
    Serial.println("JPEG framebuffer alloc failed! HALT.");
    sosHALT("JPEG framebuffer alloc failed! HALT.");
  }
  Serial.print("JPEG framebuffer at 0x");
  Serial.println((uintptr_t)jpg_fb, HEX);

  // LVGL descriptor pointing at that buffer
  jpg_dsc.header.always_zero = 0;
  jpg_dsc.header.w = JPG_W;
  jpg_dsc.header.h = JPG_H;
  jpg_dsc.header.cf = LV_IMG_CF_TRUE_COLOR;
  jpg_dsc.data_size = (uint32_t)jpg_bytes;
  jpg_dsc.data = (const uint8_t*)jpg_fb;

  // TJpg_Decoder config
  TJpgDec.setCallback(jpg_to_fb);
  TJpgDec.setJpgScale(1);
  TJpgDec.setSwapBytes(false);

  // done!!!!
  startedUp = true;
  Serial.println("Coming UP-----------------> Done!");

  delay(100);

  Serial.println("Start screens  ...");
  create_screens();           

  try{
    std::vector<arduino::String> savedToken = loadFromKVStore( "/kv/token" );
    BEARER_TOKEN = savedToken[0];
  }catch(...){
    Serial.println( "ERROR could not load token" );
  }

  stateManager = new stateManagerClass();  
  stateManager->init();
  stateManager->setOrGetPendingScreenId( SCREEN_ID_LOGIN_SCREEN );

  try{
      std::vector<String> config = readTextVecFromQSPI( "/qspi/brickconfig.txt" );
      domainManagerClass::getInstance()->parse( &config );
  }catch( const std::runtime_error& error ){
      Serial.println( error.what() );            
  }  


  //_-------------------------------------------------------------------

  Serial.println("Started !!!!");
}


//================================================================================================================================================
//================================================================================================================================================
//================================================================================================================================================

// voltaje check
#define VOLTAGE_PIN A10
// Divider resistors: R1 = 30k (high side), R2 = 7.5k (low side)
const float R1 = 30000.0;
const float R2 = 7500.0;
const float VREF = 3.3; // volts
const int maxRaw = 250;  // 
const int minRaw = 195;  // 
const int battSamples = 60;

static unsigned long batteryStabilizeStart = 0;
static unsigned int stabilizeWaitTime = 5000;

// ---- tunable cadences (ms) ----
const unsigned long RFID_MS = 500;     // RFID poll
const unsigned long RTC_MS  = 250;     // clock tick 250
const unsigned long KEYS_MS = 50;      // keypad scan
const unsigned long MEM_MS  = 3000;    // mem stats
const unsigned long SERIAL_POLL_MS = 500;  // telnet scan


// ---- timer state ----
unsigned long lastRfidAt = 0;
unsigned long lastRtcAt  = 0;
unsigned long lastKeysAt = 0;
unsigned long lastMemAt  = 0;
unsigned long lastSerialPollAt = 0;

byte currentCardUID[20];
byte currentCardLength = 0;

void navigateTo(int screenId) {
    stateManagerClass::setOrGetPendingScreenId(screenId);
}

void configChanged() {
    stateManager->saveSettingsFile();
}

void fireModalDialogEvent( String action, String button ){
  stateManager->modalDialogEvent( action, button );
}

void loop() {

  // wait for start
  if (!startedUp) {
    delay(100);
    return;
  }

  try{

    stateManager->processPendingScreenTransition();

    delayBlink();  // 50MSEC *********************
    lv_timer_handler();
    ui_tick();

    unsigned long now = millis();

    // ---------------- mem stats ----------------
    if (now - lastMemAt >= MEM_MS) {

      // memory
      getInternalHeapFreeBytes();
      
      //--------
      lastMemAt = now;
    }

    // ---------------- RFID ----------------
    #if FEATURE_RFID
        if (
            ( 
              ( stateManager->currentScreenState )
              &&
              ( stateManager->currentScreenState->screenId == SCREEN_ID_SELECT_ASSET_SCREEN 
              || 
              stateManager->currentScreenState->screenId == SCREEN_ID_INSPECTION_ZONES )
            )

            && 

            mfrc522 && (now - lastRfidAt >= RFID_MS)
        ) {

          lastRfidAt = now;

          static unsigned long lastCardReadTime = 0;
          static byte lastUID[10];
          static byte lastUIDLength = 0;

          mfrc522->PCD_WriteRegister(mfrc522->TxControlReg, 0x83); // Field ON

          delay( 10 );

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

              // Build tag string in your style
              String data = "RFID event [";
              for (byte i = 0; i < mfrc522->uid.size; i++) {
                  data += ":";
                  data += String(mfrc522->uid.uidByte[i]);
              }
              data += "]";
              Serial .println( data );

              stateManager->rfidEvent(mfrc522->uid.uidByte, mfrc522->uid.size);
            }

            mfrc522->PICC_HaltA();
            mfrc522->PCD_StopCrypto1();
          }

          mfrc522->PCD_WriteRegister(mfrc522->TxControlReg, 0x00); // Field OFF
        }
    #endif

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

    //-------------------  commands

  if (now - lastSerialPollAt >= SERIAL_POLL_MS) {
    lastSerialPollAt = now;


      //=============================================================
      // battery moni
      static int samples[battSamples] = {0};
      static int idx = 0;
      static bool filled = false;

      // Set the stabilization start time only once
      if (batteryStabilizeStart == 0) {  
          batteryStabilizeStart = millis();
      }

      int raw = analogRead(VOLTAGE_PIN);
      samples[idx++] = raw;
      if (idx >= battSamples) {
        idx = 0;
        filled = true;
      }

      // Find the highest value in the array
      int maxSeen = samples[0];
      int count = filled ? battSamples : idx;
      for (int i = 1; i < count; i++) {
        if (samples[i] > maxSeen) maxSeen = samples[i];
      }

      float v_s = maxSeen * (VREF / 1023.0);
      float v_batt = v_s * (R1 + R2) / R2;
      float percent = 100.0 * (maxSeen - minRaw) / (maxRaw - minRaw);
      if (percent > 100.0) percent = 100.0;
      if (percent < 0.0) percent = 0.0;

      // Quantize percent to nearest lower 10 for UI
      int percent10 = ((int)percent / 10) * 10;

      if (millis() - batteryStabilizeStart >=  stabilizeWaitTime ) { 
        stateManager->batteryInfo( String(v_batt) + "v \uF242  "+ String(percent10) + "%");
      }else{
        stateManager->batteryInfo( "      ");
      }
      //=============================================================



      if (Serial.available()) {

          // read
          String cmd = "";
          while (Serial.available()) {
              char c = Serial.read();
              if (c == '\n' || c == '\r') break;
              cmd += c;
          }

          // do
          cmd.trim();
          if (cmd.length() > 0) {

              if (cmd == "show config") {
                Serial.println("===== SHOW CONFIG =====");
                domainManagerClass::getInstance()->printDebugContents();               
              } else
              if (cmd == "delete config") {
                Serial.println("===== delete CONFIG =====");
                const std::vector<String> empty;
                clearTextVecQSPI( "/qspi/brickconfig.txt" );  
                domainManagerClass::getInstance()->emptyAll();
                Serial.println("*** WARNING: THIS REQUIRES RESET DEVICE ***");
              } else


              if (cmd == "delete old config") {
                Serial.println("===== delete old CONFIG =====");
                const std::vector<String> empty;
                saveToKVStore( "/kv/config", &empty );     
                domainManagerClass::getInstance()->emptyAll();
                Serial.println("*** WARNING: THIS REQUIRES RESET DEVICE ***");
              } else              
              

              if (cmd == "show settings") {              
                Serial.println("===== SHOW SETTINGS =====");
                Serial.println("===== in file");
                for (const auto& kv : stateManager->settings ) {
                    Serial.print(kv.first); Serial.print(" = "); Serial.println(kv.second);
                }
                Serial.println("===== loaded");              
                stateManager->printLoadedSettings();
                Serial.println("=================");
              }else              
              if (cmd == "reset settings") {
                Serial.println("===== RESET SETTINGS  =====");
                stateManager->resetSettingsFile();
              }else
                        
              
              if (cmd == "show inspection") {
                Serial.println("===== SHW INSPECTION  =====");
                Serial.println( domainManagerClass::getInstance()->currentInspection.toEDI() );
              } else
              if (cmd == "show human inspection") {
                Serial.println("===== SHW HUMAN INSPECTION  =====");
                Serial.println( domainManagerClass::getInstance()->currentInspection.toHumanString() );
              } else
              

            if (cmd == "show history") {
                Serial.println("===== HISTORY  =====");
                getInspectionHistory();
                Serial.println("===== HISTORY  =====");
              } else
            if (cmd == "zap history") {
                Serial.println("===== ZAP HISTORY  =====");
                zapInspectionHistory();
              } else

            if (cmd.indexOf("set token") == 0) {
                Serial.println("===== SET TOKEN =====");
                BEARER_TOKEN = cmd.substring(9);
                saveToKVStore( "/kv/token" , BEARER_TOKEN + "\n" );
                delay( 1000 );
                std::vector<arduino::String> savedToken = loadFromKVStore( "/kv/token" );
                Serial.println( "B[" + BEARER_TOKEN + "]" );
                Serial.println( "S[" + savedToken[0] + "]" );
              } else

              if (cmd.indexOf("show token") == 0) {
                Serial.println("===== (SHOW TOKEN) =====");
                std::vector<arduino::String> savedToken = loadFromKVStore( "/kv/token" );
                Serial.println( "S[" + savedToken[0] + "]" );
                Serial.println( "B[" + BEARER_TOKEN + "]" );
              } else

              if (cmd.indexOf("list qspi") == 0) {
                Serial.println("===== list qspi =====");
                
                // PART SIZE -----------------------------------
                Serial.println("QSPI  PARTITION TEST (C I/O ONLY)");
                // 1. Query partition size
                int ret = qspi.init();
                if (ret != 0) {
                  Serial.println("QSPI init failed: " + ret ); 
                  Serial.println("preloaded ??");
                }
                uint64_t partSize = qspi.size();
                Serial.print("QSPI User Partition Size: ");
                Serial.print(partSize);
                Serial.println(" bytes");
                qspi.deinit(); //?? ok - safe
                // 2. Try to mount as filesystem

                // LIST-----------------------------------                
                int err = fs.mount(&qspi);
                if (err) {
                  //sosBlink("Mount failed (code: " + err); 
                  Serial.println("Err mounted ?");
                }
                Serial.println("User partition mounted as /qspi/");
                // 3. List directory before write
                listQSPIFiles("/qspi/");
                Serial.println("===== list qspi DONE! =====");\
              } else

              
              if (cmd == "zap images") {
                  Serial.println("===== ZAP IMAGES =====");
                  domainManagerClass::getInstance()->zapPics();
                  Serial.println("===== ZAP IMAGES DONE =====");
                } else

              if (cmd == "zap kv") {
                  Serial.println("===== ZAP KV =====");
                  zapKVStore();
                  Serial.println("===== ZAP KV =====");
                } else

              if (cmd == "?") {
                Serial.println("===== HELP =====");
                
                Serial.println("show config");
                Serial.println("delete config");

                Serial.println("show settings");
                Serial.println("reset settings");              
                
                Serial.println("show inspection");
                Serial.println("show human inspection");

                Serial.println("show history");
                Serial.println("zap history");

                Serial.println("set token{token}");
                //Serial.println("show token"); redacted

                Serial.println("list qspi");
                Serial.println("zap images"); // from qspi

                Serial.println("zap kv"); 
                
              } else         

              {
                Serial.println( "*** Unknown brick command  ***" );
              }
          }


      }
    }

  }catch(const std::exception& e) {  
    while(true){
      sosHALT(  "FATAL: " + String( e.what() )  );
    }
  }
      

}






