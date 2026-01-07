/********************************************************************************************
 * CONFIDENTIAL AND PROPRIETARY
 * * The Brick 1.0 
 * © [2025] [Alejandro Vazquez]. All rights reserved.
 * ********************************************************************************************/

#pragma once

#include <Arduino.h>
#include "misc/lv_color.h"
#include "lvgl.h"
#include <functional>
#include "mbed.h"
#include <map>
#include <vector>
#include "arducam_dvp.h"
#include "OV7670/ov767x.h"
#include <JPEGENC.h>

extern "C" void add_style_button_default(lv_obj_t *obj);

// --- PRODUCTION ADDRESS MAP (Part 4) ---
#define WAREHOUSE_START    0x700000   // Partition 4 Start (7MB mark)
#define PIC_SLOT_SIZE      0x20000    // 128KB per Slot (Unified for all JPEGs)


// --- WAREHOUSE ENUMS ---
enum BucketType : uint32_t {
    BT_EMPTY      = 0,
    BT_USER_PIC   = 1, // Taken by driver, needs sync
    BT_LAYOUT_PIC = 2  // Persistent vehicle layout
};

struct WarehouseItem {
    int slotIndex;
    String uuid;
    BucketType type;
    uint32_t length;
};

// --- WAREHOUSE HEADER DEFINITION ---
// Forced to 64 bytes for clean alignment
struct BucketHeader {
    uint32_t length;      // 4 bytes: Actual JPEG size
    uint32_t type;        // 4 bytes: Using BucketType (User vs Layout)
    char uuid[36];        // 36 bytes: UUID String
    char terminal;        // 1 byte: OVERFLOW STOPPER (Forced 0)
    uint8_t padding[19];  // 19 bytes: Padding to reach 64
};



#define IMAGE_MODE CAMERA_RGB565
#define GC9A01A_CYAN 0x07FF
#define GC9A01A_RED 0xf800
#define GC9A01A_BLUE 0x001F
#define GC9A01A_GREEN 0x07E0
#define GC9A01A_MAGENTA 0xF81F
#define GC9A01A_WHITE 0xffff
#define GC9A01A_BLACK 0x0000
#define GC9A01A_YELLOW 0xFFE0
#define ALIGN_PTR(p,a)   ((p & (a-1)) ?(((uintptr_t)p + a) & ~(uintptr_t)(a-1)) : p)
#define HTONS(x) (((x >> 8) & 0x00FF) | ((x << 8) & 0xFF00))

class cameraClass {
public:

    // uses
    const uint16_t* getPixels(){
        return pixels;
    }

    size_t getPixelsSize(){
        return pixelsSize;
    }

    uint8_t* getJpegBuffer(){
        return jpegBuffer;
    }

    size_t getLastJpegSize(){
        return lastJpegSize;
    }    

    void setLastJpegSize(size_t size) {
        // 1. Minimum sanity check: No zero-byte ghosts
        if (size == 0) {
            throw std::runtime_error("WAREHOUSE EXCEPTION: Attempted to set lastJpegSize to 0!");
        }

        // 2. Maximum sanity check: Must fit in our 128KB slot (minus 64-byte header)
        if (size > (PIC_SLOT_SIZE - 64)) {
            Serial.print("[CRITICAL] Oversized JPEG: "); Serial.println(size);
            throw std::runtime_error("WAREHOUSE EXCEPTION: JPEG size exceeds allocated slot capacity!");
        }

        // 3. Set the value
        lastJpegSize = size;

        // 4. Log it for the Janitor
        Serial.print("[WAREHOUSE] lastJpegSize updated: "); 
        Serial.print(size); 
        Serial.println(" bytes.");
    }

private:

    // has
    OV7670 ov767x;
    Camera cam;
    FrameBuffer fb; 
    uint16_t* pixels;
    size_t pixelsSize = (640 * 480 * 2) + 32;

    // JPEG stuff
    JPEGENC jpg;
    uint8_t* jpegWorkSpace;     // Encoder's scratchpad
    uint8_t* jpegBuffer;        // Landing zone for finished JPG
    size_t lastJpegSize = 0;
    const size_t MAX_JPG_SIZE = 128 * 1024; // 128KB is plenty for 640x480

    String currentPK;

//---------------------------------------------------------------

private:
    cameraClass() : ov767x(),  cam(ov767x),  fb()           
    {   
        Serial.println("[LOG] Cam mem init ...");
            
        // Use the SDRAM directly, but keep the pointer honest
        uint8_t *raw_mem = (uint8_t *)SDRAM.malloc( pixelsSize );
        if (!raw_mem) throw std::runtime_error("CRITICAL: SDRAM Malloc failed!");    
        pixels = (uint16_t *)ALIGN_PTR((uintptr_t)raw_mem, 32);
        fb.setBuffer((uint8_t *)pixels);

        // JPEG ==================
        // 2. JPEG Landing Buffer (128KB)
        jpegBuffer = (uint8_t *)SDRAM.malloc(MAX_JPG_SIZE);
        if (!jpegBuffer) throw std::runtime_error("CRITICAL: SDRAM Malloc failed for JPEG Buffer!");

        // 3. JPEG Workspace (The Encoder's "Engine Room" - ~32KB)
        jpegWorkSpace = (uint8_t *)SDRAM.malloc(32000);
        if (!jpegWorkSpace) throw std::runtime_error("CRITICAL: SDRAM Malloc failed for Workspace!");
        // JPEG ==================         

        // start camera ==============================================
        Serial.println("[LOG] Starting Camera hardware...");
        if (!cam.begin(CAMERA_R640x480, IMAGE_MODE, 10)) {
            throw std::runtime_error("HARDWARE ERROR: cam.begin failed!");
        }        
        Serial.println("[LOG] Starting Camera hardware... done!");        
        
    }



public:


    static cameraClass* getInstance() {
        static cameraClass instance; 
        return &instance;
    }

    void shootToPixSDRAM(){

        // prepare blue buffer
        // get blue -> cam chip faield
        // get green -> chip worked could not take photo
        // get trash -> something bad happened
        Serial.println("[LOG] Prepare blue buffer pic");
        for (int i = 0; i < 640 * 480; i++) { pixels[i] = GC9A01A_BLUE; }
        SCB_CleanDCache_by_Addr((uint32_t *)pixels, 640 * 480 * 2);
        Serial.println("[LOG] RAM white-washed with BLUE... done!");

        // let it figure the picture exposure and all that
        Serial.println("[LOG] Pumping frames to adjust AEC/AGC...");
        for(int i = 0; i < 10; i++) {
            // If this returns -1, the DCMI is timed out.
            if (cam.grabFrame(fb, 1000) != 0) {
                Serial.print("[WARN] Adjustment ERROR at frame "); Serial.println(i);
            }
            Serial.print(".");
        }        
        Serial.println("\n[LOG] Pumping frames to adjust AEC/AGC... done!");


        // 5. The Shot
        Serial.println("[LOG] Grabbing Frame...");
        int status = cam.grabFrame(fb, 3000);
        if (status != 0) {
            Serial.print("[ERROR] grabFrame Status: "); Serial.println(status);
            throw std::runtime_error("CAMERA ERROR: Failed to capture frame data.");
        }
        Serial.println("[LOG] Grab Success.");

        // 6. Fix Endianness (HTONS) in buffer
        Serial.println("[LOG] fix buffer bits ... endianess");        
        for (int i = 0; i < 640 * 480; i++) {
            pixels[i] = HTONS(pixels[i]);
        }

        // flush
        // 7. Flush Cache for M7 DMA/LTDC
        SCB_CleanDCache_by_Addr((uint32_t *)pixels, 640 * 480 * 2);
        Serial.println("[LOG] Cache sync complete.");

        // --   
    }

    // ----------------------------------------


    void renderPicFromPixSDRAMXXX() {
        Serial.println("[LOG] Render pic.");

        // 8. LVGL Descriptor
        static lv_img_dsc_t cam_img_desc;
        cam_img_desc.header.always_zero = 0;
        cam_img_desc.header.reserved    = 0;
        cam_img_desc.header.cf          = LV_IMG_CF_TRUE_COLOR;
        cam_img_desc.header.w           = 640;
        cam_img_desc.header.h           = 480;
        cam_img_desc.data_size          = 640 * 480 * 2;
        cam_img_desc.data               = (const uint8_t *)pixels;

        // 9. UI Widget Update
        static lv_obj_t * cam_img_obj = NULL;
        static lv_obj_t * btn_dismiss = NULL;

        if (cam_img_obj == NULL) {
            // Create the image on top layer (The "Shield")
            cam_img_obj = lv_img_create(lv_layer_top());
            if (!cam_img_obj) throw std::runtime_error("LVGL ERROR: Image object creation failed!");

            lv_img_set_src(cam_img_obj, &cam_img_desc);
            lv_obj_center(cam_img_obj);
            
            // Block touches from leaking to screens below
            lv_obj_add_flag(cam_img_obj, LV_OBJ_FLAG_CLICKABLE);
            
            // 10. The EEZ-Styled Dismiss Button
            btn_dismiss = lv_btn_create(lv_layer_top()); 
            add_style_button_default(btn_dismiss); // Apply the EEZ state styles
            
            lv_obj_set_size(btn_dismiss, 180, 65); // Sized for your Montserrat 28 font
            lv_obj_align(btn_dismiss, LV_ALIGN_BOTTOM_MID, 0, -30);
            
            lv_obj_t * label = lv_label_create(btn_dismiss);
            lv_label_set_text(label, " ok ");
            lv_obj_center(label);
            lv_obj_set_style_text_font(label, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);

            // Button Lambda
            lv_obj_add_event_cb(btn_dismiss, [](lv_event_t * e) {
                lv_obj_add_flag(cam_img_obj, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(btn_dismiss, LV_OBJ_FLAG_HIDDEN);
                Serial.println("[UI] View dismissed.");
            }, LV_EVENT_PRESSED, NULL);
        }

        // Force Show and Move to Front
        lv_obj_clear_flag(cam_img_obj, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(btn_dismiss, LV_OBJ_FLAG_HIDDEN);
        
        // Z-Order: Image first, then Button on top of it
        lv_obj_move_foreground(cam_img_obj);
        lv_obj_move_foreground(btn_dismiss);

        // Refresh display
        lv_img_cache_invalidate_src(&cam_img_desc);
        lv_obj_invalidate(cam_img_obj);

        Serial.println("[LOG] Render pic ... done");        
    }

    size_t encodePixelsToJPG(int quality = 80) {
        Serial.println("[LOG] Starting JPEG Compression...");
        
        // 1. Open the encoder directly to your SDRAM landing zone
        int rc = jpg.open(jpegBuffer, (int)MAX_JPG_SIZE);
        if (rc != JPEGE_SUCCESS) {
            Serial.println("[ERROR] JPEG Open failed!");
            return 0;
        }

        // 2. Initialize the encode process
        JPEGENCODE jpe; 
        // Quality: 0=Best, 1=High, 2=Med, 3=Low
        uint8_t q = JPEGE_Q_HIGH;
        if (quality < 50) q = JPEGE_Q_MED;

        rc = jpg.encodeBegin(&jpe, 640, 480, JPEGE_PIXEL_RGB565, JPEGE_SUBSAMPLE_444, q);
        if (rc != JPEGE_SUCCESS) {
            Serial.print("[ERROR] encodeBegin failed: "); Serial.println(rc);
            return 0;
        }

        // 3. Encode the frame from your pixels SDRAM
        rc = jpg.addFrame(&jpe, (uint8_t *)pixels, 640 * 2);
        if (rc != JPEGE_SUCCESS) {
            Serial.println("[ERROR] addFrame failed!");
            return 0;
        }

        // 4. Close returns the final compressed size
        lastJpegSize = (size_t)jpg.close();
        
        Serial.print("[LOG] Compression done. Size: ");
        Serial.println(lastJpegSize);
        
        return lastJpegSize;
    }

    //---------------------------------------

    void renderJpegFromSDRAM( lv_obj_t* jpg_holder  ) {
        Serial.println("[PIC] renderJpegFromSDRAM (rendering from internal buffer)");

        try {
            // 1. Safety Check
            if (lastJpegSize == 0 || jpegBuffer == NULL) {
                throw std::runtime_error("NJ ERROR: No JPEG data available to show!");
            }

            // 2. Validate JPEG magic bytes (FF D8)
            if (!(jpegBuffer[0] == 0xFF && jpegBuffer[1] == 0xD8)) {
                Serial.println("[FATAL] Memory buffer is NOT a valid JPEG");
                return;
            }

            Serial.print("[PIC] Processing JPEG of size: ");
            Serial.println(lastJpegSize);

            // 3. Force Dimensions for Centering
            uint16_t jw = 640; 
            uint16_t jh = 480;
            int16_t  x  = 0;
            int16_t  y  = 0;

            uint16_t realW = 0, realH = 0;
            if (TJpgDec.getJpgSize(&realW, &realH, jpegBuffer, lastJpegSize)) {
                jw = realW;
                jh = realH;
                Serial.print("[PIC] Header Parse Success: ");
            } else {
                Serial.println("[WARN] Header parse failed - FORCING 640x480 for layout.");
            }

            // NJ Center Logic: Inside your 800x480 (JPG_W x JPG_H) frame
            if (jw < JPG_W) x = (int16_t)((JPG_W - jw) / 2);
            if (jh < JPG_H) y = (int16_t)((JPG_H - jh) / 2);

            Serial.print("[PIC] Dimensions: "); Serial.print(jw); Serial.print("x"); Serial.println(jh);
            Serial.print("[PIC] Center calculated: x="); Serial.print(x);
            Serial.print(" y="); Serial.println(y);

            // 4. Wipe the Framebuffer before decoding
            size_t fb_bytes = (size_t)JPG_W * (size_t)JPG_H * 2;
            memset(jpg_fb, 0, fb_bytes);

            // 5. The Decode: Blast from jpegBuffer into jpg_fb
            Serial.println("[PIC] Starting TJpgDec hardware blast...");
            TJpgDec.drawJpg(x, y, jpegBuffer, lastJpegSize); 
            Serial.println("[PIC] Decode complete.");

            // 6. LVGL UI Update
            if (jpg_holder == NULL) {
                jpg_holder = lv_img_create(lv_scr_act());
                lv_obj_add_flag(jpg_holder, LV_OBJ_FLAG_CLICKABLE);
                lv_obj_add_event_cb(
                    jpg_holder,
                    [](lv_event_t* e) {
                        lv_obj_t* obj = lv_event_get_target(e);
                        lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                        Serial.println("[UI] SDRAM Photo View Hidden.");
                    },
                    LV_EVENT_CLICKED,
                    NULL
                );
            }

            lv_img_set_src(jpg_holder, &jpg_dsc);
            lv_obj_center(jpg_holder);
            lv_obj_clear_flag(jpg_holder, LV_OBJ_FLAG_HIDDEN);
            lv_obj_move_foreground(jpg_holder);

        } catch (const std::exception& e) {
            Serial.println("========================================");
            Serial.println("CRITICAL ERROR IN renderJpegFromSDRAM:");
            Serial.println(e.what());
            Serial.println("========================================");
        }

        Serial.println("[PIC] Render finished.");
    }


    //---------------------------------------
    ///WAREHOUSE
    //---------------------------------------

    private:


    // NJ Style UUID Generator
    String generatePK() {
        uint8_t uuid[16];
        for (int i = 0; i < 16; ++i) uuid[i] = random(0, 256);
        uuid[6] = (uuid[6] & 0x0F) | 0x40; // Version 4
        uuid[8] = (uuid[8] & 0x3F) | 0x80; // Variant 1
        char buf[37];
        snprintf(buf, sizeof(buf),
            "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
            uuid[0], uuid[1], uuid[2], uuid[3], uuid[4], uuid[5], uuid[6], uuid[7],
            uuid[8], uuid[9], uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]);
        return String(buf);
    }

    int findBucketIndex(String targetUUID = "") {
        mbed::BlockDevice* bd = mbed::BlockDevice::get_default_instance();
        if (!bd) throw std::runtime_error("QSPI CRITICAL: BlockDevice is NULL");
        
        if (bd->init() != 0) throw std::runtime_error("QSPI INIT FAIL during scan");

        BucketHeader header;
        int foundIndex = -1;

        for (int i = 0; i < 64; i++) {
            uint32_t addr = WAREHOUSE_START + (i * PIC_SLOT_SIZE);
            bd->read((uint8_t*)&header, addr, sizeof(BucketHeader));
            header.terminal = 0; // Force null so String() doesn't explode if flash is dirty

            if (targetUUID == "") { 
                // Look for empty (erased flash is 0xFF)
                if (header.length == 0 || header.length == 0xFFFFFFFF) {
                    foundIndex = i;
                    break;
                }
            } else {
                // Match the UUID string via safe strncmp
                if (strncmp(targetUUID.c_str(), header.uuid, 36) == 0) {
                    foundIndex = i;
                    break;
                }
            }
        }
        bd->deinit();

        // NJ STYLE: No silent returns.
        if (foundIndex == -1) {
            if (targetUUID == "") {
                // This is a system-halting state for a 'Save' operation
                throw std::runtime_error(" EXCEPTION: SILICON IS FULL. NO SLOTS AVAILABLE.");
            } else {
                // This is a system-halting state for a 'Load' or 'Delete' operation
                Serial.print("[ERROR] UUID NOT FOUND: "); Serial.println(targetUUID);
                throw std::runtime_error(" EXCEPTION: PK NOT FOUND IN SILICON.");
            }
        }

        return foundIndex;
    }    

    public:

    // ======================================================================
    // SAVE: Finds a bucket, writes Header + Data, returns UUID
    // ======================================================================

    String saveJPGSDRAMToWarehouse(BucketType type = BT_USER_PIC) {
        return saveJPGSDRAMToWarehouse( generatePK(), type );
    }

    String saveJPGSDRAMToWarehouse(String newPK, BucketType type = BT_USER_PIC) {
        Serial.println("------------------------------------------");
        Serial.print("[WAREHOUSE] SAVE INITIATED - TYPE: "); 
        Serial.println(type == BT_LAYOUT_PIC ? "LAYOUT" : "USER");

        if (lastJpegSize == 0 || jpegBuffer == nullptr) {
            throw std::runtime_error("WAREHOUSE EXCEPTION: No image in RAM to save!");
        }

        int bucket = findBucketIndex(""); 
        
        BucketHeader header;
        memset(&header, 0, sizeof(header));
        header.length = (uint32_t)lastJpegSize;
        header.type = (uint32_t)type; 
        //String newPK = generatePK(); 
        strncpy(header.uuid, newPK.c_str(), 36);
        header.terminal = 0; 

        uint32_t targetAddr = WAREHOUSE_START + (bucket * PIC_SLOT_SIZE);
        
        Serial.print("[WAREHOUSE] Target Slot: "); Serial.println(bucket);
        Serial.print("[WAREHOUSE] Assigning PK: "); Serial.println(newPK);

        mbed::BlockDevice* bd = mbed::BlockDevice::get_default_instance();
        bd->init();
        
        if (bd->erase(targetAddr, PIC_SLOT_SIZE) != 0) throw std::runtime_error("QSPI ERASE FAIL");
        if (bd->program((uint8_t*)&header, targetAddr, 64) != 0) throw std::runtime_error("QSPI HEADER FAIL");
        if (bd->program(jpegBuffer, targetAddr + 64, header.length) != 0) throw std::runtime_error("QSPI DATA FAIL");

        bd->deinit();
        
        currentPK = newPK;
        Serial.println("[WAREHOUSE] SAVE SUCCESSFUL");
        Serial.println("------------------------------------------");
        return newPK; 
    }

    // ======================================================================
    // LOAD: Finds UUID in silicon and pulls the data into RAM
    // ======================================================================
    void loadJPGSDRAMFromWarehouse(String targetUUID) {
        Serial.println("------------------------------------------");
        Serial.print("[WAREHOUSE] SEARCHING FOR PK: "); Serial.println(targetUUID);

        int bucket = findBucketIndex(targetUUID);
        if (bucket == -1) throw std::runtime_error("WAREHOUSE EXCEPTION: PK not found!");

        uint32_t targetAddr = WAREHOUSE_START + (bucket * PIC_SLOT_SIZE);
        mbed::BlockDevice* bd = mbed::BlockDevice::get_default_instance();
        bd->init();

        BucketHeader header;
        bd->read((uint8_t*)&header, targetAddr, 64);
        header.terminal = 0; // Safety for String(header.uuid) conversion
        
        if (header.length > (PIC_SLOT_SIZE - 64)) {
            bd->deinit();
            throw std::runtime_error("WAREHOUSE EXCEPTION: Header corruption!");
        }

        lastJpegSize = (size_t)header.length;
        currentPK = String(header.uuid);

        // Read data starting at +64
        int rc = bd->read(jpegBuffer, targetAddr + 64, header.length);
        bd->deinit();

        if (rc != 0) throw std::runtime_error("QSPI READ FAIL");

        Serial.println("[WAREHOUSE] LOAD SUCCESSFUL");
        Serial.println("------------------------------------------");
    }


    // ======================================================================
    // ZAP: Locates a PK and wipes the silicon clean
    // ======================================================================
    void zapJPGfromWarehouse(String targetUUID) {
        Serial.println("------------------------------------------");
        Serial.print("[WAREHOUSE] ZAP REQUESTED FOR PK: "); Serial.println(targetUUID);

        if (targetUUID.length() < 36) {
            throw std::runtime_error("ZAP EXCEPTION: Invalid UUID provided");
        }

        // 1. Locate the physical bucket
        int bucket = findBucketIndex(targetUUID);
        if (bucket == -1) {
            Serial.print("[WARN] Zap failed - UUID not found in silicon: "); Serial.println(targetUUID);
            return; 
        }

        uint32_t targetAddr = WAREHOUSE_START + (bucket * PIC_SLOT_SIZE);
        
        Serial.print("[WAREHOUSE] Found PK in Slot: "); Serial.println(bucket);
        Serial.print("[WAREHOUSE] Nuking Address: 0x"); Serial.println(targetAddr, HEX);

        mbed::BlockDevice* bd = mbed::BlockDevice::get_default_instance();
        if (bd == NULL) throw std::runtime_error("QSPI CRITICAL: BD is NULL");

        int rc = bd->init();
        if (rc != 0) throw std::runtime_error("QSPI ZAP: Init failed");

        Serial.print("[QSPI] Erasing 128KB Slot...");
        rc = bd->erase(targetAddr, PIC_SLOT_SIZE);
        
        bd->deinit();

        if (rc != 0) {
            throw std::runtime_error("QSPI ZAP: Erase operation failed on silicon");
        }

        Serial.println("DONE");
        Serial.println("[WAREHOUSE] ZAP SUCCESSFUL - Slot is now available");
        Serial.println("------------------------------------------");
    }

    // ======================================================================
    // ZAP ALL: Scans every bucket and erases any that contain data
    // ======================================================================
    void zapAllWarehouseSlots() {
        Serial.println("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
        Serial.println("[WAREHOUSE] GLOBAL ZAP INITIATED");
        Serial.println("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");

        mbed::BlockDevice* bd = mbed::BlockDevice::get_default_instance();
        if (!bd) throw std::runtime_error("QSPI CRITICAL: BD is NULL");

        int rc = bd->init();
        if (rc != 0) throw std::runtime_error("QSPI GLOBAL ZAP: Init failed");

        int zappedCount = 0;
        BucketHeader header;

        for (int i = 0; i < 64; i++) {
            uint32_t targetAddr = WAREHOUSE_START + (i * PIC_SLOT_SIZE);
            
            bd->read((uint8_t*)&header, targetAddr, sizeof(BucketHeader));
            header.terminal = 0; // The Guardian

            Serial.print("[SLOT "); Serial.print(i); 
            Serial.print(" @ 0x"); Serial.print(targetAddr, HEX); Serial.print("]: ");

            if (header.length != 0 && header.length != 0xFFFFFFFF) {
                Serial.print("FOUND DATA. PK: "); Serial.print(header.uuid);
                Serial.print(" ("); Serial.print(header.length); Serial.println(" bytes)");
                
                Serial.print("   -> Erasing silicon...");
                rc = bd->erase(targetAddr, PIC_SLOT_SIZE);
                if (rc != 0) {
                    bd->deinit();
                    throw std::runtime_error(String( "QSPI GLOBAL ZAP: Erase failed at slot " + String(i) ).c_str());
                }
                Serial.println(" CLEANSED.");
                zappedCount++;
            } else {
                Serial.println("EMPTY. Skipping.");
            }
        }

        bd->deinit();

        Serial.println("------------------------------------------");
        Serial.print("[WAREHOUSE] GLOBAL ZAP COMPLETE. Total Slots Cleared: "); 
        Serial.println(zappedCount);
        Serial.println("------------------------------------------\n");
    }

    std::vector<WarehouseItem> getWarehouseInventory(uint32_t filterType = 0) {
        Serial.println("------------------------------------------");
        Serial.print("[WAREHOUSE] Scanning for Type: "); 
        if (filterType == 0) Serial.println("ALL");
        else Serial.println(filterType == BT_LAYOUT_PIC ? "LAYOUTS ONLY" : "USER PICS ONLY");

        std::vector<WarehouseItem> inventory;
        mbed::BlockDevice* bd = mbed::BlockDevice::get_default_instance();
        if (!bd) throw std::runtime_error("QSPI CRITICAL: BD is NULL");
        
        bd->init();
        BucketHeader header;

        for (int i = 0; i < 64; i++) {
            uint32_t targetAddr = WAREHOUSE_START + (i * PIC_SLOT_SIZE);
            bd->read((uint8_t*)&header, targetAddr, sizeof(BucketHeader));
            header.terminal = 0; 

            Serial.print("[SLOT "); 
            if (i < 10) Serial.print("0"); // Keep columns aligned
            Serial.print(i); Serial.print("] ");

            if (header.length != 0 && header.length != 0xFFFFFFFF) {
                // PESSIMISM: Check for impossible sizes (128KB max)
                if (header.length > (128 * 1024)) {
                    Serial.print("!! GARBAGE !! Length: "); Serial.print(header.length);
                    Serial.print(" PK: "); Serial.println(header.uuid);
                }
                // Check if it matches the filter
                else if (filterType == 0 || header.type == filterType) {
                    WarehouseItem item;
                    item.slotIndex = i;
                    item.uuid = String(header.uuid);
                    item.type = (BucketType)header.type;
                    item.length = header.length;
                    inventory.push_back(item);

                    Serial.print(item.type == BT_LAYOUT_PIC ? "[LAYOUT] " : "[USER]   ");
                    Serial.print("Len: "); Serial.print(item.length);
                    Serial.print(" PK: "); Serial.println(item.uuid);
                } 
                else {
                    // It's valid data, just not what we asked for
                    Serial.print("[FILTERED] Type: "); 
                    Serial.print(header.type == BT_LAYOUT_PIC ? "LAYOUT" : (header.type == BT_USER_PIC ? "USER" : "UNKNOWN"));
                    Serial.print(" PK: "); Serial.println(header.uuid);
                }
            } 
            else {
                // THE EMPTY CASE: 0 or 0xFFFFFFFF
                Serial.print(header.length == 0 ? "[EMPTY:ZAPPED] " : "[EMPTY:FRESH]  ");
                
                // Even if empty, let's see if a Ghost PK is lingering in the bytes
                if (header.uuid[0] != 0 && (uint8_t)header.uuid[0] != 0xFF) {
                    Serial.print("Ghost PK: "); Serial.println(header.uuid);
                } else {
                    Serial.println("--");
                }
            }
        }


        bd->deinit();
        Serial.print("[WAREHOUSE] Inventory Scan Complete. Found: "); Serial.println(inventory.size());
        return inventory;
    }

    
    int syncUserPics(commsClass* comms, String serverURL, String path ) {

        Serial.print("SYNC USER PICS ===================== ");

        try{

            spinnerStart();

            if (!comms) {
                throw std::runtime_error("JANITOR_FATAL: Null comms object passed to syncPics. Pipe is missing!");
                return 0;
            }

            // 1. Get the list of USER photos
            std::vector<WarehouseItem> pending = getWarehouseInventory(BT_USER_PIC);        
            if (pending.empty()){
                Serial.println("[JANITOR] Warehouse clean. No user pics to sync.");
                return 0;
            }

            Serial.print("[JANITOR] Found "); Serial.print(pending.size()); Serial.println(" items.");

            int syncedPics = 0;
            for (auto& item : pending) {
                try {
                    // 2. Load from Silicon into SDRAM (Sets jpegBuffer and lastJpegSize)
                    loadJPGSDRAMFromWarehouse(item.uuid);             
                    
                    uint8_t* rawData = getJpegBuffer();
                    size_t dataLen = getLastJpegSize();

                    // NJ Style: Validation first.
                    if (dataLen == 0 || dataLen > (128 * 1024)) {
                        Serial.print("[JANITOR] Skipping corrupt slot: "); Serial.println(item.uuid);
                        continue; 
                    }

                    // 3. The Upload 
                    // We use the credentials already held by the comms object
                    comms->POSTRawBinary(
                        serverURL, comms->ssid, comms->pass, path,
                        item.uuid, 
                        (uint8_t)item.type, 
                        rawData, 
                        dataLen
                    );

                    // 4. The ZAP (Only happens if POST didn't throw)
                    Serial.print("[JANITOR] ACK Received for: "); Serial.println(item.uuid);
                    zapJPGfromWarehouse(item.uuid);
                    syncedPics++;
                    
                } catch (const std::exception& e) {
                    // Log it and STOP. If one fails, the pipe is likely broken.
                    Serial.println("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
                    Serial.print("[JANITOR] SYNC FAILED: "); Serial.println(e.what());
                    Serial.println("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
                    break; 
                }
            }

            spinnerEnd();

            return syncedPics;

        }catch(...){
            spinnerEnd();
            throw;
        }
    }


};
