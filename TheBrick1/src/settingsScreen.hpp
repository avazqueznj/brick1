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



class settingsScreenClass:public screenClass{
public:

    settingsScreenClass(): screenClass( SCREEN_ID_SETTINGS ){    
    }

    void handleEvents( lv_event_t* e, String key ) override{
        
        screenClass::handleEvents( e, key );        
        lv_obj_t *target = lv_event_get_target(e);


        if( target == objects.back_from_settings ){
            Serial.println( "Open settings!" );
            navigateTo( SCREEN_ID_LOGIN_SCREEN );
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
    
        screenClass::init();

        screenClass::makeKeyboard();
        screenClass::addKeyboard( objects.setting_company );
        screenClass::addKeyboard( objects.setting_server_url );
        screenClass::addKeyboard( objects.setting_wifi_name );
        screenClass::addKeyboard( objects.setting_wifi_password );                        

    }

    virtual ~settingsScreenClass(){};
};