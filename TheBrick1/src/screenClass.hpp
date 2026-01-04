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

//***************************************************** */

// S C R E E N    C L A S S

//***************************************************** */

LV_FONT_DECLARE(lv_font_montserrat_28);
void fireModalDialogEvent( String action, String button );\
static lv_style_t style_font;
static bool style_ready = false;
class screenClass{
public:

    settingsClass *settings;
    ScreensEnum screenId;    
    lv_group_t* inputGroup = nullptr;

    lv_obj_t* letterKeyboard = nullptr;    
    lv_obj_t* numericKeyboard = nullptr;    


    screenClass( settingsClass* settingsParam, ScreensEnum screenIdParam ): 
    settings{settingsParam}, screenId{screenIdParam}{
        inputGroup = lv_group_create();
    }

    virtual void init() {};

    virtual void start(){};

    virtual void stop(){};    

    virtual bool modalActive(){
        
        if( !overlay ) return false;

        if ( lv_obj_has_flag(overlay, LV_OBJ_FLAG_HIDDEN) ){
            return false;
        }else{
            return true;
        }
    };

    virtual void batteryInfo( String info ){
        
    }

    virtual void rfidEvent( byte *uid, byte length ){
    }

    virtual void clockTic( String time ){
    }

    virtual void handleKeyboardEvent( String key ) {
        handleSystemKeys( key );
    }

    virtual void handleTouchEvent( lv_event_t* e ) {        
    }    

    // -------------------------------------------------------------------

    void showJpegFromQSPI(const String& path) {

        Serial.print("[PIC] showJpegFromQSPI: ");
        Serial.println(path);
        try {

            Serial.print("[PIC] load ... ");                
            size_t imgLen = 0;
            loadQSPIFileToSDRAM( path, jpg_io_buf, JPG_IO_BUF_SIZE , imgLen );
            // jpg_io_buf
            Serial.print("Loaded .... Total bytes: ");
            Serial.println(imgLen);


            // Validate JPEG magic bytes
            if (!(jpg_io_buf[0] == 0xFF && jpg_io_buf[1] == 0xD8)) {
                Serial.println("[FATAL] Not a JPEG (bad magic bytes)");
                return;
            }

                // ==================================================
                // Center the JPEG *inside* our 800x480 framebuffer
                // ==================================================
                uint16_t jw = 0;
                uint16_t jh = 0;
                int16_t  x  = 0;
                int16_t  y  = 0;

                if (TJpgDec.getJpgSize(&jw, &jh, jpg_io_buf, imgLen) ){
                    Serial.print("[PIC] JPG size: ");
                    Serial.print(jw);
                    Serial.print(" x ");
                    Serial.println(jh);

                    if (jw < JPG_W) {
                        x = (int16_t)((JPG_W - jw) / 2);
                    }
                    if (jh < JPG_H) {
                        y = (int16_t)((JPG_H - jh) / 2);
                    }

                    Serial.print("[PIC] Center offsets x=");
                    Serial.print(x);
                    Serial.print(" y=");
                    Serial.println(y);
                } else {
                    Serial.println("[PIC] Could not get JPG size (drawing at 0,0).");
                }


            // Clear framebuffer - before call back
            size_t jpg_bytes = (size_t)JPG_W * (size_t)JPG_H * 2;
            memset(jpg_fb, 0, jpg_bytes);

            Serial.println("Drawing JPEG via TJpg_Decoder into framebuffer...");
            TJpgDec.drawJpg(x, y, jpg_io_buf, imgLen); 
            //TJpgDec.drawJpg(0, 0, jpg_io_buf, imgLen); // call back to render here
            Serial.println("JPEG decode complete.");

            // Show JPEG as LVGL image on current screen
            // if (jpg_obj == NULL) {
            //     jpg_obj = lv_img_create(lv_scr_act());
            // }
            // lv_img_set_src(jpg_obj, &jpg_dsc);
            // lv_obj_center(jpg_obj);

            if (jpg_holder == NULL) {
                jpg_holder = lv_img_create(lv_scr_act());

                // Make it clickable so it eats the click
                lv_obj_add_flag(jpg_holder, LV_OBJ_FLAG_CLICKABLE);

                // Click on the image = hide (no delete)
                lv_obj_add_event_cb(
                    jpg_holder,
                    [](lv_event_t* e) {
                        lv_obj_t* obj = lv_event_get_target(e);
                        // Just hide it; no lv_obj_del, no free
                        lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
                    },
                    LV_EVENT_CLICKED,
                    NULL
                );

            }

            lv_img_set_src(jpg_holder, &jpg_dsc);
            lv_obj_center(jpg_holder);
            lv_obj_clear_flag(jpg_holder, LV_OBJ_FLAG_HIDDEN);


        } catch (const std::exception& e) {
            Serial.println("==============================");
            Serial.println("==============  FATAL  ================");
            Serial.println(e.what());
        }

        Serial.println("Show pic DONE ==============================");            
    }

    // --------------------------------------------------------------------

    // base screen class key nav
    void handleSystemKeys(String key) {

        hideKeyboards();
        
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


// lv_keyboard_mode_t mode = LV_KEYBOARD_MODE_TEXT_LOWER)    


    void makeKeyboards() {

        if (letterKeyboard == nullptr) {

            letterKeyboard = lv_keyboard_create(lv_scr_act());
            lv_obj_set_size(letterKeyboard, 800, 200);
            lv_obj_align(letterKeyboard, LV_ALIGN_BOTTOM_MID, 0, 0);
            lv_obj_add_flag(letterKeyboard, LV_OBJ_FLAG_HIDDEN);
            lv_keyboard_set_mode(letterKeyboard, LV_KEYBOARD_MODE_TEXT_LOWER);

            lv_obj_set_style_text_font(letterKeyboard, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);

            lv_obj_add_event_cb(
                letterKeyboard,
                [](lv_event_t* e) {
                    lv_event_code_t code = lv_event_get_code(e);
                    screenClass* self = static_cast<screenClass*>(lv_event_get_user_data(e));
                    if (self && (code == LV_EVENT_CANCEL || code == LV_EVENT_READY)) {
                        // hide
                        lv_obj_add_flag(self->letterKeyboard, LV_OBJ_FLAG_HIDDEN);
                    
                    }
                },
                LV_EVENT_ALL,
                this
            );
            Serial.println("Keyboard created");
        }

        if (numericKeyboard == nullptr) {

            numericKeyboard = lv_keyboard_create(lv_scr_act());
            lv_obj_set_size(numericKeyboard, 800, 200);
            lv_obj_align(numericKeyboard, LV_ALIGN_BOTTOM_MID, 0, 0);
            lv_obj_add_flag(numericKeyboard, LV_OBJ_FLAG_HIDDEN);
            lv_keyboard_set_mode(numericKeyboard, LV_KEYBOARD_MODE_NUMBER);

            lv_obj_set_style_text_font(numericKeyboard, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);    

            lv_obj_add_event_cb(
                numericKeyboard,
                [](lv_event_t* e) {
                    lv_event_code_t code = lv_event_get_code(e);
                    screenClass* self = static_cast<screenClass*>(lv_event_get_user_data(e));
                    if (self && (code == LV_EVENT_CANCEL || code == LV_EVENT_READY)) {
                        // hide keyboard
                        lv_obj_add_flag(self->numericKeyboard, LV_OBJ_FLAG_HIDDEN);
                    }
                },
                LV_EVENT_ALL,
                this
            );
            Serial.println("Keyboard created");
        }


    }

    void hideKeyboards(){
        if( ( letterKeyboard != nullptr ) && ( numericKeyboard != nullptr ) )
        {
            lv_obj_add_flag( letterKeyboard, LV_OBJ_FLAG_HIDDEN );
            lv_obj_add_flag( numericKeyboard, LV_OBJ_FLAG_HIDDEN );
        }
    }


    void addLetterKeyboard(lv_obj_t* textarea) {

        if (!letterKeyboard) {
            throw std::runtime_error("Keyboard not created before addKeyboard()!");
        }
        Serial.println("Keyboard attached");

        lv_obj_add_event_cb(
            textarea,
            [](lv_event_t* e) {
                lv_obj_t* ta = lv_event_get_target(e);
                screenClass* self = static_cast<screenClass*>(lv_event_get_user_data(e));

                // Show keyboard
               
                lv_obj_clear_flag(self->letterKeyboard, LV_OBJ_FLAG_HIDDEN);
                

                lv_keyboard_set_textarea(self->letterKeyboard, ta);                       

            },
            LV_EVENT_PRESSED,
            this
        );
    }

    void addNumericKeyboard(lv_obj_t* textarea) {

        if (!numericKeyboard) {
            throw std::runtime_error("Keyboard not created before addKeyboard()!");
        }
        Serial.println("Keyboard attached");

        lv_obj_add_event_cb(
            textarea,
            [](lv_event_t* e) {
                lv_obj_t* ta = lv_event_get_target(e);
                screenClass* self = static_cast<screenClass*>(lv_event_get_user_data(e));

                // Show keyboard
                
                lv_obj_clear_flag(self->numericKeyboard, LV_OBJ_FLAG_HIDDEN);

                lv_keyboard_set_textarea(self->numericKeyboard, ta);       
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

        Serial.print( "<<LIST>> Scroll" );                    

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

        Serial.print("<<Dropdown>> scroll...");

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


    virtual ~screenClass(){

        if (inputGroup) {
            lv_group_del(inputGroup);
            Serial.println("mainScreenClass: Screen Class destroyed, inputGroup destroyed");
        }        

        if (letterKeyboard != NULL) {
            lv_obj_del(letterKeyboard);
            letterKeyboard = NULL;
            Serial.println("Letter Keyboard destroyed");
        }   
        
        if (numericKeyboard != NULL) {
            lv_obj_del(numericKeyboard);
            numericKeyboard = NULL;
            Serial.println("Numeric Keyboard destroyed");
        }   
    }


    //-------------------------------------------
    // MODAL DIALOG


        
    lv_obj_t* overlay = NULL;
    lv_obj_t* mbox = NULL;
    String modalAction = "";

    virtual void showDialog(String message) {
        static const char* btns[] = { "OK", "" };
        showDialog(message, "default", btns);
    }

    virtual void showDialog(String message, String action, const char* btns[]) {

        modalAction = action;

        if (!overlay) {
            overlay = lv_obj_create(lv_scr_act());
        }else{
            lv_obj_clear_flag(overlay, LV_OBJ_FLAG_HIDDEN);
        }

        lv_obj_set_size(overlay, LV_PCT(100), LV_PCT(100));

        // Style: pure fullscreen dim, no border, no rounding, no padding
        lv_obj_set_style_bg_color(overlay, lv_color_black(), 0);
        lv_obj_set_style_bg_opa(overlay, LV_OPA_50, 0);
        lv_obj_set_style_border_width(overlay, 0, 0);
        lv_obj_set_style_outline_width(overlay, 0, 0);
        lv_obj_set_style_shadow_width(overlay, 0, 0);
        lv_obj_set_style_pad_all(overlay, 0, 0);
        lv_obj_set_style_radius(overlay, 0, 0);
        lv_obj_clear_flag(overlay, LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_add_flag(overlay, LV_OBJ_FLAG_CLICKABLE);

        mbox = lv_msgbox_create(overlay, "", message.c_str(), btns, false);
        lv_obj_center(mbox);

        lv_obj_set_size(mbox, 500, 250);  // *******************

        // One-time style init
        if (!style_ready) {
            lv_style_init(&style_font);
            lv_style_set_text_font(&style_font, &lv_font_montserrat_28);
            style_ready = true;
        }

        // Apply style to text and buttons
        lv_obj_add_style(lv_msgbox_get_text(mbox), &style_font, 0);
        lv_obj_t* btnm = lv_msgbox_get_btns(mbox);
        lv_obj_add_style(btnm, &style_font, 0);

        lv_obj_set_size(btnm, 500 , 100 );

        // --------- NEW: Make buttons larger ---------
        lv_obj_set_style_min_width(btnm, 180, 0);
        lv_obj_set_style_min_height(btnm, 80, 0);
        lv_obj_set_style_pad_hor(btnm, 16, 0);
        lv_obj_set_style_pad_ver(btnm, 12, 0);    

        // Set callback, pass action string as heap-allocated String
        lv_obj_add_event_cb(btnm, dialog_btn_callback, LV_EVENT_ALL, new String(action));

        lv_obj_clear_flag(overlay, LV_OBJ_FLAG_HIDDEN);
        lv_obj_clear_flag(mbox, LV_OBJ_FLAG_HIDDEN);
    }

    static void dialog_btn_callback(lv_event_t* e) {

        if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
                    
            // Hide overlay
            lv_obj_t* btnm = lv_event_get_target(e);
            lv_obj_t* mbox = lv_obj_get_parent(btnm);
            lv_obj_t* overlay = lv_obj_get_parent(mbox);
            lv_obj_add_flag(overlay, LV_OBJ_FLAG_HIDDEN);

            String* actionPtr = (String*)lv_event_get_user_data(e);
            if( actionPtr ){
                String action = *actionPtr;
                delete actionPtr; // Clean up   !!!!!!!!!!!!!!!!! 
                actionPtr = NULL;

                const char* btn_txt = lv_msgbox_get_active_btn_text(lv_obj_get_parent(lv_event_get_target(e)));
                fireModalDialogEvent(action, btn_txt ? String(btn_txt) : String(""));
            }
        }
    }

    // and we will get the result here , much later ....
    virtual void modalDialogEvent(const String action, const String button) {

        Serial.println( "Modal event " + action + ":" + button );
    }

    virtual void modalDialogKey( String key ){

        //hide the current window
        lv_obj_add_flag(overlay, LV_OBJ_FLAG_HIDDEN);
    }
};
