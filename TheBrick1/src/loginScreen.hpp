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

if (target == objects.pic_test) {
    Serial.println("PIC TEST==============================");

    try {
        size_t imgLen = 0;
        uint8_t* img = downloadImageToSDRAM("7343e03e-d632-4b36-b3e5-367b6e929e3d", imgLen);

        // Check for JPEG magic bytes
        if (!(img[0] == 0xFF && img[1] == 0xD8)) {
            Serial.println("[FATAL] Data is not a JPEG (bad magic bytes)");
            SDRAM.free(img);
            throw std::runtime_error("Downloaded data is not a JPEG file");
        }

        // LVGL will use its built-in JPEG decoder automatically
        // Pass the raw JPEG data directly as an image source

        // Make sure the image data stays alive (in RAM) while displayed!
        lv_img_set_src(objects.testpic, img); // pointer to JPEG data in RAM

        // If you want to free/replace, clear or set to NULL before freeing!
        // lv_img_set_src(objects.testpic, NULL); // If needed, to release before freeing

        // SDRAM.free(img); // Only free if image is no longer used!

    } catch (const std::exception& e) {
        Serial.println("==============================");
        Serial.println("==============  FATAL  ================");
        Serial.println(e.what());
    }

    Serial.println("PIC TEST DONE ==============================");
}

//--


    } //-- handle touche event

//===================================================    


// WiFiSSLClient& imgClient = domainManagerClass::getInstance()->comms->connectToServer( domainManagerClass::getInstance()->serverURL );
    

uint8_t* downloadImageToSDRAM(
    const String& uuid,
    size_t& outLen
) {
    outLen = 0;
    String path = "/api/device/images/" + uuid;

    Serial.println("==============================");
    Serial.println("[IMG] Starting image download (allocate-once)");
    Serial.print("[IMG] Server: "); Serial.println(domainManagerClass::getInstance()->serverURL);
    Serial.print("[IMG] Path: "); Serial.println(path);
    Serial.println("==============================");

    WiFiSSLClient& serverConnection = domainManagerClass::getInstance()->comms->connectToServer(domainManagerClass::getInstance()->serverURL);
    uint8_t* buffer = 0;
    int httpStatus = 0;
    String statusLine;
    try{
        Serial.println("[IMG] Sending HTTP request...");
        serverConnection.print("GET " + path + " HTTP/1.1\r\n");
        serverConnection.print("Host: " + domainManagerClass::getInstance()->serverURL + "\r\n");
        serverConnection.print("Authorization: Bearer " + BEARER_TOKEN + "\r\n");
        serverConnection.print("Accept: */*\r\n");
        serverConnection.print("Connection: close\r\n");
        serverConnection.print("\r\n");

        int contentLength = -1;
        String line;
        String contentType = "";
        bool isStatusLine = true;

        Serial.println("[IMG] Reading HTTP headers...");

        int headerTries = 0;
        const int HEADER_MAX_TRIES = 100; // 100 * 100ms = 10s

        Serial.println("[IMG] Reading HTTP headers...");
        while (serverConnection.connected()) {
            if (!serverConnection.available()) {
                Serial.println("wait .... "); 
                delay(100);
                headerTries++;
                if (headerTries >= HEADER_MAX_TRIES) {
                    serverConnection.stop(); WiFi.end();
                    Serial.println("[FATAL] HTTP header read timeout!");
                    throw std::runtime_error("HTTP header read timeout");
                }
                continue;
            }
            headerTries = 0; // Reset on progress
            line = serverConnection.readStringUntil('\n');
            line.trim();
            if (line.length() == 0) break;

            Serial.print("[HDR] "); Serial.println(line);

            if (isStatusLine) {
                statusLine = line;
                isStatusLine = false;
                int firstSpace = line.indexOf(' ');
                int secondSpace = line.indexOf(' ', firstSpace + 1);
                if (firstSpace > 0 && secondSpace > firstSpace) {
                    httpStatus = line.substring(firstSpace + 1, secondSpace).toInt();
                    Serial.print("[HDR] HTTP Status = "); Serial.println(httpStatus);
                }
            }
            if (line.startsWith("Content-Length:")) {
                contentLength = line.substring(15).toInt();
                Serial.print("[HDR] Content-Length = ");
                Serial.println(contentLength);
            }
            if (line.startsWith("Content-Type:")) {
                contentType = line.substring(13);
                contentType.trim();
                Serial.print("[HDR] Content-Type = ");
                Serial.println(contentType);
            }
            if (line.startsWith("Transfer-Encoding:") && line.indexOf("chunked") >= 0) {
                serverConnection.stop(); WiFi.end();
                Serial.println("[FATAL] Chunked transfer not allowed!");
                throw std::runtime_error("Image download error: chunked transfer not supported");
            }
        }

        // --- Check HTTP Status ---
        if (httpStatus < 200 || httpStatus >= 300) {
            // Not OK! Read and log the body as error message
            Serial.print("[FATAL] HTTP error code: "); Serial.println(httpStatus);
            String errorMsg;
            while (serverConnection.available()) {
                char c = serverConnection.read();
                errorMsg += c;
            }
            serverConnection.stop(); WiFi.end();
            Serial.print("[FATAL] Server error message: ");
            Serial.println(errorMsg);
            String error = "Image download failed with HTTP status " +  String(httpStatus) + errorMsg;
            throw std::runtime_error(
                error.c_str()
            );
        }

        if (!contentType.startsWith("image/jpeg")) {
            serverConnection.stop(); WiFi.end();
            Serial.print("[FATAL] Bad Content-Type: "); Serial.println(contentType);
            throw std::runtime_error("Image download error: not a JPEG (Content-Type mismatch)");
        }

        if (contentLength <= 0) {
            serverConnection.stop(); WiFi.end();
            Serial.println("[FATAL] No Content-Length!");
            throw std::runtime_error("Image download error: missing Content-Length");
        }

        Serial.print("[IMG] Allocating SDRAM: ");
        Serial.println(contentLength);
        buffer = (uint8_t*)SDRAM.malloc(contentLength);
        if (!buffer) {
            serverConnection.stop(); WiFi.end();
            Serial.println("[FATAL] SDRAM alloc failed!");
            throw std::runtime_error("SDRAM allocation failed");
        }

        size_t totalRead = 0;
        size_t tries = 0;
        const size_t MAX_TRIES = 100; 
        while (totalRead < (size_t)contentLength) {
            int n = serverConnection.read(buffer + totalRead, contentLength - totalRead);
            if (n > 0) {
                totalRead += n;
                Serial.print("[IMG] Bytes read: "); Serial.println(totalRead);
                tries = 0;
            } else if (!serverConnection.connected()) {
                Serial.println("[FATAL] Connection closed before download complete.");
                break;
            } else {
                delay(100);
                Serial.print("[IMG] No data, wait .... ");        
                tries++;
                if (tries >= MAX_TRIES) {
                    Serial.println("[FATAL] Read timeout: no progress for 10 seconds.");
                    throw std::runtime_error("Image download failed: network timeout (no progress for 10 seconds)");
                    break;
                }
            }
        }

        serverConnection.stop(); WiFi.end();

        if (totalRead != (size_t)contentLength) {
            SDRAM.free(buffer);
            Serial.print("[FATAL] Only read "); Serial.print(totalRead); Serial.print(" of "); Serial.println(contentLength);
            throw std::runtime_error("Image download incomplete (Content-Length mismatch)");
        }

        outLen = totalRead;
        Serial.println("[IMG] Download complete");
        Serial.print("[IMG] Total bytes: "); Serial.println(outLen);
        Serial.println("==============================");

        return buffer;

    } catch(...) {
        if (buffer) {
            SDRAM.free(buffer);
            buffer = nullptr;
        }        
        serverConnection.stop(); WiFi.end();
        throw;
    }
}

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



