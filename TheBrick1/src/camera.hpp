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

private:

    // has
    OV7670 ov767x;
    Camera cam;
    FrameBuffer fb; 
    uint16_t* pixels;

    // uses

//---------------------------------------------------------------


private:
    cameraClass() : ov767x(),  cam(ov767x),  fb()           
    {   
        Serial.println("[LOG] Cam mem init ...");
            
        // Use the SDRAM directly, but keep the pointer honest
        uint8_t *raw_mem = (uint8_t *)SDRAM.malloc((640 * 480 * 2) + 32);
        if (!raw_mem) throw std::runtime_error("CRITICAL: SDRAM Malloc failed!");
        
        // Fixed address for the life of the program
        pixels = (uint16_t *)ALIGN_PTR((uintptr_t)raw_mem, 32);
        fb.setBuffer((uint8_t *)pixels);

        Serial.println("[LOG] Cam mem init ...  done!");
    }

public:


    static cameraClass& getInstance() {
        static cameraClass instance; 
        return instance;
    }

    void shootToSDRAM(){

        // prepare blue buffer
        // get blue -> cam chip faield
        // get green -> chip worked could not take photo
        // get trash -> something bad happened
        Serial.println("[LOG] Prepare blue buffer pic");
        for (int i = 0; i < 640 * 480; i++) { pixels[i] = GC9A01A_BLUE; }
        SCB_CleanDCache_by_Addr((uint32_t *)pixels, 640 * 480 * 2);
        Serial.println("[LOG] RAM white-washed with BLUE... done!");

        // start camera
        Serial.println("[LOG] Starting Camera hardware...");
        if (!cam.begin(CAMERA_R640x480, IMAGE_MODE, 10)) {
            throw std::runtime_error("HARDWARE ERROR: cam.begin failed!");
        }        
        Serial.println("[LOG] Starting Camera hardware... done!");

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

    }

    void renderPicFromSDRAM(){

        Serial.println("[LOG] Render pic.");

        // 8. LVGL Descriptor (RGB565 per lv_conf.h)
        static lv_img_dsc_t cam_img_desc;
        cam_img_desc.header.always_zero = 0;
        cam_img_desc.header.reserved    = 0;
        cam_img_desc.header.cf          = LV_IMG_CF_TRUE_COLOR; // 16-bit depth
        cam_img_desc.header.w           = 640;
        cam_img_desc.header.h           = 480;
        cam_img_desc.data_size          = 640 * 480 * 2;
        cam_img_desc.data               = (const uint8_t *)pixels;

        // 9. UI Widget Update
        static lv_obj_t * cam_img_obj = NULL;
        if (cam_img_obj == NULL) {
            cam_img_obj = lv_img_create(lv_scr_act());
            if (!cam_img_obj) throw std::runtime_error("LVGL ERROR: Image object creation failed!");
        }

        lv_img_set_src(cam_img_obj, &cam_img_desc);
        lv_obj_center(cam_img_obj);
        lv_obj_invalidate(cam_img_obj);

        Serial.println("[LOG] Render pic ... done");        
    }

    virtual ~cameraClass(){
    }
    

    class cameraSystem {
    private:
        // ... your existing singleton stuff and pixels buffer ...
        uint16_t* pixels; 
        const size_t bufferSize = 640 * 480 * 2; 

    public:


        /**
         * We don't swap bits here; we assume shootToSDRAM already did the HTONS.
         */
        void saveCurrentFrame(const String& path) {
            Serial.println("[CAM] Requested save to: " + path);
            
            // Ensure the data is pushed from CPU cache to SDRAM before saving
            SCB_CleanDCache_by_Addr((uint32_t *)pixels, bufferSize);
            
            // Call  utility helper
            saveQSPIFileFromSDRAM(path, (const uint8_t*)pixels, bufferSize);
            
            Serial.println("[CAM] Save complete.");
        }

        void loadFrameToPixels(const String& path) {
            Serial.println("[CAM] Requested load from: " + path);
            
            size_t actualLoaded = 0;
            
            // Load straight into our pixel memory
            loadQSPIFileToSDRAM(path, (uint8_t*)pixels, bufferSize, actualLoaded);

            // MANDATORY NJ RULE: Invalidate cache so the CPU/LVGL sees the new data 
            // that the QSPI driver just dropped into RAM.
            SCB_InvalidateDCache_by_Addr((uint32_t *)pixels, bufferSize);

            Serial.println("[CAM] Load complete. Buffer refreshed.");
        }
    };    



};

