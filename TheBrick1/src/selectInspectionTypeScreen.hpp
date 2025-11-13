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
                if( !saveSelected() ){
                    showDialog( "Error: Select an inspection type" );
                }else{
                    navigateTo( SCREEN_ID_INSPECTION_FORM );                
                }                                               
            }
        }    
        
        // NAVI ================================
        if( focused == objects.back_from_select_insp&& key == "#" ){
            navigateTo( SCREEN_ID_SELECT_ASSET_SCREEN );
        }

        if( focused == objects.do_inspection_form && key == "#"  ){
            if( !saveSelected() ){
                showDialog( "Error: Select an inspection type" );
            }else{
                navigateTo( SCREEN_ID_INSPECTION_FORM );                
            }                                               
        }     
        

 
    }

    void handleTouchEvent( lv_event_t* e ) override{
        lv_obj_t* target = lv_event_get_target(e);

        // make sure one button is selected at a time
        if (lv_obj_check_type(target, &lv_btn_class)) {

            // find button and buffer it
            lv_obj_t* selected = NULL;

            uint32_t child_count = lv_obj_get_child_cnt(objects.inspection_types);

            for (uint32_t i = 0; i < child_count; ++i) {
                lv_obj_t* btn = lv_obj_get_child(objects.inspection_types, i);
                if (!lv_obj_check_type(btn, &lv_btn_class)) continue;
                if (btn == target) selected = btn;
            }

            if( selected !=  NULL ){
                for (uint32_t i = 0; i < child_count; ++i) {
                    lv_obj_t* btn = lv_obj_get_child(objects.inspection_types, i);
                    if (!lv_obj_check_type(btn, &lv_btn_class)) continue;
                    if (btn != selected) lv_obj_clear_state(btn, LV_STATE_CHECKED);
                }
            }            
        }              

        // NAVI  ++++++++++++++++++++++++++
        if( target == objects.back_from_select_insp ){
            navigateTo( SCREEN_ID_SELECT_ASSET_SCREEN );
        }

        if( target == objects.do_inspection_form   ){
            if( !saveSelected() ){
                showDialog( "Error: Select an inspection type" );
            }else{
                navigateTo( SCREEN_ID_INSPECTION_FORM );                
            }                                                                                              
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
        
        // add all inspi types shared with the selected asset
        for (const inspectionTypeClass& type : *(domain->getInspectionTypes()) ) {  // for each layout
            bool valid = true;
            for (const assetClass& asset : domain->currentInspection.assets) { // is it compaible with all assets
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

            // yes compatible, add it to the list for selection
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

   
    bool saveSelected(){
        domainManagerClass* domain = domainManagerClass::getInstance();

        // save selected type
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
                    Serial.println("SAVED selected " +  selectedType->name );
                    return true;
                } else {
                    domain->currentInspection.type = NULL;
                    Serial.println("Type in button was NULL");
                    sosBlink("Type in button was NULL");
                }
            }
        }   
        
        return false;
    }


    void stop() override {

    }

};


