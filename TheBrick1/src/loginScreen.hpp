/********************************************************************************************
 * CONFIDENTIAL AND PROPRIETARY
 * 
 * The Brick 1.0 
 * © [2025] [Alejandro Vazquez]. All rights reserved.
 * 
 ********************************************************************************************/


//-------------------------------------------------


class loginScreenClass:public screenClass{
public:
    

    loginScreenClass(): screenClass( SCREEN_ID_LOGIN_SCREEN ){            
    }

    void clockTic( String time ) override {
        lv_label_set_text( objects.clock_login, time.c_str());
    }


    void handleEvents( lv_event_t* e, String key ) override{

        screenClass::handleEvents( e, key );
        
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

    }

    void open() override {

        // Create fresh group or reuse existing
        {
            //-------------------------------------            
            // Add focusable widgets (  lv_group_focus_obj(objects.do_inspect_button);             )


            lv_group_add_obj(inputGroup, objects.login_username  );
            lv_group_add_obj(inputGroup, objects.login_password);
            lv_group_add_obj(inputGroup, objects.login);

            lv_group_add_obj(inputGroup, objects.do_sync_2);
            lv_group_add_obj(inputGroup, objects.do_settings_2);


        }
    
        // Add focusable widgets
        //-------------------------------------

        screenClass::open();
    }

    virtual ~loginScreenClass(){
    };
};



