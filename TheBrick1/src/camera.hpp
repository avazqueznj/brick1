/********************************************************************************************
 * CONFIDENTIAL AND PROPRIETARY
 * 
 * The Brick 1.0 
 * © [2025] [Alejandro Vazquez]. All rights reserved.
 * 
 ********************************************************************************************/

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

    // has


    // uses
    const uint16_t* getPixels(){
        return pixels;
    }

    size_t getPixelsSize(){
        return pixelsSize;
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
    uint8_t* jpegBuffer;        // Landing zone for finished JPG
    uint8_t* jpegWorkSpace;     // Encoder's scratchpad
    size_t lastJpegSize = 0;
    const size_t MAX_JPG_SIZE = 128 * 1024; // 128KB is plenty for 640x480


    // uses

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


    static cameraClass& getInstance() {
        static cameraClass instance; 
        return instance;
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


    void renderPicFromPixSDRAM() {
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

    size_t pixelsToJPG(int quality = 80) {
        Serial.println("[LOG] Starting JPEG Compression...");
        
        // 1. Open the encoder directly to your SDRAM landing zone
        // Based on JPEGENC.cpp: open(uint8_t *pOutput, int iBufferSize)
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
        // addFrame(pEncode, pPixels, iPitch)
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

void showJpegFromSDRAM() {
    Serial.println("[PIC] showJpegFromSDRAM (rendering from internal buffer)");

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
        // We know the Grinder is set to 640x480. We don't care if TJpgDec's 
        // getJpgSize is being picky about the header markers.
        uint16_t jw = 640; 
        uint16_t jh = 480;
        int16_t  x  = 0;
        int16_t  y  = 0;

        // Try to get size anyway for logging, but don't let it stop us
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
        Serial.println("CRITICAL ERROR IN showJpegFromSDRAM:");
        Serial.println(e.what());
        Serial.println("========================================");
    }

    Serial.println("[PIC] Render finished.");
}


    //---------------------------------------

};

