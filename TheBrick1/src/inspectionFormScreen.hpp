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

#include "core/lv_event.h"

class formFieldsScreenClass : public screenClass {
public:

    std::vector<lv_obj_t*> textareas;     

    formFieldsScreenClass( settingsClass* settings ): screenClass( settings, SCREEN_ID_INSPECTION_FORM ){    
    }

    void clockTic( String time ) override {
        lv_label_set_text( objects.clock_form, time.c_str());
        lv_label_set_text(  objects.driver_name_form, domainManagerClass::getInstance()->loggedUser.name.c_str()  );        
    }    


   void handleKeyboardEvent( String key ) override {        
        screenClass::handleKeyboardEvent( key );
        lv_obj_t* focused = lv_group_get_focused(inputGroup);

        // if numeric send to the current field in the form 
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
                    lv_textarea_del_char( focused ); // oohh ok, some lgvl bug. second is the good one
                    lv_textarea_del_char( focused );                                         
                }
            }
        }

        // NAVI ================================
        if( focused == objects.back_from_form_fields && key == "#" ){
            navigateTo( SCREEN_ID_SELECT_INSPECTION_TYPE );
        }

        if( focused == objects.do_inspect_button && key == "#"  ){
                                        
        }             

    }

    void handleTouchEvent( lv_event_t* e ) override{
        lv_obj_t* target = lv_event_get_target(e);


        // NAVI  ++++++++++++++++++++++++++
        if( target == objects.back_from_form_fields ){
            navigateTo( SCREEN_ID_SELECT_INSPECTION_TYPE );
        }

        if( target == objects.do_inspect_button   ){
                                                                                           
        }            

    }

    void init() override {

        screenClass::makeKeyboard();


        {
            // default
            //lv_group_add_obj(inputGroup, objects.form_fields  );

            // nav bar
            lv_group_add_obj(inputGroup, objects.do_zones );            
            lv_group_add_obj(inputGroup, objects.back_from_form_fields);       
        }

        screenClass::init();
    }

    void start() override{

        domainManagerClass* domain = domainManagerClass::getInstance();
        inspectionTypeClass* currentType = domain->currentInspection.type;
        inspectionClass* currentInspection = &domain->currentInspection;

        if (currentType == NULL) {
            throw std::runtime_error("Error: No inspection type selected!");
        }

        // Show the current inspection type name at top
        lv_label_set_text(objects.inspection_type_name, currentType->name.c_str());            

        // Clear old textarea handles
        textareas.clear();

        // get list
        lv_obj_t* parent_obj = objects.form_fields;
        lv_obj_clean(parent_obj);

        // Make the Form Fields ====================================================
        for (size_t i = 0; i < currentType->formFields.size(); ++i) {
            const std::vector<String>& row = currentType->formFields[i];

            String fieldName = row[0];
            String fieldType = row[1];
            String fieldMax = row.size() >= 3 ? row[2] : "30";

            int maxLength = fieldMax.toInt();
            if (maxLength <= 0) maxLength = 30;

            // === Flex container ===
            lv_obj_t* rowContainer = lv_obj_create(parent_obj);
            lv_obj_set_size(rowContainer, 700, 72);
            lv_obj_set_style_pad_right(rowContainer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(rowContainer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_bg_opa(rowContainer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_border_width(rowContainer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_radius(rowContainer, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(rowContainer, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(rowContainer, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(rowContainer, 5, LV_PART_MAIN | LV_STATE_DEFAULT);

            // === Label ===
            lv_obj_t* label = lv_label_create(rowContainer);
            lv_obj_set_size(label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
            lv_obj_set_style_text_font(label, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(label, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(label, fieldName.c_str());

                    // === Textarea ===
                    lv_obj_t* textarea = lv_textarea_create(rowContainer);
                    lv_obj_set_size(textarea, 388, 54);
                    lv_textarea_set_max_length(textarea, maxLength);

                    // If there’s a previous value, restore it
                    String prefill = "";
                    if (i < currentInspection->inspectionFormFieldValues.size()) {
                        prefill = currentInspection->inspectionFormFieldValues[i];
                    }
                    lv_textarea_set_text(textarea, prefill.c_str());

                    lv_textarea_set_one_line(textarea, true);
                    lv_textarea_set_password_mode(textarea, false);
                    lv_obj_clear_flag(textarea,
                        LV_OBJ_FLAG_SCROLLABLE |
                        LV_OBJ_FLAG_SCROLL_CHAIN_HOR |
                        LV_OBJ_FLAG_SCROLL_CHAIN_VER |
                        LV_OBJ_FLAG_SCROLL_ELASTIC |
                        LV_OBJ_FLAG_SCROLL_MOMENTUM |
                        LV_OBJ_FLAG_SCROLL_ON_FOCUS |
                        LV_OBJ_FLAG_SCROLL_WITH_ARROW);
                    lv_obj_set_style_text_font(textarea, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_color(textarea, lv_color_hex(0xff7095c8), LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_border_color(textarea, lv_color_hex(0xffff0000), LV_PART_MAIN | LV_STATE_FOCUSED);                        

                    // Save handle for sync
                    textareas.push_back(textarea);
                    lv_group_add_obj(inputGroup, textarea );

                    // Hook: show keyboard on focus
                    // lv_obj_add_event_cb(textarea, 
                    
                    //     [](lv_event_t* e) { 

                    //         // get the form field
                    //         lv_obj_t* ta = lv_event_get_target(e);
                    //         formFieldsScreenClass* self = static_cast<formFieldsScreenClass*>(lv_event_get_user_data(e));

                    //         // unhide kb
                    //         lv_obj_clear_flag(self->kb, LV_OBJ_FLAG_HIDDEN);
                    //         lv_keyboard_set_textarea(self->kb, ta);                                

                    //         Serial.println("Keyboard opened for textarea.");
                    //     }

                    // , LV_EVENT_PRESSED, this);

                    addKeyboardHacked( textarea );
        }

        // keyboard spacer ============
        lv_obj_t* spacer = lv_obj_create( objects.form_fields );  
        lv_obj_set_size(spacer, LV_PCT(100), LV_PCT(50));  
        lv_obj_clear_flag(spacer, LV_OBJ_FLAG_SCROLLABLE); 
        lv_obj_set_style_bg_opa(spacer, LV_OPA_TRANSP, 0);                 


    }

    void  stop() override{
        domainManagerClass* domain = domainManagerClass::getInstance();
        inspectionClass* currentInspection = &domain->currentInspection; // FIXED
        inspectionTypeClass* currentType = currentInspection->type;

        if (currentType == NULL) {
            Serial.println("syncToInspection: No inspection type selected!");
            return;
        }

        currentInspection->inspectionFormFieldValues.clear(); // FIXED

        for (lv_obj_t* ta : textareas) {
            const char* input = lv_textarea_get_text(ta);
            currentInspection->inspectionFormFieldValues.push_back(String(input)); // FIXED
        }

        Serial.println("syncToInspection: Form field values saved:");
        for (size_t i = 0; i < currentInspection->inspectionFormFieldValues.size(); ++i) {
            Serial.print("  [");
            Serial.print(i);
            Serial.print("] ");
            Serial.println(currentInspection->inspectionFormFieldValues[i]);
        }

    }

    virtual ~formFieldsScreenClass() {
        if (kb != NULL) {
            lv_obj_del(kb);
            kb = NULL;
            Serial.println("Keyboard destroyed by formFieldsScreenClass destructor.");
        }
    }

    void addKeyboardHacked(lv_obj_t* textarea) {
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
                                
                // work around for lvgl no able to do it on its own
                lv_obj_t* row = lv_obj_get_parent(ta);
                if (row != nullptr) {
                    // Get Y offset of row inside form_fields
                    lv_coord_t y = lv_obj_get_y(row);
                    // Scroll to exact Y offset — brute force
                    lv_obj_scroll_to_y(objects.form_fields, y, LV_ANIM_ON );
                }                

            },
            LV_EVENT_PRESSED,
            this
        );
    }

};

