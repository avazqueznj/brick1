//***************************************************** */

// S C R E E N    C L A S S

//***************************************************** */

LV_FONT_DECLARE(lv_font_montserrat_28);



static lv_style_t style_font;
static bool style_ready = false;
class screenClass{
public:

    settingsClass *settings;

    ScreensEnum screenId;    
    lv_group_t* inputGroup = nullptr;
    lv_obj_t* kb = nullptr;    

    screenClass( settingsClass* settingsParam, ScreensEnum screenIdParam ): 
    settings{settingsParam}, screenId{screenIdParam}
        {
        inputGroup = lv_group_create();
    }

    virtual void init(){};

    virtual void start(){};

    virtual void stop(){};    

    virtual bool modalActive(){
        return false;
    };

    virtual void rfidEvent( byte *uid, byte length ){
    }

    virtual void clockTic( String time ){
    }

    //---
    // LV_KEYBOARD_MODE_NUMBER 

void makeKeyboard(lv_keyboard_mode_t mode = LV_KEYBOARD_MODE_TEXT_LOWER) {
    if (kb == nullptr) {
        kb = lv_keyboard_create(lv_scr_act());
        lv_obj_set_size(kb, 800, 200);
        lv_obj_align(kb, LV_ALIGN_BOTTOM_MID, 0, 0);
        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);

        lv_keyboard_set_mode(kb, mode);

        lv_obj_add_event_cb(
            kb,
            [](lv_event_t* e) {
                lv_event_code_t code = lv_event_get_code(e);
                screenClass* self = static_cast<screenClass*>(lv_event_get_user_data(e));
                if (self && (code == LV_EVENT_CANCEL || code == LV_EVENT_READY)) {
                    lv_obj_add_flag(self->kb, LV_OBJ_FLAG_HIDDEN);
                    // Optionally, restore settings screen position if you moved it
          
                }
            },
            LV_EVENT_ALL,
            this
        );
        Serial.println("Keyboard created");
    }
}



void addKeyboard(lv_obj_t* textarea) {
    if (!kb) {
        throw std::runtime_error("Keyboard not created before addKeyboard()!");
    }
    Serial.println("Keyboard attached");

    lv_obj_add_event_cb(
        textarea,
        [](lv_event_t* e) {
            lv_obj_t* ta = lv_event_get_target(e);
            screenClass* self = static_cast<screenClass*>(lv_event_get_user_data(e));

            // Show keyboard
            lv_obj_clear_flag(self->kb, LV_OBJ_FLAG_HIDDEN);
            lv_keyboard_set_textarea(self->kb, ta);



        },
        LV_EVENT_PRESSED,
        this
    );
}






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

    void keyDropdownScrolling(String key) {

        Serial.print("Dropdown scroll...");

        lv_obj_t* focused = lv_group_get_focused(inputGroup);
        if (!focused || !lv_obj_check_type(focused, &lv_dropdown_class)) return;

        int32_t selected = lv_dropdown_get_selected(focused);


        const char* opts = lv_dropdown_get_options(focused);
        uint16_t option_cnt = 0;
        if (opts && opts[0]) {
            option_cnt = 1;
            for (const char* c = opts; *c; ++c) {
                if (*c == '\n') ++option_cnt;
            }
        }
        if (option_cnt == 0) return;

        int32_t next = selected;
        if (key == "A") {
            next = (selected > 0) ? selected - 1 : option_cnt - 1;
        } else if (key == "B") {
            next = (selected + 1 < option_cnt) ? selected + 1 : 0;
        } else {
            return;
        }

        lv_dropdown_set_selected(focused, next);
        Serial.print("Dropdown scrolled to option: ");
        Serial.println(next);
    }


    // ===================================================

    virtual void handleEvents(lv_event_t* e, String key) {

      if( key != "" ){
        keyboardEvent( key );
      }    

    }

    // base screen class key nav
    void keyboardEvent(String key) {

        if( kb != nullptr )
        {
            lv_obj_add_flag( kb, LV_OBJ_FLAG_HIDDEN );
        }
        

        // get the focused thing
        Serial.print( "*** *** Screen Base: Key Handler: " );            
        Serial.println( key );            

        // LIST UP/DWN ---------------
        Serial.print( "L" );            
        keyListScrolling( key );

        // DROPDOWN SCROLL ---------------
        keyDropdownScrolling(key);        

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

        if (kb != NULL) {
            lv_obj_del(kb);
            kb = NULL;
            Serial.println("Keyboard destroyed");
        }        
    }


            //-------------------------------------------
            // DIALOG

            
            lv_obj_t *overlay = NULL;
            lv_obj_t *mbox = NULL;

            virtual void createDialog( String message )
            {

                // if it does not exist, show it
                if (overlay == NULL ) {
                     
                    static const char * btns[] = { "OK", "" };

                    // Create overlay
                    overlay = lv_obj_create(lv_scr_act());
                    lv_obj_set_size(overlay, LV_PCT(100), LV_PCT(100));
                    lv_obj_set_style_bg_color(overlay, lv_color_black(), 0);
                    lv_obj_set_style_bg_opa(overlay, LV_OPA_50, 0);
                    lv_obj_clear_flag(overlay, LV_OBJ_FLAG_SCROLLABLE);
                    lv_obj_add_flag(overlay, LV_OBJ_FLAG_CLICKABLE); 

                    // Create message box
                    mbox = lv_msgbox_create(overlay, "", message.c_str() , btns, false);
                    lv_obj_center(mbox);

                    // One-time style init
                    if (!style_ready) {
                        lv_style_init(&style_font);
                        lv_style_set_text_font(&style_font, &lv_font_montserrat_28);
                        style_ready = true;
                    }

                    // Apply style to text and buttons
                    lv_obj_add_style(lv_msgbox_get_text(mbox), &style_font, 0);
                    lv_obj_t * btnm = lv_msgbox_get_btns(mbox);
                    lv_obj_add_style(btnm, &style_font, 0);

                    // Set callback on button matrix
                      // Lambda callback: hide overlay on button press
                        lv_obj_add_event_cb(btnm,
                            [](lv_event_t* e) {
                                if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
                                    lv_obj_t* btnm = lv_event_get_target(e);
                                    lv_obj_t* mbox = lv_obj_get_parent(btnm);
                                    lv_obj_t* overlay = lv_obj_get_parent(mbox);
                                    lv_obj_add_flag(overlay, LV_OBJ_FLAG_HIDDEN);
                                }
                            },
                            LV_EVENT_ALL,
                            nullptr
                        );
                } 

                // if it exists, show  it
                lv_label_set_text(lv_msgbox_get_text(mbox), message.c_str() );
                lv_obj_clear_flag(overlay, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(mbox, LV_OBJ_FLAG_HIDDEN);


            }



};
