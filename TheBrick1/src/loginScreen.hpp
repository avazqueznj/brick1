/********************************************************************************************
 * CONFIDENTIAL AND PROPRIETARY
 * 
 * The Brick 1.0 
 * © [2025] [Alejandro Vazquez]. All rights reserved.
 * 
 ********************************************************************************************/


//-------------------------------------------------

// global calls
void navigateTo(int screenId);
void configChanged();

class loginScreenClass:public screenClass{
public:
    

    loginScreenClass( settingsClass* settings ): screenClass( settings, SCREEN_ID_LOGIN_SCREEN ){            
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
            if(
                domainManagerClass::getInstance()->login(    
                    String( lv_textarea_get_text( objects.login_username ) ),
                    String( lv_textarea_get_text( objects.login_password ) ) )
                    
            ){
                navigateTo( SCREEN_ID_MAIN );
            }else{
                showDialog( "Invalid credentials" );  
            }

        }


//--

//  7f88b799-a223-471a-a1ff-964a46e43166
//   if (target == objects.pic_test) {
//             Serial.println("PIC TEST==============================");

//             try {
//                 size_t imgLen = 0;
//                 // Use your UUID that points to the test JPEG on the server
//                 uint8_t* img = downloadImageToSDRAM("a80e0519-c1b3-4a02-84ec-7f52bdfc4b57", imgLen);

//                 Serial.print("[IMG] Total bytes: ");
//                 Serial.println(imgLen);

//                 if (img == NULL) {
//                     Serial.println("[FATAL] downloadImageToSDRAM returned null");
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

//             Serial.println("PIC TEST DONE ==============================");
//         }
// //--


    } //-- handle touche event

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



