//***************************************************** */

// S C R E E N    C L A S S

//***************************************************** */


class screenClass{
public:

    ScreensEnum screenId;    
    lv_group_t* inputGroup = nullptr;

    screenClass( ScreensEnum screenIdParam ): 
        screenId{screenIdParam}{
        inputGroup = lv_group_create();
    }

    virtual void open(){
        loadScreen( screenId );        
    };

    virtual bool modalActive(){
        return false;
    };

    virtual void rfidEvent( byte *uid, byte length ){
    }

    virtual void clockTic( String time ){
    }

    //---

    lv_obj_t* getFocusedButton() {
        if (!inputGroup) return nullptr;

        lv_obj_t* focused = lv_group_get_focused(inputGroup);
        if (!focused) return nullptr;

        const lv_obj_class_t* obj_class = lv_obj_get_class(focused);
        if (!obj_class) return nullptr;

        return (obj_class == &lv_btn_class) ? focused : nullptr;
    }

    // lvgl should have done this ...
    virtual void checkTextAreaInView(  ){

            // roll into view
            if (inputGroup != nullptr) {
                lv_obj_t* ta = lv_group_get_focused(inputGroup);

                if (ta != nullptr && lv_obj_check_type(ta, &lv_textarea_class)) {

                    //Serial.println("scroll TA");

                    // Ensure layout is valid before scrolling
                    lv_obj_update_layout(objects.form_fields); //TODO: MAKE GENERIC!!!

                    // Get parent row of textarea
                    lv_obj_t* row = lv_obj_get_parent(ta);
                    if (row != nullptr) {
                        // Get Y offset of row inside form_fields
                        lv_coord_t y = lv_obj_get_y(row);
                        // Scroll to exact Y offset — brute force
                        lv_obj_scroll_to_y(objects.form_fields, y, LV_ANIM_OFF);
                    }
                }
            }
    }


    // For list navigation ....

    lv_obj_t* get_prev_sibling(lv_obj_t* obj) {
        if (!obj) return nullptr;

        lv_obj_t* parent = lv_obj_get_parent(obj);
        if (!parent) return nullptr;

        lv_obj_t* prev = nullptr;
        uint32_t count = lv_obj_get_child_cnt(parent);
        for (uint32_t i = 0; i < count; ++i) {
            lv_obj_t* child = lv_obj_get_child(parent, i);
            if (child == obj) {
                return prev;
            }
            prev = child;
        }
        return nullptr;
    }

    lv_obj_t* get_next_sibling(lv_obj_t* obj) {
        if (!obj) return nullptr;

        lv_obj_t* parent = lv_obj_get_parent(obj);
        if (!parent) return nullptr;

        bool found = false;
        uint32_t count = lv_obj_get_child_cnt(parent);
        for (uint32_t i = 0; i < count; ++i) {
            lv_obj_t* child = lv_obj_get_child(parent, i);
            if (found) {
                return child;
            }
            if (child == obj) {
                found = true;
            }
        }
        return nullptr;
    }


    void keyListScrolling( String key ){

        Serial.print( "LIST: Scroll" );                    

        // get target
        lv_obj_t* list = lv_group_get_focused(inputGroup);

        // is it a list ? 
        if (!list || !lv_obj_check_type(list, &lv_list_class) ) return;

        // is it empty ??
        uint32_t count = lv_obj_get_child_cnt(list);
        if( count == 0 ) return;

        // find current selection in the list
        Serial.print( "ActiveSelection?" );                            
        lv_obj_t* selected = nullptr;        
        for (uint32_t i = 0; i < count; ++i) {
            lv_obj_t* btn = lv_obj_get_child(list, i);
            if (lv_obj_has_state(btn, LV_STATE_CHECKED)) {
                selected = btn;
                Serial.print( "Y" );                            
                break;
            }
        }

        // nothing, pick first
        if (!selected) {
            Serial.print( "N" );                                        
            lv_obj_t* selected = lv_obj_get_child(list, 0);
            if (selected) {
                Serial.print( "default" );                            
                lv_obj_add_state(selected, LV_STATE_CHECKED);
                lv_obj_scroll_to_view(selected, LV_ANIM_ON);
                return;
            }else{
                Serial.print( "no default!!!" );                            
                return; // should not be
            }
        }


        // start scrolling...
        Serial.print( "doscroll" );                            
        if (key == "A" || key == "B") {
            lv_obj_t* next = nullptr;

            if (key == "A") {
                next = get_prev_sibling(selected);

            } else if (key == "B") {
                next = get_next_sibling(selected);
            }

            // move
            if (next) {
                lv_obj_clear_state(selected, LV_STATE_CHECKED);
                lv_obj_add_state(next, LV_STATE_CHECKED);
                lv_obj_scroll_to_view(next, LV_ANIM_ON);
            }
        }

        Serial.println( " Scrolled" );                    

    }

    // ===================================================

    virtual void handleEvents(lv_event_t* e, String key) {

      if( key != "" ){
        keyboardEvent( key );
      }    

    }

    // base screen class key nav
    void keyboardEvent(String key) {

        // get the focused thing
        Serial.print( "*** *** Screen Base: Key Handler: " );            
        Serial.println( key );            

        // LIST UP/DWN ---------------
        Serial.print( "L" );            
        keyListScrolling( key );

        // TAB NAVI ---------------
        Serial.print( "Tab" );            
        if (key == "C") {
            lv_group_focus_prev(inputGroup);
            checkTextAreaInView();
            return;

        } else if (key == "D") {
            lv_group_focus_next(inputGroup);
            checkTextAreaInView();
            return;

        } else if (key == "*") {
            lv_group_send_data(inputGroup, LV_KEY_ESC);
            return;
        }
        //--------------------------------

        Serial.println( "3end" );                    
        // else we are done, child class could do something special to the screen
    }


    virtual ~screenClass(){
        if (inputGroup) {
            lv_group_del(inputGroup);
            Serial.println("mainScreenClass: Screen Class destroyed, inputGroup destroyed");
        }        
    }


};
