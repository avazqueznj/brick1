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

std::vector<String> getInspectionHistory() {

    std::vector<String> result;
    result.reserve(NUM_INSPECTION_SLOTS);

    for (int i = 1; i <= NUM_INSPECTION_SLOTS; i++) {

        String path = "/kv/insp";
        path += String(i);

        Serial.print("Try read ");
        Serial.print(path);
        Serial.print(" -> ");

        // get slot
        if (!kvKeyExists(path)) {
            Serial.println("Cannot read...");
            continue;
        }

        // else read it
        try {

            std::vector<String> file = loadFromKVStore(path);

            // extract header
            for (size_t j = 0; j < file.size(); j++) {
                if (file[j].startsWith("DISPLAYHEADER*")) {
                    Serial.print("Slot ");
                    Serial.print(i);
                    Serial.print(": ");
                    Serial.println(file[j]);
                    result.push_back(file[j]);             
                    break; // done
                }
            }

        } catch (...) {
            Serial.println("ERROR read slot");
        }
    }

    return result;
}

void zapInspectionHistory() {

    for (int i = 1; i <= NUM_INSPECTION_SLOTS; i++) {

        // make filename
        String path = "/kv/insp";
        path += String(i);

        // zap it!
        Serial.print( "Zap " + path + " " );
        if( kv_remove(path.c_str()) == MBED_SUCCESS ){
            Serial.println( "Zapped  " );
        }else{
            Serial.println( "Zap error " );
        }

    }

    Serial.println("All inspection slots deleted.");
}


#include "core/lv_event.h"
class inspectionHistoryScreenClass:public screenClass{
public:

    std::vector<String> inspectionHistory;
    String inspectionEDI = "";
    String inspectionTEXT = "";
    String currentEDIPath = "";

    inspectionHistoryScreenClass( settingsClass* settingsParam ): screenClass( settingsParam, SCREEN_ID_INSPECTION_HISTORY ){    
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

    void start() override {

        lv_obj_add_flag(objects.inspection_detail_dialog, LV_OBJ_FLAG_HIDDEN);
        lv_group_focus_obj(objects.history_list);

        // 1) Get headers in slot order (1..NUM_INSPECTION_SLOTS)
        inspectionHistory = getInspectionHistory();

        // 2) Build a small struct array with slot + header + unixTime
        struct HistoryRow {
            int slot;
            String header;
            long unixTime;
        };
        // build list for sorting
        std::vector<HistoryRow> rows;
        rows.reserve(inspectionHistory.size());
        for (size_t index = 0; index < inspectionHistory.size(); index++) {
            HistoryRow row;
            row.slot = (int)(index + 1);          // slot = index + 1 (slot order)
            row.header = inspectionHistory[index];
            std::vector<String> tokens = tokenize(row.header, '*');
            if (tokens.size() < 2) {
                sosHALT("Bad DISPLAYHEADER (no time): " + row.header);
            }
            long t = tokens[1].toInt();           // Unix time from header
            row.unixTime = t;
            rows.push_back(row);
        }

        // 3) Sort rows by time (newest first)
        if (rows.size() > 1) {
            std::sort(
                rows.begin(),
                rows.end(),
                [](const HistoryRow& a, const HistoryRow& b) {
                    if (a.unixTime != b.unixTime) {
                        return a.unixTime > b.unixTime; // newer first
                    }
                    // tie-breaker: lower slot first (arbitrary but stable)
                    return a.slot < b.slot;
                }
            );
        }

        // 4) Rebuild inspectionHistory to match UI order (optional but nice)
        inspectionHistory.clear();
        inspectionHistory.reserve(rows.size());
        for (size_t i = 0; i < rows.size(); i++) {
            inspectionHistory.push_back(rows[i].header);
        }

        // 5) Reset LVGL list and build buttons in sorted order
        lv_obj_clean(objects.history_list);

        for (size_t i = 0; i < rows.size(); i++) {

            HistoryRow& row = rows[i];
            String& pastInspectionHeader = inspectionHistory[i];

            std::vector<String> tokens = tokenize(pastInspectionHeader, '*');
            if (tokens.size() < 7) {
                sosHALT("?? Corrupted inspection header ?" + pastInspectionHeader);
            }

            // Create button for this inspection
            lv_obj_t* btn = lv_btn_create(objects.history_list);
            lv_obj_set_size(btn, 1500, 47);
            lv_obj_add_flag(btn, LV_OBJ_FLAG_CHECKABLE);
            lv_obj_set_style_bg_color(btn, lv_color_hex(0xffffffff),
                                    LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(btn, lv_color_hex(0xff000000),
                                        LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(btn, LV_LAYOUT_FLEX,
                                    LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_track_place(btn, LV_FLEX_ALIGN_CENTER,
                                            LV_PART_MAIN | LV_STATE_DEFAULT);

            // Keep your dispatcher user data (header pointer) if you need it
            lv_obj_add_event_cb(btn, action_main_event_dispatcher,
                                LV_EVENT_PRESSED, &pastInspectionHeader);

            // *** IMPORTANT: store the KV slot number in the button's user_data ***
            // Need <stdint.h> for intptr_t
            intptr_t slotValue = (intptr_t)row.slot;
            lv_obj_set_user_data(btn, (void*)slotValue);

            // Label with formatted info
            lv_obj_t* label = lv_label_create(btn);
            lv_obj_set_style_align(label, LV_ALIGN_CENTER,
                                LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(label, &lv_font_montserrat_28,
                                    LV_PART_MAIN | LV_STATE_DEFAULT);

            String labelText =
                tokens[6] + " " +
                tokens[3] + " " +
                tokens[4] + "/" +
                tokens[5] +
                " [" + tokens[2] + "]";

            lv_label_set_text(label, labelText.c_str());
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
          
                    // load file: slot is stored in button user_data
                    intptr_t rawSlot = (intptr_t)lv_obj_get_user_data(btn);
                    int slot = (int)rawSlot;
                    if (slot <= 0) {
                        sosHALT("Invalid slot in openInspectionDetail");
                    }

                    String path = "/kv/insp";
                    path += String(slot);

                    Serial.print(" Try read ");
                    Serial.print(path);
                    Serial.println(" -> ");

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
        try{

            domainManagerClass::getInstance()->doReSubmitInspection( 
                currentEDIPath,
                inspectionEDI, 
                inspectionTEXT 
            );            
            spinnerEnd();      
            Serial.println("Submit ... done!");        
            showDialog( "Submitted!" );
                        
        }catch( const std::runtime_error& error ){

            spinnerEnd();       
            String chainedError = String( "ERROR: Inspection saved, but possibly not sent:" ) + error.what();                       
            showDialog( chainedError.c_str() );
        }

        start();        

    }

};





