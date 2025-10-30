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


    void handleEvents( lv_event_t* e, String key ) override{

        screenClass::handleEvents( e, key );            
        lv_obj_t* target = lv_event_get_target(e);
        //lv_obj_t* focused = lv_group_get_focused(inputGroup);

        // add numeric input to focused text areas
        if( key != "A" && key != "B" && key != "C" && key != "D" && key != "*" && key != "#"  ){
            lv_obj_t* focused = lv_group_get_focused(inputGroup);
            if (focused && lv_obj_check_type(focused, &lv_textarea_class)) {
                lv_textarea_add_text(focused, key.c_str());                    
            }
        }

        // use * as backspace
        if (key == "*") {
            lv_obj_t* focused = lv_group_get_focused(inputGroup);
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


        if( target == objects.do_sync_2 ){
            try{
                createDialog( domainManagerClass::getInstance()->sync() );
                
            }catch( const std::runtime_error& error ){
                Serial.println( error.what() );            
                createDialog( error.what() );  
            }            
        }

        if( target == objects.do_settings_2 ){

            Serial.println( "Open Settings!" );
            navigateTo( SCREEN_ID_SETTINGS );
        }

        if( 
            ( target == objects.login && key == "" ) ||
            ( lv_group_get_focused(inputGroup) == objects.login && key == "#" ) ||
            ( lv_group_get_focused(inputGroup) == objects.login_password && key == "#" )
        ){

            navigateTo( SCREEN_ID_MAIN );

            // if(
            //     domainManagerClass::getInstance()->login(    
            //         String( lv_textarea_get_text( objects.login_username ) ),
            //         String( lv_textarea_get_text( objects.login_password ) ) )
            //     ){
            //         navigateTo( SCREEN_ID_MAIN );
            //     }else{
            //         createDialog( "Invalid credentials" );  
            //     }

        }

    }

    void init() override {

        {

            lv_group_add_obj(inputGroup, objects.login_username  );
            lv_group_add_obj(inputGroup, objects.login_password);
            lv_group_add_obj(inputGroup, objects.login);

            lv_group_add_obj(inputGroup, objects.do_sync_2);
            lv_group_add_obj(inputGroup, objects.do_settings_2);


        }
    
        screenClass::makeKeyboard( LV_KEYBOARD_MODE_NUMBER );
        screenClass::addKeyboard( objects.login_username );
        screenClass::addKeyboard( objects.login_password );

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



