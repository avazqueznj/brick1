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

extern "C" void action_main_event_dispatcher(lv_event_t * e);
extern "C" void action_message_box_event_handler(lv_event_t * e);

class selectAssetScreenClass:public screenClass{
public:

    lv_obj_t* selectedButton = NULL;

    //----------------------------------

    selectAssetScreenClass( settingsClass* settingsParam ): screenClass( settingsParam, SCREEN_ID_SELECT_ASSET_SCREEN ){    
    }

    virtual ~selectAssetScreenClass(){};    


    //----------------------------------

    void clockTic( String time ) override {
        lv_label_set_text( objects.clock_asset, time.c_str());
        lv_label_set_text(  objects.driver_name_asset, domainManagerClass::getInstance()->loggedUser.name.c_str()  );        
    }    

    
    void batteryInfo( String info ) override {
        lv_label_set_text( objects.battery_asset, info.c_str());
    }

    virtual void rfidEvent(byte *uid, byte length) override {

        // 1) Convert UID to same tag format
        String rfidTag = ":";
        for (byte i = 0; i < length; i++) {
            rfidTag += ":";
            rfidTag += String(uid[i]);
        }

        Serial.print("RFID event tag = ");
        Serial.println(rfidTag);

        // is this a double read?
        {
            uint32_t child_count = lv_obj_get_child_cnt(objects.selected_asset_list);
            for (uint32_t i = 0; i < child_count; ++i) {
                lv_obj_t* child = lv_obj_get_child(objects.selected_asset_list, i);

                if (lv_obj_check_type(child, &lv_btn_class)) {
                    assetClass* childAsset = static_cast<assetClass*>(lv_obj_get_user_data(child));
                    if (childAsset && childAsset->tag == rfidTag) {
                        Serial.println("RFID asset already in selected list — ignoring event.");
                        return; // Already selected, skip the rest
                    }
                }
            }        
        }


            // 2) Find matching asset and button
            assetClass* matchedAsset = nullptr;
            uint32_t count = lv_obj_get_child_cnt(objects.asset_list);
            for (uint32_t i = 0; i < count; ++i) {
                lv_obj_t* btn = lv_obj_get_child(objects.asset_list, i);
                assetClass* asset = static_cast<assetClass*>(lv_obj_get_user_data(btn));
                if (asset && asset->tag == rfidTag) {
                matchedAsset = asset;
                selectedButton = btn;
                break;
                }
            }
            if (!matchedAsset) {
                showDialog("No matching asset found for this tag!");
                return;
            }

            // *** NEW GUARD: layout must exist in synced layouts ***
            if (!isLayoutSynced(matchedAsset->layoutName)) {
                String msg = "Layout ";
                msg += matchedAsset->layoutName;
                msg += " was not sync, contact the administrator";
                showDialog(msg);
                return;
            }

            // 3) Mark button visually selected, unselect others
            count = lv_obj_get_child_cnt(objects.asset_list);
            for (uint32_t i = 0; i < count; ++i) {
            lv_obj_t* btn = lv_obj_get_child(objects.asset_list, i);
                if (btn == selectedButton) {
                lv_obj_add_state(btn, LV_STATE_CHECKED);
                } else {
                lv_obj_clear_state(btn, LV_STATE_CHECKED);
                }
            }

            Serial.print("RFID matched asset ID: ");
            Serial.println(matchedAsset->ID);


        // 5) Check inspection type
        if (!hasCommonInspectionType(matchedAsset)) {
            showDialog("Error: The assets selected do not have a common inspection type!");
            return;
        }

        // 6) Add it
        addAssetToList(objects.selected_asset_list, matchedAsset, false);

        Serial.println("RFID asset added to selected list!");
    }


    //----------------------------------


   void handleKeyboardEvent( String key ) override {        
        screenClass::handleKeyboardEvent( key );
        lv_obj_t* focused = lv_group_get_focused(inputGroup);

        updateSelected();

        Serial.println("select: by key ..." + key );                         

        if (key.length() == 1 && isdigit(key[0])) {
            // key is a single digit character
            if (focused == objects.search_asset) {
                lv_textarea_add_char(objects.search_asset, key[0]);
            }
        }

        // special short cut to add remove assets        
        if( key == "#" || key == "*" ){     


            // NAVI  ++++++++++++++++++++++++++
            if( focused == objects.back_from_select_asset && key == "#" ){
                navigateTo( SCREEN_ID_MAIN );
            }
            if( focused == objects.do_select_inspection_type && key == "#" ){

                uint32_t child_count = lv_obj_get_child_cnt(objects.selected_asset_list);
                if( child_count == 0 ){
                    showDialog( "Error: select at least one asset" );
                }else{
                    navigateTo( SCREEN_ID_SELECT_INSPECTION_TYPE );
                }                                                    
            }


            if( focused == objects.search_asset_clear && key == "#" ){
                lv_textarea_set_text(objects.search_asset, "");
            }

            // on the seatch and #
            if (focused && focused == objects.search_asset   ){
                if( key == "#" ){
                    doSelectAsset();
                }
            }

            // on the seatch and #
            if (focused && focused == objects.search_asset   ){
                if( key == "*" ){
                    deselectAsset();
                    lv_textarea_set_text(objects.search_asset, "");
                }
            }

            // on the list and #
            if (focused && focused == objects.asset_list   ){
                if( key == "#" ){
                    doSelectAsset();
                }
            }

            // on the list and *
            if (focused && focused == objects.asset_list   ){
                if(  key == "*" ){
                    deselectAsset();
                }
            }

            // on ">"
            if ( focused && focused == objects.select_asset  ){
                if(  key == "#" ){
                    doSelectAsset();
                }
            }

            // on "<"
            if ( focused && focused == objects.de_select_asset ){
                if(  key == "#" ){
                    deselectAsset();                        
                }
                if(  key == "*" ){
                    deselectAsset();
                }

            }
        }        
    }




    void handleTouchEvent( lv_event_t* e ) override{
        lv_obj_t* target = lv_event_get_target(e);
        lv_obj_t* parent = lv_obj_get_parent(target);

        updateSelected();   


        // temp asset =======================================

            // defect list scroll reset 
            if (  lv_obj_check_type(target, &lv_btn_class) &&  parent == objects.temp_asset_layouts ) {
                
                Serial.println("layout click");
                
                uint32_t child_count = lv_obj_get_child_cnt( objects.temp_asset_layouts ); 
                for (uint32_t i = 0; i < child_count; ++i) {                
                    lv_obj_t* btn = lv_obj_get_child( objects.temp_asset_layouts , i);
                    if (!lv_obj_check_type(btn, &lv_btn_class)) continue;

                    if (btn != target) {
                        lv_obj_clear_state(btn, LV_STATE_CHECKED);
                    } else {
                        lv_obj_add_state(btn, LV_STATE_CHECKED);
                    }

                }


                Serial.println("temp_asset_layouts click DONE");
                return;
            }     


            if( tempAssetDialogOpen ){
                
                if( target == objects.temp_asset_ok ){
                    String layoutName = "";                    
                    uint32_t i = 0;
                    lv_obj_t* btn = lv_obj_get_child(objects.temp_asset_layouts, i);
                    while (btn) {                        
                        if (lv_obj_has_state(btn, LV_STATE_CHECKED)){
                            layoutName = *((String*) lv_obj_get_user_data(btn));                            
                            break;                        
                        } 
                        ++i;
                        btn = lv_obj_get_child(objects.temp_asset_layouts, i);                
                    }       
                    if( layoutName == "" ) throw std::runtime_error( "No layout selected in temp asset" );
                    String name = sanitizeEDIValue( lv_textarea_get_text(objects.temp_asset_id) );
                    Serial.println( "User asset --> [" + name + ":" + layoutName + "]" );
                    assetClass newAsset( name, layoutName, "" );
                    domainManagerClass::getInstance()->tempAssets.push_back( newAsset );
                    addAssetToList( objects.selected_asset_list , &(domainManagerClass::getInstance()->tempAssets.back()), false ); 
                    closeTempAssetDialog();
                }

                if( target == objects.temp_asset_cancel ){
                    closeTempAssetDialog();
                }
            
                return;
            }

            if( target == objects.temp_asset_button ){
                openTempAssetDialog();
            }
        
        // temp asset =======================================            
        
        // NAVI  ++++++++++++++++++++++++++
        if( target == objects.back_from_select_asset){
            navigateTo( SCREEN_ID_MAIN );
        }
        if( target == objects.do_select_inspection_type  ){

            uint32_t child_count = lv_obj_get_child_cnt(objects.selected_asset_list);
            if( child_count == 0 ){
                showDialog( "Error: select at least one asset" );
            }else{
                navigateTo( SCREEN_ID_SELECT_INSPECTION_TYPE );
            }
                        
        }
    
        // CLICK search clear
        if( target == objects.search_asset_clear  ){
            lv_textarea_set_text(objects.search_asset, "");
        }

        // CLICK select
        if( target == objects.select_asset  ){
            doSelectAsset();
        }

        // CLICK deselect selected
        if( target == objects.de_select_asset ){
            deselectAsset();
        }

        // unselect no targeted
        uint32_t count = lv_obj_get_child_cnt(objects.asset_list);
        for (uint32_t i = 0; i < count; ++i) {
            lv_obj_t* btn = lv_obj_get_child(objects.asset_list, i);

            if (btn == target) {      // if the target was a  list button only!!! then           
                Serial.println("select: click button ...");                              
                selectedButton = btn;

                uint32_t count = lv_obj_get_child_cnt(objects.asset_list);
                for (uint32_t i = 0; i < count; ++i) {
                lv_obj_t* btn = lv_obj_get_child(objects.asset_list, i);
                    if (btn != target) {
                        lv_obj_clear_state(btn, LV_STATE_CHECKED);
                    }
                }
                Serial.println("select: click button ... DONE" );                                                                  

                break;
            }
        }  
            
            
        // search 
        if (target == objects.search_asset && lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {            
            const char* searchText = lv_textarea_get_text(objects.search_asset);
            if (searchText && strlen(searchText) > 0) {
                // Lowercase for case-insensitive matching (if needed)
                String query = String(searchText);
                query.toLowerCase();

                uint32_t count = lv_obj_get_child_cnt(objects.asset_list);
                for (uint32_t i = 0; i < count; ++i) {
                    lv_obj_t* btn = lv_obj_get_child(objects.asset_list, i);
                    assetClass* asset = static_cast<assetClass*>(lv_obj_get_user_data(btn));
                    if (asset) {
                        String btnName = asset->buttonName;
                        btnName.toLowerCase();
                        if (btnName.indexOf(query) != -1) {
                            // Match found: select visually and call doSelectAsset()
                            selectedButton = btn;

                            // Mark as checked, uncheck others
                            for (uint32_t j = 0; j < count; ++j) {
                                lv_obj_t* b = lv_obj_get_child(objects.asset_list, j);            
                                if (b == btn) {
                                    lv_obj_add_state(b, LV_STATE_CHECKED);
                                } else {
                                    lv_obj_clear_state(b, LV_STATE_CHECKED);
                                }
                            }

                            lv_obj_scroll_to_view(selectedButton, LV_ANIM_ON);

                            //doSelectAsset();
                            break; // Only first match
                        }
                    }
                }
            }            
        }        

    }


    void updateSelected(){

        // update if changes in main source asset list
        lv_obj_t* list = objects.asset_list;
        if (!list) return;
        selectedButton = nullptr;  // Reset
        
        // find selected currently
        uint32_t count = lv_obj_get_child_cnt(list);
        for (uint32_t i = 0; i < count; ++i) {
            lv_obj_t* btn = lv_obj_get_child(list, i);
            if (lv_obj_has_state(btn, LV_STATE_CHECKED)) {
                selectedButton = btn;
                Serial.println("Updated selectedButton pointer from list");
                break;
            }
        }
        if (!selectedButton) {
            Serial.println("No CHECKED button found in list.");
        }        
    }


    
    //----------------------------------

    void doSelectAsset(){

        Serial.println("select: do select");              
        if( selectedButton != NULL ){

            // get selected asset
            assetClass* asset = (assetClass*) lv_obj_get_user_data(selectedButton);
            if (!asset) return;

            // *** NEW GUARD: layout must exist in synced layouts ***
            if (!isLayoutSynced(asset->layoutName)) {
                String msg = "Layout ";
                msg += asset->layoutName;
                msg += " was not sync, contact the administrator";
                showDialog(msg);
                return;
            }            
            
            // Check if asset is already in selected_asset_list
            uint32_t child_count = lv_obj_get_child_cnt(objects.selected_asset_list);
            for (uint32_t i = 0; i < child_count; ++i) {
                lv_obj_t* child = lv_obj_get_child(objects.selected_asset_list, i);

                if (lv_obj_check_type(child, &lv_btn_class)) {
                    assetClass* childAsset = static_cast<assetClass*>(lv_obj_get_user_data(child));                    
                    if (childAsset && childAsset == asset) {
                        return;  // Asset already in list, skip adding
                    }
                }
            }

            if (!hasCommonInspectionType(asset)) {
                showDialog("Error: The asset selected do have a common inspection type with selected asset!");
                return;
            }

            // else, add it
            addAssetToList( objects.selected_asset_list ,  asset, false ); 
        }       
        Serial.println("select: do select  OK");                           
        return;

    }

    //----------------------------------

    void deselectAsset(){
        Serial.println("select: do deselect");              

        lv_obj_clean(objects.selected_asset_list);   
        domainManagerClass::getInstance()->currentInspection.clear();

        Serial.println("select: do deselect OK");                          
        return;

    }

    //----------------------------------

    void init() override {


        {
            //-------------------------------------
            // Add focusable widgets

            lv_group_add_obj(inputGroup, objects.search_asset );            
            //lv_group_add_obj(inputGroup, objects.search_asset_clear);            

            lv_group_add_obj(inputGroup, objects.asset_list  );
            // lv_group_add_obj(inputGroup, objects.select_asset);
            // lv_group_add_obj(inputGroup, objects.de_select_asset);

            // nav bar 
            lv_group_add_obj(inputGroup, objects.do_select_inspection_type);            
            lv_group_add_obj(inputGroup, objects.back_from_select_asset);            

        }
        
        screenClass::init(); // always last, only if no issues

        // setup temp asset dialog
            domainManagerClass* domain = domainManagerClass::getInstance();

            lv_obj_t* close_btn = lv_msgbox_get_close_btn(objects.temp_asset_dialog);
            if (close_btn){
                lv_obj_del(close_btn);         
            }         

            lv_obj_add_flag(  objects.temp_asset_dialog , LV_OBJ_FLAG_HIDDEN);     
            lv_obj_add_flag(  objects.temp_asset_overlay, LV_OBJ_FLAG_HIDDEN);    
            lv_textarea_set_text( objects.temp_asset_id, "" );    
            
            lv_obj_clean(objects.temp_asset_layouts);
            const auto* layouts = domain->getLayouts();
            for (const layoutClass& layout : *layouts) {

                lv_obj_t* layout_button = lv_btn_create(objects.temp_asset_layouts);                
                lv_obj_set_user_data(layout_button, (void*)&(layout.name) );                
                lv_obj_add_event_cb(layout_button, action_main_event_dispatcher, LV_EVENT_PRESSED, NULL );

                lv_obj_set_size(layout_button, 339, 50);                
                lv_obj_set_style_bg_color(layout_button, lv_color_hex(0xffdddddd), LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_text_color(layout_button, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_layout(layout_button, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_flex_track_place(layout_button, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                //lv_obj_add_flag(defect_btn, LV_OBJ_FLAG_CHECKABLE);

                lv_obj_t* label = lv_label_create(layout_button);
                lv_obj_set_style_text_font(label, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_label_set_text(label, layout.name.c_str());
            }

            // default top
            lv_obj_t* btn = lv_obj_get_child( objects.temp_asset_layouts , 0);
            if (lv_obj_check_type(btn, &lv_btn_class)){
                lv_obj_add_state(btn, LV_STATE_CHECKED);
            }
                                    
        // setup temp asset dialog

        screenClass::makeKeyboards();
        screenClass::addLetterKeyboard( objects.search_asset);
        screenClass::addLetterKeyboard( objects.temp_asset_id);
    }

    void start() override{

        Serial.println( "Select asset start +++++++++++++++++++++++++++++++++++" );

        domainManagerClass* domain = domainManagerClass::getInstance();        
        domain->currentInspection.clear();
    
        // clean        
        lv_obj_clean(objects.asset_list); 
        lv_obj_clean(objects.selected_asset_list); 
        lv_textarea_set_text(objects.search_asset, "");
        domainManagerClass::getInstance()->tempAssets.clear();

        // add assets to select
        for (const assetClass& asset : *(domain->getAssets()) ) {
            addAssetToList(objects.asset_list, &asset, true);
            for (assetClass& selected : domain->currentInspection.assets) {
                if (asset.ID == selected.ID) {
                    addAssetToList(objects.selected_asset_list, &asset, false);
                    break;
                }
            }
        }

    }

    void stop() override{

        domainManagerClass* domain = domainManagerClass::getInstance();         
        
        domain->currentInspection.clear();

        // prepare new inspection
        domain->currentInspection.driver_name = domain->loggedUser.name;
        domain->currentInspection.driver_username = domain->loggedUser.username;
        domain->currentInspection.company = domain->company;
        domain->currentInspection.startTime = lv_label_get_text( objects.clock_asset );
        domain->currentInspection.offset = String( domain->timeOffsetFromUTC );
        domain->currentInspection.dst = String( domain->DST );

        // Count selected asset buttons
        domain->currentInspection.assets.clear();
        uint32_t child_count = lv_obj_get_child_cnt(objects.selected_asset_list);
        for (uint32_t i = 0; i < child_count; ++i) {
            lv_obj_t* child = lv_obj_get_child(objects.selected_asset_list, i);

            if (lv_obj_check_type(child, &lv_btn_class)) {
                assetClass* asset = static_cast<assetClass*>(lv_obj_get_user_data(child));
                if (asset) {
                    domain->currentInspection.assets.push_back(*asset);
                }
            }
        }

    }

    // checkable for source list uncheckable to selected list
    lv_obj_t* addAssetToList( lv_obj_t* parent_obj, const assetClass* asset, bool checkable ){
                    
        lv_obj_t* button = lv_btn_create(parent_obj);
        lv_obj_set_pos(button, 503, 42);
        lv_obj_set_size(button, 293, 50);

        lv_obj_add_event_cb(button, action_main_event_dispatcher, LV_EVENT_PRESSED, const_cast<assetClass*>(asset) );

        if( checkable ) lv_obj_add_flag(button, LV_OBJ_FLAG_CHECKABLE);

        lv_obj_set_user_data(button, const_cast<assetClass*>(asset));

        lv_obj_set_style_bg_color(button, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_text_color(button, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_layout(button, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_set_style_flex_track_place(button, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
        {
            lv_obj_t *parent_obj = button;
            {
                lv_obj_t *obj = lv_label_create(parent_obj);
                lv_obj_set_pos(obj, 0, 0);
                lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
                lv_obj_set_style_align(obj, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_text_font(obj, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_label_set_text(obj, asset->buttonName.c_str() );
            }
        }

        return( button );            
    }
    
    
    //-------------------------------


    bool hasCommonInspectionType(const assetClass* newAsset) {
        domainManagerClass* domain = domainManagerClass::getInstance();

        for (const inspectionTypeClass& type : *(domain->getInspectionTypes()) ) {
            bool newAssetOk = false;
            bool allSelectedOk = true;

            // --- Does this type cover the NEW asset?
            for (const String& layout : type.layouts) {
                if (layout == "ALL" || layout == newAsset->layoutName) {
                    newAssetOk = true;
                    break;
                }
            }

            if (!newAssetOk) continue;

            // --- Does this type cover ALL SELECTED assets?
            uint32_t child_count = lv_obj_get_child_cnt(objects.selected_asset_list);
            for (uint32_t i = 0; i < child_count; ++i) {
                lv_obj_t* child = lv_obj_get_child(objects.selected_asset_list, i);

                if (lv_obj_check_type(child, &lv_btn_class)) {
                    assetClass* childAsset = static_cast<assetClass*>(lv_obj_get_user_data(child));
                    if (childAsset) {
                        bool found = false;
                        for (const String& layout : type.layouts) {
                            if (layout == "ALL" || layout == childAsset->layoutName) {
                                found = true;
                                break;
                            }
                        }
                        if (!found) {
                            allSelectedOk = false;
                            break;
                        }
                    }
                }
            }

            if (newAssetOk && allSelectedOk) {
                return true;  // Found valid inspection type
            }
        }

        return false;  // No valid type found
    }

    bool isLayoutSynced(const String& layoutName) {
        domainManagerClass* domain = domainManagerClass::getInstance();
        const std::vector<layoutClass, SDRAMAllocator<layoutClass>>* layouts = domain->getLayouts();

        for (size_t i = 0; i < layouts->size(); i++) {
            const layoutClass& layout = (*layouts)[i];
            if (layout.name == layoutName) {
                return true;
            }
        }

        return false;
    }    

    //==========================================================================

    void openTempAssetDialog(){
        Serial.println("temp asset dialog click ...");
        lv_obj_clear_flag(  objects.temp_asset_dialog, LV_OBJ_FLAG_HIDDEN);     
        lv_obj_clear_flag(  objects.temp_asset_overlay, LV_OBJ_FLAG_HIDDEN);     
        tempAssetDialogOpen = true;       
        Serial.println("temp asset dialog click done!");
    }

    bool tempAssetDialogOpen = false;
    void closeTempAssetDialog(){
        Serial.println("Close temp asset dialog ...");                
        if( tempAssetDialogOpen ){
            lv_obj_add_flag(  objects.temp_asset_dialog, LV_OBJ_FLAG_HIDDEN);   
            lv_obj_add_flag(  objects.temp_asset_overlay, LV_OBJ_FLAG_HIDDEN);   
            lv_textarea_set_text( objects.temp_asset_id, "" );         
        }
        tempAssetDialogOpen = false;
    }

    //==========================================================================    

};
