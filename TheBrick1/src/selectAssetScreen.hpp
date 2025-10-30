/********************************************************************************************
 * CONFIDENTIAL AND PROPRIETARY
 * 
 * ZZZ2025 
 * © [2025] [Alejandro Vazquez]. All rights reserved.
 * 
 ********************************************************************************************/

#include <deque>

extern "C" void action_main_event_dispatcher(lv_event_t * e);
extern "C" void action_message_box_event_handler(lv_event_t * e);

class selectAssetScreenClass:public screenClass{
public:

    lv_obj_t* selectedButton = NULL;

    //----------------------------------


    selectAssetScreenClass( settingsClass* settings ): screenClass( settings, SCREEN_ID_SELECT_ASSET_SCREEN ){    
    }

    virtual ~selectAssetScreenClass(){};    


    //----------------------------------

    void clockTic( String time ) override {
        lv_label_set_text( objects.clock_asset, time.c_str());
        lv_label_set_text(  objects.driver_name_asset, domainManagerClass::getInstance()->loggedUser.name.c_str()  );        
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
                createDialog("No matching asset found for this tag!");
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
            createDialog("Error: The assets selected do not have a common inspection type!");
            return;
        }

        // 6) Add it
        addAssetToList(objects.selected_asset_list, matchedAsset, false);

        Serial.println("RFID asset added to selected list!");
    }


    //----------------------------------

    void handleEvents( lv_event_t* e, String key ) override{
        screenClass::handleEvents( e, key );        
        lv_obj_t *target = lv_event_get_target(e);    

        Serial.print( key );

        // update if changes in main source asset list
        lv_obj_t* list = objects.asset_list;
        if (!list) return;
        selectedButton = nullptr;  // Reset
        
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

        // keyboard events
        if( key != "" ){

            // special short cut to add remove assets        
            if( key == "#" || key == "*" ){     
                 
                Serial.println("select: by key ..." + key );                         
                lv_obj_t* focused = lv_group_get_focused(inputGroup);

                // on the list and #
                if (focused && focused == objects.asset_list   ){
                    if( key == "#" ){
                        // clicked # enter ? then add assset short cut
                        doSelectAsset();
                    }
                }

                // on the list and *
                if (focused && focused == objects.asset_list   ){
                    if(  key == "*" ){
                        // clicked # enter ? then add assset short cut
                        deselectAsset();
                    }
                }

                // on ">"
                if ( focused && focused == objects.select_asset  ){
                    if(  key == "#" ){
                        // clicked # enter ? then add assset short cut
                        doSelectAsset();
                    }
                }

                // on X
                if ( focused && focused == objects.de_select_asset ){
                    if(  key == "#" ){
                        // clicked # enter ? then add assset short cut
                        deselectAsset();
                    }
                }
            }
        }

        // non keyboard events
        if( key == "" ){

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

        }

    }


    
    //----------------------------------

    void doSelectAsset(){

        Serial.println("select: do select");              
        if( selectedButton != NULL ){

            // get selected asset
            assetClass* asset = (assetClass*) lv_obj_get_user_data(selectedButton);
            if (!asset) return;
            
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
                createDialog("Error: The asset selected do have a common inspection type with selected asset!");
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

        domainManagerClass* domain = domainManagerClass::getInstance();        

        domain->currentInspection.clear();
    
        // clean
        lv_obj_clean(objects.asset_list); 
        lv_obj_clean(objects.selected_asset_list); 

        // add assets to select
        // load all assets
        for (const assetClass& asset : *(domain->getAssets()) ) {
            addAssetToList(objects.asset_list, &asset, true);
            for (assetClass& selected : domain->currentInspection.assets) {
                if (asset.ID == selected.ID) {
                    addAssetToList(objects.selected_asset_list, &asset, false);
                    break;
                }
            }
        }


        {
            //-------------------------------------
            // Add focusable widgets

            // default
            lv_group_add_obj(inputGroup, objects.asset_list  );
            lv_group_add_obj(inputGroup, objects.select_asset);
            lv_group_add_obj(inputGroup, objects.de_select_asset);

            // nav bar 
            lv_group_add_obj(inputGroup, objects.do_select_inspection_type);            
            lv_group_add_obj(inputGroup, objects.back_from_select_asset);            

        }
        
        screenClass::init(); // always last, only if no issues
    }

    void start() override{

    }

    void stop() override{

        domainManagerClass* domain = domainManagerClass::getInstance();    
        domain->currentInspection.assets.clear();
        domain->currentInspection.driver_name = domain->loggedUser.name;
        domain->currentInspection.driver_username = domain->loggedUser.username;

        // Count selected asset buttons
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

};
