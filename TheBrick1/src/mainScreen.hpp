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


//-------------------------------------------------

void navigateTo(int screenId);

class mainScreenClass:public screenClass{
public:
    

    mainScreenClass( settingsClass* settings ): screenClass( settings, SCREEN_ID_SETTINGS ){    
    }

    void clockTic( String time ) override {
        lv_label_set_text( objects.clock, time.c_str());
        lv_label_set_text(  objects.driver_name_main, domainManagerClass::getInstance()->loggedUser.name.c_str()  );        
    }

    void handleEvents( lv_event_t* e, String key ) override{
        
        screenClass::handleEvents( e, key );  
        lv_obj_t* target = lv_event_get_target(e);
        lv_obj_t* focused = lv_group_get_focused(inputGroup);


        if( 
            ( target == objects.do_sync ) ||
            ( focused == objects.do_sync && key == "#" )
        ){
            try{
                createDialog( domainManagerClass::getInstance()->sync() );
            }catch( const std::runtime_error& error ){
                Serial.println( error.what() );            
                createDialog( error.what() );  
            }            
        }

        if( 
            ( target == objects.logout ) ||
            ( focused == objects.logout && key == "#" )
        ){

            domainManagerClass::getInstance()->logout();
        }    
        
        if( 
            ( target == objects.do_inspect_button ) ||
            ( focused == objects.do_inspect_button && key == "#" )
        ){

            navigateTo( SCREEN_ID_SELECT_ASSET_SCREEN );
        }            
        
    }

    void init() override {

        // Create fresh group or reuse existing
        {
            //-------------------------------------            
            // Add focusable widgets (  lv_group_focus_obj(objects.do_inspect_button);             )


            lv_group_add_obj(inputGroup, objects.do_inspect_button  );
            lv_group_add_obj(inputGroup, objects.do_sync);
            lv_group_add_obj(inputGroup, objects.logout);            
            
        }
    
        // Add focusable widgets
        //-------------------------------------
    }

    virtual ~mainScreenClass(){
    };
};




