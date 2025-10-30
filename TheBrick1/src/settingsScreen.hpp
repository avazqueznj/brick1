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

// ---- TIMEZONE OFFSET TABLE, NO STATIC, NO CLASS MEMBERS ----
const int tz_offset_minutes[] = {
    -600,  // (UTC-10:00) Pacific/Honolulu
    -540,  // (UTC-09:00) America/Anchorage
    -480,  // (UTC-08:00) America/Los_Angeles
    -420,  // (UTC-07:00) America/Denver
    -420,  // (UTC-07:00) America/Phoenix
    -360,  // (UTC-06:00) America/Chicago
    -300,  // (UTC-05:00) America/New_York
    -480,  // (UTC-08:00) America/Vancouver
    -420,  // (UTC-07:00) America/Edmonton
    -360,  // (UTC-06:00) America/Winnipeg
    -300,  // (UTC-05:00) America/Toronto
    -240,  // (UTC-04:00) America/Halifax
    -210,  // (UTC-03:30) America/St_Johns
    -480,  // (UTC-08:00) America/Tijuana
    -420,  // (UTC-07:00) America/Chihuahua
    -360,  // (UTC-06:00) America/Mexico_City
    -300,  // (UTC-05:00) America/Cancun
    -240,  // (UTC-04:00) America/Puerto_Rico
    -300,  // (UTC-05:00) America/Jamaica
    -240,  // (UTC-04:00) America/Barbados
    -300,  // (UTC-05:00) America/Nassau
    -300,  // (UTC-05:00) America/Lima
    -300,  // (UTC-05:00) America/Bogota
    -240,  // (UTC-04:00) America/Caracas
    -240,  // (UTC-04:00) America/Santiago
    -180,  // (UTC-03:00) America/Argentina/Buenos_Aires
    -180   // (UTC-03:00) America/Sao_Paulo
};
const int tz_count = sizeof(tz_offset_minutes) / sizeof(tz_offset_minutes[0]);


class settingsScreenClass:public screenClass{
public:

    settingsScreenClass( settingsClass* settings ): screenClass( settings, SCREEN_ID_SETTINGS ){    
    }

    void clockTic( String time ) override {
        lv_label_set_text( objects.clock_settings, time.c_str());
    }

    void handleEvents( lv_event_t* e, String key ) override{       
        screenClass::handleEvents( e, key );        
        lv_obj_t *target = lv_event_get_target(e);
        lv_event_code_t code = lv_event_get_code(e);


        if( target == objects.back_from_settings ){
            Serial.println( "Back to login screen!" );
            navigateTo( SCREEN_ID_LOGIN_SCREEN );
        }

        if ( code == LV_EVENT_VALUE_CHANGED && ( target == objects.settings_tz || target == objects.dst  ) ) {

            int sel = lv_dropdown_get_selected(objects.settings_tz);
            if (sel >= 0 && sel < tz_count) {

                // offset
                int offset = tz_offset_minutes[sel];
                // + DST
                bool dst_active = lv_obj_has_state(objects.dst, LV_STATE_CHECKED);
                if (dst_active) {
                    offset += 60;
                    Serial.println("DST is ON, adding 60 minutes.");
                    domainManagerClass::getInstance()->DST = 1;
                }else{
                    domainManagerClass::getInstance()->DST = 0;
                }
                domainManagerClass::getInstance()->timeOffsetFromUTC = offset;                                

                domainManagerClass::getInstance()->timeZoneIndex = sel;                                     
                

                Serial.print("Timezone changed to index: ");
                Serial.print( domainManagerClass::getInstance()->timeZoneIndex );

                Serial.print(" offset min: ");
                Serial.print(domainManagerClass::getInstance()->timeOffsetFromUTC);

                Serial.print(" dst: ");
                Serial.println( String( domainManagerClass::getInstance()->DST ) );


            }
        }
    }

    void init() override {

  
        {
            lv_group_add_obj(inputGroup, objects.setting_company  );
            lv_group_add_obj(inputGroup, objects.settings_tz  );
            lv_group_add_obj(inputGroup, objects.setting_server_url  );
            lv_group_add_obj(inputGroup, objects.setting_wifi_name  );
            lv_group_add_obj(inputGroup, objects.setting_wifi_password  );                                                

        }
    
        screenClass::makeKeyboard();
        screenClass::addKeyboard( objects.setting_company );
        screenClass::addKeyboard( objects.setting_server_url );
        screenClass::addKeyboard( objects.setting_wifi_name );
        screenClass::addKeyboard( objects.setting_wifi_password );                        

        Serial.println( "Setting inited *********" );
    }

    void start() override {

        Serial.println( ">>>Setting Start *********" );        
         
        lv_textarea_set_text( objects.setting_company , domainManagerClass::getInstance()->company.c_str() );

        lv_dropdown_set_selected( objects.settings_tz , domainManagerClass::getInstance()->timeZoneIndex );        
        if(  domainManagerClass::getInstance()->DST  == 1  ){
            lv_obj_add_state(objects.dst, LV_STATE_CHECKED);
        }else{
            lv_obj_clear_state(objects.dst, LV_STATE_CHECKED);
        }

        lv_textarea_set_text( objects.setting_server_url , domainManagerClass::getInstance()->serverURL.c_str() );
        lv_textarea_set_text( objects.setting_wifi_name , domainManagerClass::getInstance()->comms->ssid.c_str() );
        lv_textarea_set_text( objects.setting_wifi_password , domainManagerClass::getInstance()->comms->pass.c_str() );


        Serial.println( "<<<Setting started *********" );
    }

    void stop() override {
        
        // Write back company
        domainManagerClass::getInstance()->company = lv_textarea_get_text(objects.setting_company);

        // Write back selected timezone index
        domainManagerClass::getInstance()->timeZoneIndex = lv_dropdown_get_selected(objects.settings_tz);

        // Write back DST switch (true if checked, false otherwise)
        domainManagerClass::getInstance()->DST = lv_obj_has_state(objects.dst, LV_STATE_CHECKED);

        // Write back server URL
        domainManagerClass::getInstance()->serverURL = lv_textarea_get_text(objects.setting_server_url);

        // Write back WiFi credentials
        if(domainManagerClass::getInstance()->comms) {
            domainManagerClass::getInstance()->comms->ssid = lv_textarea_get_text(objects.setting_wifi_name);
            domainManagerClass::getInstance()->comms->pass = lv_textarea_get_text(objects.setting_wifi_password);
        }

        // update offset
        int offset = tz_offset_minutes[lv_dropdown_get_selected(objects.settings_tz)];
        bool dst_active = lv_obj_has_state(objects.dst, LV_STATE_CHECKED);
        if (dst_active) {
            offset += 60;
        }
        domainManagerClass::getInstance()->timeOffsetFromUTC = offset;               

        Serial.println("Setting stopped (values saved) *********");


        configChanged();
    }

    virtual ~settingsScreenClass(){};
};



