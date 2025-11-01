#include "core/lv_event.h"
/********************************************************************************************
 * CONFIDENTIAL AND PROPRIETARY
 * 
 * ZZZ2025 
 * © [2025] [Alejandro Vazquez]. All rights reserved.
 * 
 ********************************************************************************************/

class selectInspectionTypeScreenClass:public screenClass{
public:

    selectInspectionTypeScreenClass( settingsClass* settings ): screenClass( settings, SCREEN_ID_SELECT_INSPECTION_TYPE ){    
    }


    void clockTic( String time ) override {
        lv_label_set_text( objects.clock_insptype, time.c_str());
        lv_label_set_text(  objects.driver_name_insptype, domainManagerClass::getInstance()->loggedUser.name.c_str()  );        
    }    

   void handleKeyboardEvent( String key ) override {        
        screenClass::handleKeyboardEvent( key );
        lv_obj_t* focused = lv_group_get_focused(inputGroup);


        // shortcut to get out fast
        if (key == "#") {
            lv_obj_t* focused = lv_group_get_focused(inputGroup);
            if (focused == objects.inspection_types) {
                // navigate to next screen
                return;
            }
        }    
        
        if( focused == objects.back_from_select_insp&& key == "#" ){
            navigateTo( SCREEN_ID_SELECT_ASSET_SCREEN );
        }
        if( focused == objects.do_inspection_form && key == "#"  ){

            // uint32_t child_count = lv_obj_get_child_cnt(objects.selected_asset_list);
            // if( child_count == 0 ){
            //     showDialog( "Error: select at least one asset" );
            // }else{
            //     navigateTo( SCREEN_ID_SELECT_INSPECTION_TYPE );
            // }                                                    
        }     
        

        if(  key == "7" ){
            navigateTo( SCREEN_ID_SELECT_ASSET_SCREEN );
        }
        if(  key == "9"  ){

            // uint32_t child_count = lv_obj_get_child_cnt(objects.selected_asset_list);
            // if( child_count == 0 ){
            //     showDialog( "Error: select at least one asset" );
            // }else{
            //     navigateTo( SCREEN_ID_SELECT_INSPECTION_TYPE );
            // }                                                    
        }          
    
    }

    void handleTouchEvent( lv_event_t* e ) override{
        lv_obj_t* target = lv_event_get_target(e);

        // make sure one button is selected at a time
        if (lv_obj_check_type(target, &lv_btn_class)) {

            uint32_t child_count = lv_obj_get_child_cnt(objects.inspection_types);
            for (uint32_t i = 0; i < child_count; ++i) {
                lv_obj_t* btn = lv_obj_get_child(objects.inspection_types, i);
                if (!lv_obj_check_type(btn, &lv_btn_class)) continue;

                if (btn == target) {
                    Serial.println("inspection type: click button ...");
                    // LVGL toggles state automatically
                } else {
                    lv_obj_clear_state(btn, LV_STATE_CHECKED);
                }
            }
        }              

        // NAVI  ++++++++++++++++++++++++++
        if( target == objects.back_from_select_insp ){
            navigateTo( SCREEN_ID_SELECT_ASSET_SCREEN );
        }
        if( target == objects.do_inspection_form   ){

            // uint32_t child_count = lv_obj_get_child_cnt(objects.selected_asset_list);
            // if( child_count == 0 ){
            //     showDialog( "Error: select at least one asset" );
            // }else{
            //     navigateTo( SCREEN_ID_SELECT_INSPECTION_TYPE );
            // }                                                    
        }            
}


    void init() override {

        screenClass::init();

        {
            //-------------------------------------
            // Add focusable widgets

            // default
            lv_group_add_obj(inputGroup, objects.inspection_types  );

            // nav bar
            lv_group_add_obj(inputGroup, objects.do_inspection_form);            
            lv_group_add_obj(inputGroup, objects.back_from_select_insp);            

        }
    }

    void start() override{

        lv_group_focus_obj(objects.inspection_types);

        domainManagerClass* domain = domainManagerClass::getInstance();

        // LOAD the LIST from ASSETS ...

        // reset
        lv_obj_clean(objects.inspection_types);
        // For each inspection type, check if asset matches ALL selected assets
        for (const inspectionTypeClass& type : *(domain->getInspectionTypes()) ) {
            bool valid = true;
            for (const assetClass& asset : domain->currentInspection.assets) {
                bool found = false;
                for (const String& layout : type.layouts) {
                    if (layout == "ALL" || layout == asset.layoutName) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    valid = false;
                    break;
                }
            }

            if (valid) { // ie all assets share the type

                // Create button for this inspection type
                lv_obj_t* btn = lv_btn_create(objects.inspection_types);
                lv_obj_set_size(btn, 411, 84);
                lv_obj_add_event_cb(btn, action_main_event_dispatcher, LV_EVENT_PRESSED, static_cast<void*>(const_cast<inspectionTypeClass*>(&type)));
                lv_obj_add_flag(btn, LV_OBJ_FLAG_CHECKABLE);
                lv_obj_set_style_bg_color(btn, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_text_color(btn, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_layout(btn, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_flex_track_place(btn, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

                lv_obj_set_user_data(btn, static_cast<void*>(const_cast<inspectionTypeClass*>(&type)));

                if (domain->currentInspection.type == &type) {
                    lv_obj_add_state(btn, LV_STATE_CHECKED);
                }

                // Add label with inspection type name
                {
                    lv_obj_t* label = lv_label_create(btn);
                    lv_obj_set_style_align(label, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(label, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_label_set_text(label, type.name.c_str());
                }
            }
        }


    }

   void stop() override {

        domainManagerClass* domain = domainManagerClass::getInstance();

        // check inspe type ui
        uint32_t child_count = lv_obj_get_child_cnt(objects.inspection_types);
        for (uint32_t i = 0; i < child_count; ++i) {

            // get buttons
            lv_obj_t* btn = lv_obj_get_child(objects.inspection_types, i);
            if (!lv_obj_check_type(btn, &lv_btn_class)) continue;

            // find selected
            if (lv_obj_has_state(btn, LV_STATE_CHECKED)) {
                inspectionTypeClass* selectedType =
                    static_cast<inspectionTypeClass*>(lv_obj_get_user_data(btn));

                if (selectedType != NULL) {
                    domain->currentInspection.type = selectedType;
                    Serial.println("syncToInspection: Selected inspection type set by user_data.");
                } else {
                    domain->currentInspection.type = NULL;
                    Serial.println("syncToInspection: WARNING — user_data is NULL!");
                    sosBlink();
                }
                return; // found checked → done
            }
        }

    }

};


