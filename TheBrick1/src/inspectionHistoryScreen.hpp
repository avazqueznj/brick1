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
class inspectionHistoryScreenClass:public screenClass{
public:

    std::vector<String> inspectionHistory;
    String inspectionEDI = "";
    String inspectionTEXT = "";
    String currentEDIPath = "";

    inspectionHistoryScreenClass( settingsClass* settings ): screenClass( settings, SCREEN_ID_INSPECTION_HISTORY ){    
    }


    void clockTic( String time ) override {
        lv_label_set_text( objects.clock_history, time.c_str());
        lv_label_set_text(  objects.driver_name_history, domainManagerClass::getInstance()->loggedUser.name.c_str()  );        
    }    

    
    void batteryInfo( String info ) override {
        lv_label_set_text( objects.battery_history, info.c_str());
    }

    void handleKeyboardEvent( String key ) override {    
    
        if( !lv_obj_has_flag(objects.inspection_detail_dialog, LV_OBJ_FLAG_HIDDEN) ){
            
            // modal is open -->

            if(  key == "4"  ){
                lv_obj_add_flag(  objects.inspection_detail_dialog, LV_OBJ_FLAG_HIDDEN);   
                lv_textarea_set_text(objects.inspection_view, "" );
            }        
            
            if (  key == "3"  ){        
                static const char* btns[] = { "Ok", "Cancel", "" };
                showDialog( "Submit inspection?", "Submit", btns );
            }        

            return; // modal
        }
    
        screenClass::handleKeyboardEvent( key );
        lv_obj_t* focused = lv_group_get_focused(inputGroup);
        
        // NAVI ================================
        if( focused == objects.back_from_history  && key == "#" ){
            navigateTo( SCREEN_ID_MAIN );
        }

        if( ( focused == objects.open_inspection  && key == "#" ) || key == "1" ){
            openInspectionDetail();
        }
        



    }

    void handleTouchEvent( lv_event_t* e ) override{
        lv_obj_t* target = lv_event_get_target(e);

        // make sure one button is selected at a time
        if (lv_obj_check_type(target, &lv_btn_class)) {

            // find button and buffer it
            lv_obj_t* selected = NULL;

            uint32_t child_count = lv_obj_get_child_cnt(objects.history_list);
            for (uint32_t i = 0; i < child_count; ++i) {
                lv_obj_t* btn = lv_obj_get_child(objects.history_list, i);
                if (!lv_obj_check_type(btn, &lv_btn_class)) continue;
                if (btn == target) selected = btn;
            }

            if( selected !=  NULL ){
                for (uint32_t i = 0; i < child_count; ++i) {
                    lv_obj_t* btn = lv_obj_get_child(objects.history_list, i);
                    if (!lv_obj_check_type(btn, &lv_btn_class)) continue;
                    if (btn != selected) lv_obj_clear_state(btn, LV_STATE_CHECKED);
                }
            }            
        }              

        // NAVI  ++++++++++++++++++++++++++
        if( target == objects.back_from_history ){
            navigateTo( SCREEN_ID_MAIN );
        }

        if( target == objects.open_inspection ){
            openInspectionDetail();
        }

        if( target == objects.history_close ){
            lv_obj_add_flag(  objects.inspection_detail_dialog, LV_OBJ_FLAG_HIDDEN);   
            lv_textarea_set_text(objects.inspection_view, "" );
        }

        if( target == objects.re_submit_inspection ){
            static const char* btns[] = { "Ok", "Cancel", "" };
            showDialog( "Submit inspection?", "Submit", btns );
        }

    }


    void init() override {

        screenClass::init();

        lv_obj_t* close_btn = lv_msgbox_get_close_btn(objects.inspection_detail_dialog);
        if (close_btn){
            lv_obj_del(close_btn);         
        }         

        {
            //-------------------------------------
            // Add focusable widgets

            // default
            lv_group_add_obj(inputGroup, objects.history_list  );

            // nav bar
            lv_group_add_obj(inputGroup, objects.open_inspection);                        
            lv_group_add_obj(inputGroup, objects.back_from_history);            
            

        }
    }

    void start() override{
    
        lv_obj_add_flag(  objects.inspection_detail_dialog, LV_OBJ_FLAG_HIDDEN);   

        lv_group_focus_obj(objects.history_list);
        //domainManagerClass* domain = domainManagerClass::getInstance();

        inspectionHistory = getInspectionHistory();
       

        // reset
        lv_obj_clean(objects.history_list);
        
        
        for ( String& pastInspectionHeader : inspectionHistory ) {  // for each layout

                std::vector<String> tokens = tokenize( pastInspectionHeader, '*' );
                if( tokens.size() < 6 ){
                    sosBlink( "?? Corrupted inspection header ?" + pastInspectionHeader );
                }
        
                // Create button for this inspection type
                lv_obj_t* btn = lv_btn_create(objects.history_list);
                lv_obj_set_size(btn, 1000, 47);                
                lv_obj_add_flag(btn, LV_OBJ_FLAG_CHECKABLE);
                lv_obj_set_style_bg_color(btn, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_text_color(btn, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_layout(btn, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_flex_track_place(btn, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

                lv_obj_add_event_cb(btn, action_main_event_dispatcher, LV_EVENT_PRESSED,  &pastInspectionHeader );                
                lv_obj_set_user_data(btn, &pastInspectionHeader  );

                // if (domain->currentInspection.type == &type) {
                //     lv_obj_add_state(btn, LV_STATE_CHECKED);
                // }

                // Add label with inspection type name
                {
                    lv_obj_t* label = lv_label_create(btn);
                    lv_obj_set_style_align(label, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                    lv_obj_set_style_text_font(label, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);

                    String labelText = 
                        "[" +
                        tokens[ 2 ] + " " +
                        tokens[ 3 ] + "] " +
                        tokens[ 4 ] + "/" +
                        tokens[ 5 ] ;
                    
                    lv_label_set_text(label, labelText.c_str());
                }
        
        }


    }

   

    void stop() override {

    }

    void modalDialogEvent(const String modalActionTouch, const String button) override {

        Serial.println( "Override Modal event " + modalActionTouch + ":" + button );

        if( modalActionTouch == "Submit" && button == "Ok" ){
            doSubmitInspection();
        }

    }

    virtual void modalDialogKey( String key ){    
        screenClass::modalDialogKey( key );

        Serial.println( "Inspection modal key " + modalAction + ":" + key );

        if( modalAction == "Submit" && key == "#" ){
            doSubmitInspection();
        }

    }        


    void openInspectionDetail(){

        lv_textarea_set_text(objects.inspection_view, "" );
        
        try{

            spinnerStart();

            // find selected
            uint32_t child_count = lv_obj_get_child_cnt(objects.history_list);
            for (uint32_t i = 0; i < child_count; ++i) {

                // get buttons
                lv_obj_t* btn = lv_obj_get_child(objects.history_list, i);
                if (!lv_obj_check_type(btn, &lv_btn_class)) continue;

                // find selected, load it
                if (lv_obj_has_state(btn, LV_STATE_CHECKED)) {

                    // load file
                    String path = "/kv/insp" + String(i + 1);
                    Serial.println(" Try read"  + path + " -> " );

                    std::vector<String> row = loadFromKVStore(path);    

                    // show it                    
                    bool afterDataRecord = false;
                    inspectionEDI = "";
                    inspectionTEXT = "";
                    currentEDIPath = path;
                    for (size_t i = 0; i < row.size(); ++i) {

                        if( !afterDataRecord ){
                            inspectionEDI += row[i] + "\n";
                        }

                        if( row[i].startsWith( "END***") ){
                            afterDataRecord = true;
                            continue;
                        }

                        if( afterDataRecord ){
                            inspectionTEXT += row[i];
                            if (i < row.size() - 1) inspectionTEXT += "\n";                             
                        }
                    }

                    inspectionTEXT += "\n";

                    Serial.println(inspectionTEXT.c_str());
                    lv_textarea_set_text(objects.inspection_view, inspectionTEXT.c_str());

                    lv_textarea_set_cursor_pos(objects.inspection_view, 0); // Cursor at very start
                    lv_obj_scroll_to_y(objects.inspection_view, 0, LV_ANIM_OFF);
                    lv_obj_clear_flag(  objects.inspection_detail_dialog, LV_OBJ_FLAG_HIDDEN);   

                    break;
                }   
                                    
            }    

            spinnerEnd();

        }catch(...){

            spinnerEnd();

        }    

    }

    void doSubmitInspection(){

        Serial.println("Submit ...");

        spinnerStart();

        domainManagerClass* domain = domainManagerClass::getInstance(); 
        String result = "";
        try{            

            result =  domain->comms->POST( 
                domain->serverURL, 
                domain->postInspectionsPath + "?company=" + domain->company,  
                inspectionEDI
                );
            
            String filingRecord = inspectionEDI + inspectionTEXT + result +"\n";
            Serial.println("Update ... " + currentEDIPath);
            Serial.println("With ... " + filingRecord);
            saveToKVStore( currentEDIPath,  filingRecord);                
            Serial.println("Submit ... done!");
            spinnerEnd();      

            lv_obj_add_flag(  objects.inspection_detail_dialog, LV_OBJ_FLAG_HIDDEN);   
            lv_textarea_set_text(objects.inspection_view, "" );            

            showDialog( "Submitted!" );        
            
        }catch( const std::runtime_error& error ){
            spinnerEnd();       
            String chainedError = String( "ERROR: Inspection possibly not sent." ) + error.what();           
            showDialog( chainedError.c_str() );

            String filingRecord = inspectionEDI + inspectionTEXT + error.what() +"\n";
            Serial.println("Update ... " + currentEDIPath);
            Serial.println("With ... " + filingRecord);
            saveToKVStore( currentEDIPath,  filingRecord);            
        }


    }    

};



