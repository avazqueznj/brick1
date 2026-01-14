/********************************************************************************************
 * CONFIDENTIAL AND PROPRIETARY
 * 
 * The Brick 1.0 
 * © [2025] [Alejandro Vazquez]. All rights reserved.
 * 
 ********************************************************************************************/

#include "arducam_dvp.h"
#include "OV7670/ov767x.h"


// global calls
void navigateTo(int screenId);
void configChanged();

class loginScreenClass:public screenClass{
public:
    

    loginScreenClass( settingsClass* settingsParam ): screenClass( settingsParam, SCREEN_ID_LOGIN_SCREEN ){            
    }

    void clockTic( String time ) override {
        lv_label_set_text( objects.clock_login, time.c_str());
    }

    void batteryInfo( String info ) override {
        lv_label_set_text( objects.battery_login, info.c_str());
    }

    void handleKeyboardEvent( String key ) override {
        screenClass::handleKeyboardEvent( key );

        lv_obj_t* focused = lv_group_get_focused(inputGroup);

        // add numeric input to focused text areas
        if( key != "A" && key != "B" && key != "C" && key != "D" && key != "*" && key != "#"  ){
           
            if (focused && lv_obj_check_type(focused, &lv_textarea_class)) {
                lv_textarea_add_text(focused, key.c_str());                    
            }
        }

        // use * as backspace
        if (key == "*") {
            if (focused && lv_obj_check_type(focused, &lv_textarea_class)) {
                String txt = lv_textarea_get_text(focused);  // copy the text
                int len = txt.length();
                if (len > 0) {
                    txt = txt.substring(0, len - 1);  // remove last character
                    //lv_textarea_set_text(focused, txt.c_str());
                    //lv_textarea_add_text(focused, "*" );   

                    // lvgl bug ??
                    // One for the ghost, one for the real char
                    lv_textarea_del_char( focused );
                    lv_textarea_del_char( focused );                                         
                }
            }
        }  


        if( 
            ( focused == objects.login && key == "#" ) ||
            ( focused == objects.login_password && key == "#" )
        ){
            try{
                if(
                        domainManagerClass::getInstance()->login(    
                            String( lv_textarea_get_text( objects.login_username ) ),
                            String( lv_textarea_get_text( objects.login_password ) ) 
                            )                    
                    ){
                        navigateTo( SCREEN_ID_MAIN );
                    }else{
                        showDialog( "Invalid credentials" );  
                    }
            }catch( std::runtime_error& error ){
                showDialog( error.what() );  
            }                

        }

        if( focused == objects.do_sync_2  && key == "#"  ){
            try{
                showDialog( domainManagerClass::getInstance()->sync() );                
            }catch( const std::runtime_error& error ){
                Serial.println( error.what() );            
                showDialog( error.what() );  
            }            
        }

        if( focused == objects.do_settings_2  && key == "#"  ){
            Serial.println( "Open Settings!" );
            navigateTo( SCREEN_ID_SETTINGS );
        }        

    }

    //---------->

    void handleTouchEvent( lv_event_t* e ) override{
        lv_obj_t* target = lv_event_get_target(e);

        if( target == objects.do_sync_2 ){
            try{
                showDialog( domainManagerClass::getInstance()->sync() );                
            }catch( const std::runtime_error& error ){
                Serial.println( error.what() );            
                showDialog( error.what() );  
            }            
        }

        if( target == objects.do_settings_2 ){
            Serial.println( "Open Settings!" );
            navigateTo( SCREEN_ID_SETTINGS );
        }

        if( 
            ( target == objects.login  )
        ){
            try{
                if(
                    domainManagerClass::getInstance()->login(    
                        String( lv_textarea_get_text( objects.login_username ) ),
                        String( lv_textarea_get_text( objects.login_password ) ) )
                        
                ){
                    navigateTo( SCREEN_ID_MAIN );
                }else{
                    showDialog( "Invalid credentials" );  
                }
            }catch( std::runtime_error& error ){
                showDialog( error.what() );  
            }
        }



if (target == objects.test2 ){

    cameraClass* camera = cameraClass::getInstance();

    camera->shootMegaDecodeAndRender(jpg_holder, megaCam);


}





// if (target == objects.sync_pic_button ){

//     try{

//         cameraClass* camera = cameraClass::getInstance();

//         camera->syncUserPics( 
//             domainManagerClass::getInstance()->comms,  
//             domainManagerClass::getInstance()->serverURL,
//             "/api/device/upload_photo"
//          );


//     }catch( std::runtime_error& error ){

//     }
// }




// static String jpegPK  = "[test]";

// if (target == objects.test_load1 ){

//     try{

//         cameraClass camera = cameraClass::getInstance();

//         camera.loadJPGSDRAMFromWarehouse( jpegPK );        

//         camera.renderJpegFromSDRAM( jpg_holder );


//     }catch( std::runtime_error& error ){

//     }


// }



// /// JPG!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// if (target == objects.test_jpg ) {

//     try{

//         cameraClass camera = cameraClass::getInstance();

//         camera.shootToPixSDRAM();        

//         camera.encodePixelsToJPG();

//         camera.renderJpegFromSDRAM( jpg_holder );

//         jpegPK  = camera.saveJPGSDRAMToWarehouse();

//     }catch( std::runtime_error& error ){

//     }


// }

//     Serial.println("[LOG] >>> INSPECTION START");

//     // 1. Hardware Objects (MUST be static to survive after function return)
//     static OV7670 ov767x;
//     static Camera cam(ov767x);
//     static FrameBuffer fb; 

//     // 2. Allocation & Alignment
//     static uint8_t *fb_mem = (uint8_t *)SDRAM.malloc(640 * 480 * 2 + 32);
//     if (!fb_mem) throw std::runtime_error("CRITICAL: SDRAM Malloc failed!");
    
//     static bool fb_init = false;
//     if (!fb_init) {
//         fb.setBuffer((uint8_t *)ALIGN_PTR((uintptr_t)fb_mem, 32));
//         fb_init = true;
//         Serial.println("[LOG] FrameBuffer aligned and initialized.");
//     }

// //-

//     uint16_t *pixels = (uint16_t *)fb.getBuffer();
//     if (!pixels) throw std::runtime_error("CRITICAL: Buffer pointer is NULL!");

//     // 3. THE SMOKING GUN: Clear to BLUE (using your define)
//     // If the camera fails, you see BLUE. If it works, you see an image.
//     for (int i = 0; i < 640 * 480; i++) {
//         pixels[i] = GC9A01A_BLUE; 
//     }
//     SCB_CleanDCache_by_Addr((uint32_t *)pixels, 640 * 480 * 2);
//     Serial.println("[LOG] RAM white-washed with BLUE.");

// // --    
//     // 4. Hardware Kickstart
//     Serial.println("[LOG] Starting Camera hardware...");
//     if (!cam.begin(CAMERA_R640x480, IMAGE_MODE, 10)) {
//         throw std::runtime_error("HARDWARE ERROR: cam.begin failed!");
//     }

//     //--

//         // 3. THE "ADJUSTMENT" PUMP
//     // The OV7670 AEC/AGC needs active clock cycles to see the dark room.
//     // We grab 10 frames in a fast loop to "pump" the auto-exposure logic.
//     Serial.println("[LOG] Pumping frames to adjust AEC/AGC...");
//     for(int i = 0; i < 10; i++) {
//         // If this returns -1, the DCMI is timed out.
//         if (cam.grabFrame(fb, 1000) != 0) {
//             Serial.print("[WARN] Adjustment skip at frame "); Serial.println(i);
//         }
//     }

//     // 5. The Shot
//     Serial.println("[LOG] Grabbing Frame...");
//     int status = cam.grabFrame(fb, 3000);
//     if (status != 0) {
//         Serial.print("[ERROR] grabFrame Status: "); Serial.println(status);
//         throw std::runtime_error("CAMERA ERROR: Failed to capture frame data.");
//     }
//     Serial.println("[LOG] Grab Success.");

//     // 6. Fix Endianness (HTONS)
//     for (int i = 0; i < 640 * 480; i++) {
//         pixels[i] = HTONS(pixels[i]);
//     }

//     // 7. Flush Cache for M7 DMA/LTDC
//     SCB_CleanDCache_by_Addr((uint32_t *)pixels, 640 * 480 * 2);
//     Serial.println("[LOG] Cache sync complete.");

// //--

//     // 8. LVGL Descriptor (RGB565 per lv_conf.h)
//     static lv_img_dsc_t cam_img_desc;
//     cam_img_desc.header.always_zero = 0;
//     cam_img_desc.header.reserved    = 0;
//     cam_img_desc.header.cf          = LV_IMG_CF_TRUE_COLOR; // 16-bit depth
//     cam_img_desc.header.w           = 640;
//     cam_img_desc.header.h           = 480;
//     cam_img_desc.data_size          = 640 * 480 * 2;
//     cam_img_desc.data               = (const uint8_t *)pixels;

//     // 9. UI Widget Update
//     static lv_obj_t * cam_img_obj = NULL;
//     if (cam_img_obj == NULL) {
//         cam_img_obj = lv_img_create(lv_scr_act());
//         if (!cam_img_obj) throw std::runtime_error("LVGL ERROR: Image object creation failed!");
//     }

//     lv_img_set_src(cam_img_obj, &cam_img_desc);
//     lv_obj_center(cam_img_obj);
//     lv_obj_invalidate(cam_img_obj);

//     Serial.println("[LOG] <<< INSPECTION COMPLETE. EXITING.");
//}



};  //==

// //--

//  //7f88b799-a223-471a-a1ff-964a46e43166
//   if (target == objects.pic_test) {
//             Serial.println("PIC TEST==============================");

//             try {
//                 size_t imgLen = 0;
//                 // Use your UUID that points to the test JPEG on the server
//                 //WiFiSSLClient serverConnection = domainManagerClass::getInstance()->comms->connectToServer(domainManagerClass::getInstance()->serverURL);                                                                                
//                 //uint8_t* img = domainManagerClass::getInstance()->GETImageToSDRAM(serverConnection,"a80e0519-c1b3-4a02-84ec-7f52bdfc4b57", imgLen);

//                 //--------------
            
// loadQSPIFileToSDRAM(
//     "/qspi/brickimg_a80e0519-c1b3-4a02-84ec-7f52bdfc4b57.jpg", imgLen );
//     uint8_t* img = jpg_io_buf;

//                 //-------------

//                 Serial.print("[IMG] Total bytes: ");
//                 Serial.println(imgLen);

//                 if (img == NULL) {
//                     Serial.println("[FATAL] GETImageToSDRAM returned null");
//                     return;
//                 }

//                 // Validate JPEG magic bytes
//                 if (!(img[0] == 0xFF && img[1] == 0xD8)) {
//                     Serial.println("[FATAL] Not a JPEG (bad magic bytes)");
//                     SDRAM.free(img);
//                     return;
//                 }

//                 if (jpg_fb == NULL) {
//                     Serial.println("[FATAL] jpg_fb not allocated");
//                     SDRAM.free(img);
//                     return;
//                 }

//                 // Optional: log JPEG size
//                 uint16_t jw = 0;
//                 uint16_t jh = 0;
//                 if (TJpgDec.getJpgSize(&jw, &jh, img, (uint32_t)imgLen) == 1) {
//                     Serial.print("JPG size: ");
//                     Serial.print(jw);
//                     Serial.print(" x ");
//                     Serial.println(jh);
//                 } else {
//                     Serial.println("Could not get JPG size");
//                 }

//                 // Clear framebuffer - before call back
//                 size_t jpg_bytes = (size_t)JPG_W * (size_t)JPG_H * 2;
//                 memset(jpg_fb, 0, jpg_bytes);

//                 Serial.println("Drawing JPEG via TJpg_Decoder into framebuffer...");
//                 TJpgDec.drawJpg(0, 0, img, imgLen); // call back to render here
//                 Serial.println("JPEG decode complete.");

//                 SDRAM.free(img);

//                 // Show JPEG as LVGL image on current screen
//                 if (jpg_obj == NULL) {
//                     jpg_obj = lv_img_create(lv_scr_act());
//                 }
//                 lv_img_set_src(jpg_obj, &jpg_dsc);
//                 lv_obj_center(jpg_obj);

//             } catch (const std::exception& e) {
//                 Serial.println("==============================");
//                 Serial.println("==============  FATAL  ================");
//                 Serial.println(e.what());
//             }

        //     Serial.println("PIC TEST DONE ==============================");
        // }



//===================================================    


// WiFiSSLClient& imgClient = domainManagerClass::getInstance()->comms->connectToServer( domainManagerClass::getInstance()->serverURL );
    



//===================================================

    void init() override {

        {

            lv_group_add_obj(inputGroup, objects.login_username  );
            lv_group_add_obj(inputGroup, objects.login_password);
            lv_group_add_obj(inputGroup, objects.login);

            lv_group_add_obj(inputGroup, objects.do_sync_2);
            lv_group_add_obj(inputGroup, objects.do_settings_2);


        }
    
        screenClass::makeKeyboards();
        screenClass::addNumericKeyboard( objects.login_username );
        screenClass::addNumericKeyboard( objects.login_password );

        screenClass::init();

        Serial.println( "Login inited *********" );
    }


    void start() override {

        Serial.println( ">>>Setting Start *********" );        
              
        lv_textarea_set_text( objects.login_username , "" );
        lv_textarea_set_text( objects.login_password , "" );

        Serial.println( "<<<Setting started *********" );
    }    

    virtual ~loginScreenClass(){
    };
};



