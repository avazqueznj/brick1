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


                
class inspectionZonesScreenClass : public screenClass {
public:


    assetClass* lastSelectedAsset = nullptr; // its the current asset not last!!

    inspectionZonesScreenClass( settingsClass* settings ): screenClass( settings, SCREEN_ID_INSPECTION_ZONES ){    
    }

    //-------------------------------------------------

    void clockTic( String time ) override {
        lv_label_set_text( objects.clock_zones, time.c_str());
        lv_label_set_text(  objects.driver_name_zones, domainManagerClass::getInstance()->loggedUser.name.c_str()  );        
    }    


    void rfidEvent(byte *uid, byte length) override {

        // Build tag string in your style
        String rfidTag = ":";
        for (byte i = 0; i < length; i++) {
            rfidTag += ":";
            rfidTag += String(uid[i]);
        }

        Serial.print("Tag string for inspection zone [");
        Serial.print(rfidTag);
        Serial.print("]");

        // Match to the 4 known zone tags → map to zone tag
        // are all zone tags the same ? idk
        String targetZoneTag;

        if (rfidTag == "::4:98:28:2:177:115:128") {  //conti
            targetZoneTag = "1";
        }else
        
        if (rfidTag == "::233:112:67:194") { // kfob
            targetZoneTag = "5";
        } else 
        
        
        
        {
            showDialog("Unknown tag");
            return;
        }

        Serial.print("Matched to zone tag: ");
        Serial.println(targetZoneTag);

        // Find zone button in UI and select it
        uint32_t zone_count = lv_obj_get_child_cnt(objects.zone_list);
        lv_obj_t* matchingZoneButton = nullptr;

        for (uint32_t i = 0; i < zone_count; ++i) {
            lv_obj_t* zbtn = lv_obj_get_child(objects.zone_list, i);
            layoutZoneClass* zone = static_cast<layoutZoneClass*>(lv_obj_get_user_data(zbtn));
            if (!zone) continue;

            if (zone->tag == targetZoneTag) {
                lv_obj_add_state(zbtn, LV_STATE_CHECKED);
                matchingZoneButton = zbtn;   // Save the match
            } else {
                lv_obj_clear_state(zbtn, LV_STATE_CHECKED);
            }
        }

        if (!matchingZoneButton) {
            showDialog("Read zone tag, but zone was not found or no asset selected.");
            return;
        }

        Serial.println("Zone selected by RFID OK.");

        // render
        renderComponents();
        refreshZoneAndComponentFlags();                        

        lv_group_focus_obj(objects.zone_component_list);             
    }


    //----------------


    void handleModalkeyboardEvent( String key ){

        lv_obj_t* list = objects.defect_dialog_list;
        lv_obj_t* selected = nullptr;

        // find current selection in the defect list
        uint32_t count = lv_obj_get_child_cnt(list);
        if( count == 0 ) return;
        for (uint32_t i = 0; i < count; ++i) {
            lv_obj_t* btn = lv_obj_get_child(list, i);
            if (lv_obj_has_state(btn, LV_STATE_CHECKED)) {
                selected = btn;
                break;
            }        
        }

        // nothing then default to first
        if(!selected) {
            lv_obj_t* selected = lv_obj_get_child(list, 0);
            if (selected) {
                lv_obj_add_state(selected, LV_STATE_CHECKED);
                lv_obj_scroll_to_view(selected, LV_ANIM_ON);
            }    
        }    

        if( !selected ){
            return;
        }


        // scroll defects
        if (key == "A" || key == "B") {            
            lv_obj_t* next = nullptr;
            if (key == "A") {
                next = get_prev_sibling(selected);
            } else if (key == "B") {
                next = get_next_sibling(selected);
            }
            if (next) {
                lv_obj_clear_state(selected, LV_STATE_CHECKED);
                lv_obj_add_state(next, LV_STATE_CHECKED);
                lv_obj_scroll_to_view(next, LV_ANIM_ON);
            }

               return;  
        }


        if( key == "1" ){
            saveDefect( -1 );
            refreshZoneAndComponentFlags();
            return;  
        }
        if( key == "2" ){
            saveDefect( 1 );
            refreshZoneAndComponentFlags();
            return;              
        }
        if( key == "3" ){
            saveDefect( 10 );
            refreshZoneAndComponentFlags();  
            return;                        
        }
        if( key == "4" ){
            closeDefectDialog();
            refreshZoneAndComponentFlags();
            return;                
        }

    }


    //----------------------------------

    void handleKeyboardEvent( String key ) override {                

        Serial.print("Inspection key event:");
        Serial.println(key);

        // are we under defecto modal? 
        if ( defectDialogOpen ) {
            Serial.print("Modal is up, handle modal.");
            handleModalkeyboardEvent( key );
            return; // simulate modal, eat events
        }     

        screenClass::handleKeyboardEvent(key);
        lv_obj_t* focused = lv_group_get_focused(inputGroup);

        // Navi
        if ( key == "#" and focused == objects.back_from_form_zones  ) {
            navigateTo( SCREEN_ID_INSPECTION_FORM );
            return;              
        }

        /*
            updateAssetSeverityLabels();
            updateZoneSeverityLabels();
            updateComponentSeverityLabels();
        */

        Serial.println("ABCD refresh?");                                    
        if (key == "A" || key == "B" || key == "C" || key == "D") {
            lv_obj_t* focused = lv_group_get_focused(inputGroup);     
            
            lv_textarea_set_text(objects.insp_component_instructions, "");                                

            if (focused == objects.zone_asset_list) {
                assetClass* asset = nullptr;

                uint32_t count = lv_obj_get_child_cnt(objects.zone_asset_list);
                for (uint32_t i = 0; i < count; ++i) {
                    lv_obj_t* btn = lv_obj_get_child(objects.zone_asset_list, i);
                    if (lv_obj_has_state(btn, LV_STATE_CHECKED)) {
                        asset = static_cast<assetClass*>(lv_obj_get_user_data(btn));
                        break;
                    }
                }
                if (asset) {
                    Serial.println("Refresh zones...");
                    lastSelectedAsset = asset;
                    renderAssetZones();
                    updateZoneSeverityLabels();
                }

            }
            else if (focused == objects.zone_list) {
                Serial.println("Refresh compos...");
                renderComponents();
                updateComponentSeverityLabels();
            }
            else if (focused == objects.zone_component_list) {

                // update instructions
                uint32_t child_count = lv_obj_get_child_cnt(objects.zone_component_list  ); 
                for (uint32_t i = 0; i < child_count; ++i) {

                    lv_obj_t* btn = lv_obj_get_child(objects.zone_component_list, i);
                    if (!lv_obj_check_type(btn, &lv_btn_class)) continue;
                    if (lv_obj_has_state(btn, LV_STATE_CHECKED)) {                    
                        // show instructions - get vector
                        const std::vector<String>* compVec = static_cast<const std::vector<String>*>(lv_obj_get_user_data(btn));
                        if (compVec) {
                            lv_textarea_set_text(objects.insp_component_instructions ,  (*compVec)[2].c_str() );                    
                        }
                    }
                }   
                
            }

            // else: do nothing
        }


        Serial.println("Defect buttons?");                                    
        
        if (key == "1") {
            Serial.println("allokDefectClick");                                                
            allokDefectClick();
            refreshZoneAndComponentFlags();
            return;              
        }
        if (key == "2") {
            Serial.println("okDefectClick");                                                
            okDefectClick();
            refreshZoneAndComponentFlags();
            return;              
        }
        if (key == "3") {
            Serial.println("defectClick");                                                
            defectClick();
            refreshZoneAndComponentFlags();
            return;              
        }
        
        if (
        (key == "4") ||
        ( key == "#" && focused == objects.submit  )
        ){
            submitInspection();
            return;              
        }
        
        if (
        (key == "5") ||
        ( key == "#" && focused == objects.save_insp  )
        ){
            saveInspection();
            return;              
        }
        


        Serial.println("DONE key handling");                                    
    }

    //--------------------------------

    void handleTouchEvent( lv_event_t* e ) override{
        
        lv_obj_t* target = lv_event_get_target(e);  // The object that triggered the event
        lv_obj_t* parent = lv_obj_get_parent(target);

        if ( target == objects.back_from_form_zones  ) {
            navigateTo( SCREEN_ID_INSPECTION_FORM );
            return;              
        }

        // =====================================================
        // CLICK ASSET ----
        if (  lv_obj_check_type(target, &lv_btn_class) &&  parent == objects.zone_asset_list ) {
            Serial.println("Asset clicked...");

            lv_textarea_set_text(objects.insp_component_instructions, "");      

            // go over the asset list
            bool render  = false;
            uint32_t child_count = lv_obj_get_child_cnt(objects.zone_asset_list  ); // ZONE assetrs list
            for (uint32_t i = 0; i < child_count; ++i) {

                // next child
                lv_obj_t* btn = lv_obj_get_child(objects.zone_asset_list, i);
                if (!lv_obj_check_type(btn, &lv_btn_class)) continue;

                // clear not selected
                if (btn != target) {                    
                    lv_obj_clear_state(btn, LV_STATE_CHECKED);

                } else {
                    // set selected!
                    lv_obj_add_state(btn, LV_STATE_CHECKED);

                    // get the domain asset in the asset button
                    assetClass* asset = static_cast<assetClass*>(lv_obj_get_user_data(btn));
                    if (!asset) {
                        throw std::runtime_error("inspectionZonesScreenClass: asset in button is null ?");
                    }

                    Serial.println( (*asset).ID );

                    // has the asset changed from last ?
                    // if (asset == lastSelectedAsset) {
                    //     return; // nothing to do                        
                    // }else{
                        // yes update and render
                        lastSelectedAsset = asset;  // new selection   
                        render  = true;
                    
                }                        
            }

            if( render ){
                renderAssetZones();
                refreshZoneAndComponentFlags();                
            }

            return;
        }     


        // On ZONE click -->
        if (  lv_obj_check_type(target, &lv_btn_class) &&  parent == objects.zone_list ) {
            Serial.println("Zone clicked...");

            lv_textarea_set_text(objects.insp_component_instructions, "");      

            // go over zones
            uint32_t child_count = lv_obj_get_child_cnt(objects.zone_list  ); // ZONE assetrs list
            if( child_count == 0 ) return;

            for (uint32_t i = 0; i < child_count; ++i) {
                // reset selection
                lv_obj_t* btn = lv_obj_get_child(objects.zone_list, i);
                if (!lv_obj_check_type(btn, &lv_btn_class)) continue;
                if (btn != target) {
                    lv_obj_clear_state(btn, LV_STATE_CHECKED);
                } else {
                    lv_obj_add_state(btn, LV_STATE_CHECKED);
                } 
            }           

            renderComponents();
            refreshZoneAndComponentFlags();                        
            return;
        }     

        // On COMPONENT click reset check -->
        if (  lv_obj_check_type(target, &lv_btn_class) &&  parent == objects.zone_component_list ) {
            Serial.println("compo click");            

            lv_textarea_set_text(objects.insp_component_instructions, "");                                

            uint32_t child_count = lv_obj_get_child_cnt(objects.zone_component_list  ); 
            for (uint32_t i = 0; i < child_count; ++i) {
                // reset selection
                lv_obj_t* btn = lv_obj_get_child(objects.zone_component_list, i);
                if (!lv_obj_check_type(btn, &lv_btn_class)) continue;

                if (btn != target) {
                    lv_obj_clear_state(btn, LV_STATE_CHECKED);
                    
                } else {
                    lv_obj_add_state(btn, LV_STATE_CHECKED);

                    // show instructions - get vector
                    const std::vector<String>* compVec = static_cast<const std::vector<String>*>(lv_obj_get_user_data(btn));
                    if (compVec) {
                        lv_textarea_set_text(objects.insp_component_instructions ,  (*compVec)[2].c_str() );                    
                    }
                }
            }   

            Serial.println("compo click DONE");
            return;
        }     


        //  top bar---------------------------------------------------------------------------------------------------

        // save 0 sev 
        if (target == objects.comp_ok_button) {
            okDefectClick();
            refreshZoneAndComponentFlags();
            return;
        }


        if (target == objects.all_ok_button) {  
            allokDefectClick();
            refreshZoneAndComponentFlags();
            return;
        }

        if ( target == objects.submit  ) {
            submitInspection();
            return;              
        }

        if ( target == objects.save_insp  ) {
            saveInspection();
            return;              
        }


        // =====================================================
        // DEFECTO dialog ---

        // defect list scroll reset 
        if (  lv_obj_check_type(target, &lv_btn_class) &&  parent == objects.defect_dialog_list ) {
            Serial.println("defecto click");
            
            uint32_t child_count = lv_obj_get_child_cnt( objects.defect_dialog_list ); 
            for (uint32_t i = 0; i < child_count; ++i) {                
                lv_obj_t* btn = lv_obj_get_child( objects.defect_dialog_list , i);
                if (!lv_obj_check_type(btn, &lv_btn_class)) continue;

                if (btn != target) {
                    lv_obj_clear_state(btn, LV_STATE_CHECKED);
                } else {
                    lv_obj_add_state(btn, LV_STATE_CHECKED);
                }

            }
            Serial.println("defecto click DONE");
            return;
        }   


        // Buttons        

        if (target == objects.defect_button) {  
            defectClick();
            refreshZoneAndComponentFlags();
            return;
        }

        // close defecto dialog <-- just call close below

        if (  target == objects.defect_dialog_close_btn_v2 ) {
            closeDefectDialog();
            refreshZoneAndComponentFlags();
            return;                
        }


  
        if (  
                ( target == objects.defect_dialog_delete || target ==  objects.defect_dialog_minor || target ==  objects.defect_dialog_major )
            ){

            int severity = 0;
            if( target == objects.defect_dialog_major ) severity = 10;
            if( target == objects.defect_dialog_minor ) severity = 1;
            if( target == objects.defect_dialog_delete ) severity = -1;

            saveDefect( severity );
            refreshZoneAndComponentFlags();

            return;                
        }



    }



//==================================================================================================================================
//==================================================================================================================================
//==================================================================================================================================

    void okDefectClick(){

        Serial.println("OK defect");

        // Find selected component in zone_component_list (index version)
        std::vector<String>* compVec = nullptr;
        uint32_t i = 0;
        lv_obj_t* btn = lv_obj_get_child(objects.zone_component_list, i);
        while (btn) {
            if (lv_obj_has_state(btn, LV_STATE_CHECKED)) {
                compVec = (std::vector<String>*) lv_obj_get_user_data(btn);            
                break;
            }
            ++i;
            btn = lv_obj_get_child(objects.zone_component_list, i);                
        }

        if (compVec != nullptr) {

            // Validate caset
            if (!lastSelectedAsset) {
                showDialog("No asset selected.");
                return;
            }

            // zone
            lv_obj_t* selected_zone_item = get_checked_child(objects.zone_list);
            if (!selected_zone_item) {
                showDialog("No zone selected.");
                return;
            }

            // layout
            layoutZoneClass* selected_zone = static_cast<layoutZoneClass*>(lv_obj_get_user_data(selected_zone_item));
            if (!selected_zone) {
                showDialog("Failed to resolve selected zone.");
                return;
            }

            if (compVec->size() <= 1) {
                showDialog("Component vector is incomplete.");
                return;
            }

            // compo
            String compName = (*compVec)[1];
            if (compName.isEmpty()) {
                showDialog("Component name is empty.");
                return;
            }

            domainManagerClass* domain = domainManagerClass::getInstance();
            std::vector<defectClass>& defects = domain->currentInspection.defects;

            // Check if a defect already exists for this component
            bool exists = false;
            for (const auto& defect : defects) {
   
                if (
                    defect.asset.ID == (*lastSelectedAsset).ID &&
                    defect.zoneName == selected_zone->tag &&
                    defect.componentName == compName) {
                    exists = true;
                    break;
                }
            }

            if (exists) {
                Serial.println("Defect already exists — ignoring OK post.");
                return; // Skip adding severity 0 if any defect exists
            }

            // Make severity 0 defect
            defectClass newDefect(
                *lastSelectedAsset,
                selected_zone->tag,
                compName,
                "GOOD",
                0,
                "",
                lv_label_get_text( objects.clock_zones )
            );

            defects.push_back(newDefect);

            Serial.println("OK defect saved!");
            Serial.println(domain->currentInspection.toEDI().c_str());

        } else {
            Serial.println("No component selected!");
            showDialog("Please select a component.");
        }

    }


    void allokDefectClick(){

            Serial.println("OK ALL defects for zone");

            if (!lastSelectedAsset) {
                showDialog("No asset selected.");
                return;
            }

            lv_obj_t* selected_zone_item = get_checked_child(objects.zone_list);
            if (!selected_zone_item) {
                showDialog("No zone selected.");
                return;
            }

            layoutZoneClass* selected_zone = static_cast<layoutZoneClass*>(lv_obj_get_user_data(selected_zone_item));
            if (!selected_zone) {
                showDialog("Failed to resolve selected zone.");
                return;
            }

            domainManagerClass* domain = domainManagerClass::getInstance();
            std::vector<defectClass>& defects = domain->currentInspection.defects;

            uint32_t comp_btn_count = lv_obj_get_child_cnt(objects.zone_component_list);
            uint32_t newDefectsCount = 0;

            for (uint32_t c = 0; c < comp_btn_count; ++c) {
                lv_obj_t* cbtn = lv_obj_get_child(objects.zone_component_list, c);
                std::vector<String>* uiCompVec = static_cast<std::vector<String>*>(lv_obj_get_user_data(cbtn));

                if (!uiCompVec || uiCompVec->size() <= 1) continue;

                String compName = (*uiCompVec)[1];
                if (compName.isEmpty()) continue;

                // Check if defect exists
                bool exists = false;
                for (const auto& defect : defects) {
                    if (
                        defect.asset.ID == (*lastSelectedAsset).ID &&
                        defect.zoneName == selected_zone->tag &&
                        defect.componentName == compName) {
                        exists = true;
                        break;
                    }
                }

                if (exists) {
                    Serial.print("Defect already exists for ");
                    Serial.println(compName);
                    continue; // Skip adding
                }

                // Add severity 0 defect
                defectClass newDefect(
                    (*lastSelectedAsset),
                    selected_zone->tag,
                    compName,
                    "GOOD",
                    0,
                    "",
                    lv_label_get_text( objects.clock_zones )
                );
                defects.push_back(newDefect);
                ++newDefectsCount;

                Serial.print("OK defect saved for ");
                Serial.println(compName);
            }

            if (newDefectsCount == 0) {
                Serial.println("No new OK defects needed — all already have defects.");
            } else {
                Serial.print("Added OK defects for ");
                Serial.print(newDefectsCount);
                Serial.println(" components.");
            }

            Serial.println(domain->currentInspection.toEDI().c_str());
    }

    void defectClick(){

        Serial.println("create defect click");

        // Find selected component in zone_component_list (index version)
        std::vector<String>* compVec = nullptr;
        uint32_t i = 0;
        lv_obj_t* btn = lv_obj_get_child(objects.zone_component_list, i);
        while (btn) {
            if (lv_obj_has_state(btn, LV_STATE_CHECKED)) {
                compVec = (std::vector<String>*) lv_obj_get_user_data(btn);            
                break;
            }
            ++i;
            btn = lv_obj_get_child(objects.zone_component_list, i);                
        }

        if (compVec != nullptr) {
            openDefectDialog( compVec );
        }else{
            Serial.println("No component selected!");
            showDialog("Please select a component.");
        }
    }



    void saveDefect( int severity ){

        Serial.println("Defect post...");

        domainManagerClass* domain = domainManagerClass::getInstance();

        // get selection
        lv_obj_t* checked_btn = get_checked_child(objects.defect_dialog_list);
        if (!checked_btn) {
            throw std::runtime_error("No defect selected in dialog: checked_btn is null!");
        }

        String* selected_defect = static_cast<String*>(lv_obj_get_user_data(checked_btn));
        if (!selected_defect) {
            throw std::runtime_error("Selected defect button has null user data!");
        }

        if (selected_defect->isEmpty()) {
            throw std::runtime_error("Selected defect string is empty!");
        }

        String notes = "";
        if( severity > 0  ){
            notes = lv_textarea_get_text( objects.defect_dialog_notes );
        }

        // assemble a tentative defect
        if( selected_defect !=  nullptr ){        
            defectClass newDefect(
                *selected_asset,
                selected_zone->tag,
                selected_component_name,
                selected_defect ? *selected_defect : String(""),
                severity,
                notes,
                lv_label_get_text( objects.clock_zones )
            );

            // is there a sibling already there - delete it
            std::vector<defectClass>& defects = domain->currentInspection.defects;
            for (size_t i = 0; i < defects.size(); ) {
                if (defects[i].isSameComponent(newDefect)) {
                    defects.erase(defects.begin() + i);
                } else {
                    ++i;
                }
            }

            // save it, or delete it
            if( severity != -1 ) domain->currentInspection.defects.push_back( newDefect );                

            closeDefectDialog();

            // debugo
            Serial.println( domain->currentInspection.toEDI().c_str() );                    
        }

    }



//==================================================================================================================================
//==================================================================================================================================
//==================================================================================================================================


    void renderAssetZones(){

        Serial.println("Render asset zones...");
            
        domainManagerClass* domain = domainManagerClass::getInstance();

        // reset zones and compos
        lv_obj_clean(objects.zone_list);
        lv_obj_clean(objects.zone_component_list);

        // Find the layout for the asset
        const layoutClass* layout = nullptr;
        for ( const layoutClass& l : *(domain->getLayouts()) ) {
            if (l.name == lastSelectedAsset->layoutName) {
                layout = &l;
                break;
            }
        }
        if (!layout) {
            throw std::runtime_error("inspectionZonesScreenClass: layout not found" );
        }

        //=================================================
        // ZONE RENDER

        // read the zones from the layout
        bool foundZone = false;
        for ( const  layoutZoneClass& zone : layout->zones) {
            foundZone = true;

            // Add zone button
            lv_obj_t* zbtn = lv_btn_create(objects.zone_list);
            lv_obj_set_size(zbtn, 230, 50);
            //lv_obj_add_flag(zbtn, LV_OBJ_FLAG_CHECKABLE);
            lv_obj_set_style_bg_color(zbtn, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(zbtn, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);

            lv_obj_add_event_cb(zbtn, action_main_event_dispatcher, LV_EVENT_PRESSED, this);

                lv_obj_t* zlabel = lv_label_create(zbtn);
                lv_obj_set_style_text_font(zlabel, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_label_set_text(zlabel, zone.name.c_str());
                lv_obj_set_user_data(zbtn, (void*)&zone);    
        }

        if (!foundZone) {
            throw std::runtime_error("inspectionZonesScreenClass: no zones found in layout: " );
        }
    
                
        Serial.println("Render asset zones...done!");
        return;
    }   

    void renderComponents(){  

        Serial.println("Render components ...");

        lv_obj_t* btn = NULL;

        // find the selected zone btn
        uint32_t child_count = lv_obj_get_child_cnt(objects.zone_list);
        if (child_count == 0) return;
        for (uint32_t i = 0; i < child_count; ++i) {
            lv_obj_t* b = lv_obj_get_child(objects.zone_list, i);
            if (!lv_obj_check_type(b, &lv_btn_class)) continue;

            if (lv_obj_has_state(b, LV_STATE_CHECKED)) {
                btn = b;
                break;
            }
        }    

        if(!btn) return;

        // clean compos
        lv_obj_clean(objects.zone_component_list);

        // get the zone
        layoutZoneClass* zone = static_cast<layoutZoneClass*>(lv_obj_get_user_data(btn));
        if (!zone) {
            throw std::runtime_error("Zone user_data is null in ZONE click handler");
        }
        if (!lastSelectedAsset) {
            throw std::runtime_error("lastSelectedAsset is null in ZONE click handler");
        }

        // COMPO RENDER ====================
        for (size_t j = 0; j < zone->components.size(); ++j) {

            const std::vector<String>& compVec = zone->components[j];
                        
            if (compVec.empty()) {
                throw std::runtime_error("Component vector is empty");
            }
            if (compVec.size() <= 3) {
                throw std::runtime_error("Component vector incomplete !>3");
            }

            String compName = compVec[1];
            String labelText = String( " " ) + compName;

            lv_obj_t* cbtn = lv_btn_create(objects.zone_component_list);
            lv_obj_set_size(cbtn, 280, 50);
            //lv_obj_add_flag(cbtn, LV_OBJ_FLAG_CHECKABLE);
            lv_obj_set_style_bg_color(cbtn, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(cbtn, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);

            lv_obj_set_user_data(cbtn, (void*)&compVec);
            lv_obj_add_event_cb(cbtn, action_main_event_dispatcher, LV_EVENT_PRESSED, this);

            lv_obj_t* clabel = lv_label_create(cbtn);
            lv_obj_set_style_text_font(clabel, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(clabel, labelText.c_str());
            
            
        }
    
        Serial.println("Render components ...done!");

        return;

    }  



//-----------------------------------------------------------


    void refreshZoneAndComponentFlags() {

        updateAssetSeverityLabels();
        updateZoneSeverityLabels();
        updateComponentSeverityLabels();
    }        

    void updateAssetSeverityLabels() {
        domainManagerClass* domain = domainManagerClass::getInstance();

        Serial.println("Refresh Asset flags:");

        uint32_t count = lv_obj_get_child_cnt(objects.zone_asset_list);
        for (uint32_t i = 0; i < count; ++i) {

            // get the asset button
            lv_obj_t* assetButton = lv_obj_get_child(objects.zone_asset_list, i);
            if (!assetButton) continue;

            // get the asset in the button
            assetClass* asset = static_cast<assetClass*>(lv_obj_get_user_data(assetButton));
            if (!asset) continue;

            Serial.print(asset->ID);
            Serial.print(" Defect?");

            // ---- Find the layout for this asset ----
            const layoutClass* layout = nullptr;
            for (const auto& l : *domain->getLayouts()) {
                if (l.name == asset->layoutName) {
                    layout = &l;
                    break;
                }
            }
            if (!layout) {
                // HARD FAIL: logic/config error
                throw std::runtime_error("Layout not found for asset: " );
            }

            bool allInspected = true;
            int maxSeverity = -1;

            // ---- Check every zone/component ----
            for (const layoutZoneClass& zone : layout->zones) {
                for (const auto& componentRow : zone.components) {

                    if (componentRow.size() < 2) {
                        throw std::runtime_error("Malformed component definition in config: expected at least key and label.");
                    }

                    String componentLabel = componentRow[1];  //<<<<<

                    bool found = false;
                    // Check if there is ANY defect entry (including "good") for this component
                    for (const defectClass& defect : domain->currentInspection.defects) {

                        if (defect.asset.ID == asset->ID &&
                            defect.zoneName == zone.tag &&
                            defect.componentName == componentLabel) 
                        {
                            found = true;
                            if (defect.severity > maxSeverity) maxSeverity = defect.severity;
                            break;
                        }
                    }       
                    if (!found) {
                        allInspected = false;
                        break;
                    }
                }
                if (!allInspected) break;
            }

            // ---- Build the flag prefix ----
            String prefix;
            if (!allInspected) {
                prefix = ""; // No flag if not all components inspected
            } else if (maxSeverity == 10) {
                prefix = String(LV_SYMBOL_CLOSE) + " ";
            } else if (maxSeverity == 1) {
                prefix = String(LV_SYMBOL_WARNING) + " ";
            } else if (maxSeverity == 0) {
                prefix = String(LV_SYMBOL_OK) + " ";
            } else {
                prefix = "";
            }

            lv_obj_t* label = lv_obj_get_child(assetButton, 0);
            if (label) {
                String newText = prefix + asset->buttonName;
                lv_label_set_text(label, newText.c_str());

                Serial.print(" -> ");
                Serial.println(newText);
            }
        }
    }

    void updateZoneSeverityLabels() {

        Serial.println("Refresh Zone flags:");

        domainManagerClass* domain = domainManagerClass::getInstance();
        if (!lastSelectedAsset) return;

        uint32_t count = lv_obj_get_child_cnt(objects.zone_list);
        for (uint32_t i = 0; i < count; ++i) {
            lv_obj_t* zoneButton = lv_obj_get_child(objects.zone_list, i);
            if (!zoneButton) continue;

            layoutZoneClass* zone = static_cast<layoutZoneClass*>(lv_obj_get_user_data(zoneButton));
            if (!zone) continue;

            Serial.print("Zone:");
            Serial.print(zone->name);
            Serial.print(" def:");

            // Get max severity for this zone
            int maxSeverity = -1;
            for (const defectClass& defect : domain->currentInspection.defects) {
                if (defect.asset.ID == lastSelectedAsset->ID && defect.zoneName == zone->tag) {
                    Serial.print(defect.severity);
                    Serial.print(" ");
                    if (defect.severity > maxSeverity) {
                        maxSeverity = defect.severity;
                    }
                }
            }

            String prefix;
            if (maxSeverity == 10) prefix = String(LV_SYMBOL_CLOSE) + " ";
            else if (maxSeverity == 1) prefix = String(LV_SYMBOL_WARNING) + " ";
            else if (maxSeverity == 0) prefix = String(LV_SYMBOL_OK) + " ";
            else prefix = "";

            // Update label text
            lv_obj_t* label = lv_obj_get_child(zoneButton, 0); // Assuming first child is label
            if (label) {
                String newText = prefix + zone->name;
                lv_label_set_text(label, newText.c_str());
                Serial.print(" label:");
                Serial.println(newText);
            }
        }
    }

    void updateComponentSeverityLabels() {
        domainManagerClass* domain = domainManagerClass::getInstance();
        if (!lastSelectedAsset) return;

        Serial.println("Refresh Compo flags:");

        // --- Find the selected zone ---
        layoutZoneClass* selectedZone = nullptr;
        uint32_t zoneCount = lv_obj_get_child_cnt(objects.zone_list);
        for (uint32_t i = 0; i < zoneCount; ++i) {
            lv_obj_t* zoneButton = lv_obj_get_child(objects.zone_list, i);
            if (!zoneButton) continue;

            if (lv_obj_has_state(zoneButton, LV_STATE_CHECKED)) {
                selectedZone = static_cast<layoutZoneClass*>(lv_obj_get_user_data(zoneButton));
                break;
            }
        }
        if (!selectedZone) return; // no zone selected, nothing to update

        Serial.print("Zone:");
        Serial.println(selectedZone->tag);

        // --- Update all component labels for selected zone ---
        uint32_t count = lv_obj_get_child_cnt(objects.zone_component_list);
        for (uint32_t i = 0; i < count; ++i) {
            lv_obj_t* compButton = lv_obj_get_child(objects.zone_component_list, i);
            if (!compButton) continue;

            // get the name form the comp vec
            std::vector<String>* compVec = static_cast<std::vector<String>*>(lv_obj_get_user_data(compButton));
            if (!compVec || compVec->size() < 2) continue;
            String componentName = (*compVec)[1];

            Serial.print(" Comp:");
            Serial.print(componentName);
            Serial.print(" ");                                    

            int maxSeverity = -1;
            for (const defectClass& defect : domain->currentInspection.defects) {            

                if (defect.asset.ID == lastSelectedAsset->ID &&
                    defect.zoneName == selectedZone->tag &&
                    defect.componentName == componentName) 
                    {

                        if (defect.severity > maxSeverity) maxSeverity = defect.severity;
                        Serial.print(defect.severity);
                        Serial.print(" ");                        
                    }

            }

            String prefix;
            if (maxSeverity == 10) prefix = String(LV_SYMBOL_CLOSE) + " ";
            else if (maxSeverity == 1) prefix = String(LV_SYMBOL_WARNING) + " ";
            else if (maxSeverity == 0) prefix = String(LV_SYMBOL_OK) + " ";
            else prefix = "";

            lv_obj_t* label = lv_obj_get_child(compButton, 0);
            if (label) {
                String newText = prefix + componentName;
                lv_label_set_text(label, newText.c_str());

                Serial.print("  -> Label:");
                Serial.println(newText);
            }
        }
    }    




//==================================================================================================================================
//==================================================================================================================================
//==================================================================================================================================



    void init() override {

        { // key nav

            // default
            lv_group_add_obj(inputGroup, objects.zone_asset_list  );
            lv_group_add_obj(inputGroup, objects.zone_list  );            
            lv_group_add_obj(inputGroup, objects.zone_component_list  );            

            // too many items in the group, lets only do number shortcuts
            //lv_group_add_obj(inputGroup, objects.all_ok_button  );
            //lv_group_add_obj(inputGroup, objects.comp_ok_button  );            
            //lv_group_add_obj(inputGroup, objects.defect_button  );            

            // nav bar -- also too complicated, use numbers this will never work
            lv_group_add_obj(inputGroup, objects.back_from_form_zones );            
            //lv_group_add_obj(inputGroup, objects.submit);       
        }

        screenClass::init();

        lv_obj_t* close_btn = lv_msgbox_get_close_btn(objects.defect_dialog);
        if (close_btn){
            lv_obj_del(close_btn);         
        } 
        lv_obj_add_flag(  objects.defect_dialog, LV_OBJ_FLAG_HIDDEN);     
        lv_obj_add_flag(  objects.inspection_zones_overlay, LV_OBJ_FLAG_HIDDEN);     

        makeKeyboards();
        addLetterKeyboard( objects.defect_dialog_notes );
        
    }

    void start() override {

        domainManagerClass* domain = domainManagerClass::getInstance();
        
        // Clear existing items
        lv_obj_clean(objects.zone_asset_list);
        lv_obj_clean(objects.zone_list);
        lv_obj_clean(objects.zone_component_list);

        for (assetClass& asset : domain->currentInspection.assets) {

            lv_obj_t* btn = lv_btn_create(objects.zone_asset_list);

            lv_obj_set_size(btn, 280, 50);
            lv_obj_add_event_cb(btn, action_main_event_dispatcher, LV_EVENT_PRESSED, this);
            //lv_obj_add_flag(btn, LV_OBJ_FLAG_CHECKABLE);
            lv_obj_set_user_data(btn, static_cast<void*>(&asset));

            // Style
            lv_obj_set_style_bg_color(btn, lv_color_hex(0xff2196f3), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(btn, lv_color_hex(0xffffffff), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_layout(btn, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_flex_track_place(btn, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_bottom(btn, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_top(btn, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_left(btn, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_pad_right(btn, 4, LV_PART_MAIN | LV_STATE_DEFAULT);

            // Label
            lv_obj_t* label = lv_label_create(btn);
            lv_obj_set_style_align(label, LV_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_font(label, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(label, asset.buttonName.c_str() );

        }

        // activate the first asset
        uint32_t count = lv_obj_get_child_cnt(objects.zone_asset_list);
        if (count > 0) {
            lv_obj_t* first_asset_btn = lv_obj_get_child(objects.zone_asset_list, 0);
            if (lv_obj_check_type(first_asset_btn, &lv_btn_class)) {
                Serial.println("Activate first asset in list.");
                lv_obj_add_state(first_asset_btn, LV_STATE_CHECKED);
                assetClass* asset = static_cast<assetClass*>(lv_obj_get_user_data(first_asset_btn));
                if (!asset) {
                    throw std::runtime_error("inspectionZonesScreenClass: asset in button is null ?");
                }else{
                    lastSelectedAsset = asset;  // new selection   
                    renderAssetZones();
                    refreshZoneAndComponentFlags();                
                }
            }
        }

    }

    void stop() override{
        // Placeholder if needed later
    }

    virtual ~inspectionZonesScreenClass() {
        domainManagerClass* domain = domainManagerClass::getInstance();
        domain->currentInspection.defects.clear();
    }


    //---------------------------------------------

    bool defectDialogOpen = false;
    void closeDefectDialog(){
        Serial.println("Close defect dialog ...");                
        if( defectDialogOpen ){
            lv_obj_add_flag(  objects.defect_dialog, LV_OBJ_FLAG_HIDDEN);   
            lv_obj_add_flag(  objects.inspection_zones_overlay, LV_OBJ_FLAG_HIDDEN);   
            lv_textarea_set_text( objects.defect_dialog_notes, "" );         
        }
        defectDialogOpen = false;
    }

        
    //---------------------------------------------    

    assetClass* selected_asset = nullptr;
    layoutZoneClass* selected_zone = nullptr;
    std::vector<String>* selected_component_vec = nullptr;
    String selected_component_name;    
    void openDefectDialog( std::vector<String>* compVec ){


        //=======================
        // while i fix the unselect issue

            lv_obj_t* selected_zone_item = nullptr;
            lv_obj_t* selected_component_item = nullptr;

                // Asset selection check
                lv_obj_t* selected_asset_item = get_checked_child(objects.zone_asset_list);
                if (!selected_asset_item) {
                    showDialog("Please select an asset.");
                    return;
                }
                selected_asset = static_cast<assetClass*>(lv_obj_get_user_data(selected_asset_item));
                if (!selected_asset) {
                    showDialog("Failed to resolve selected asset.");
                    return;
                }

                // Zone selection check
                selected_zone_item = get_checked_child(objects.zone_list);
                if (!selected_zone_item) {
                    showDialog("Please select a zone.");
                    return;
                }
                selected_zone = static_cast<layoutZoneClass*>(lv_obj_get_user_data(selected_zone_item));
                if (!selected_zone) {
                    showDialog("Failed to resolve selected zone.");
                    return;
                }

                // Component selection check
                selected_component_item = get_checked_child(objects.zone_component_list);
                if (!selected_component_item) {
                    showDialog("Please select a component.");
                    return;
                }
                selected_component_vec = static_cast<std::vector<String>*>(lv_obj_get_user_data(selected_component_item));
                if (!selected_component_vec) {
                    showDialog("Failed to resolve selected component.");
                    return;
                }
                if (selected_component_vec->size() <= 1) {
                    showDialog("Selected component data is incomplete.");
                    return;
                }
                selected_component_name = (*selected_component_vec)[1];
                if (selected_component_name.isEmpty()) {
                    showDialog("Selected component name is empty.");
                    return;
                }

        //===================

        domainManagerClass* domain = domainManagerClass::getInstance();
        defectClass* existingDefect = nullptr;

        // restore if this is an edit ....
        for (auto& d : domain->currentInspection.defects) {
            if (d.asset.ID == (*selected_asset).ID && d.zoneName == selected_zone->tag && d.componentName == selected_component_name) {
                existingDefect = &d;
                break;
            }
        }        
    
        // defect dialog ==========
        if (compVec->size() >= 4) {
            String compName = (*compVec)[1];
            Serial.print("Selected component: ");
            Serial.println(compName);

            // Defective component label            
            lv_obj_set_style_text_font(objects.defect_dialog_title, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_label_set_text(objects.defect_dialog_title, compName.c_str());

            // Add defect buttons
            lv_obj_clean(objects.defect_dialog_list);
            for (size_t i = 3; i < compVec->size(); ++i) {
                String defectName = (*compVec)[i];

                lv_obj_t* defect_btn = lv_btn_create(objects.defect_dialog_list);
                lv_obj_set_size(defect_btn, 230, 50);

                lv_obj_set_user_data(defect_btn, (void*)&(*compVec)[i] );
                
                lv_obj_add_event_cb(defect_btn, action_main_event_dispatcher, LV_EVENT_PRESSED, (void*)&(*compVec)[i]);

                lv_obj_set_style_bg_color(defect_btn, lv_color_hex(0xffdddddd), LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_text_color(defect_btn, lv_color_hex(0xff000000), LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_layout(defect_btn, LV_LAYOUT_FLEX, LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_obj_set_style_flex_track_place(defect_btn, LV_FLEX_ALIGN_CENTER, LV_PART_MAIN | LV_STATE_DEFAULT);

                //lv_obj_add_flag(defect_btn, LV_OBJ_FLAG_CHECKABLE);

                lv_obj_t* label = lv_label_create(defect_btn);
                lv_obj_set_style_text_font(label, &lv_font_montserrat_28, LV_PART_MAIN | LV_STATE_DEFAULT);
                lv_label_set_text(label, defectName.c_str());

                // restore
                if (existingDefect && defectName == existingDefect->defectType) {
                    lv_obj_add_state(defect_btn, LV_STATE_CHECKED);
                }
                // or default
                else if (i == 3 && !existingDefect) {                     
                    lv_obj_add_state(defect_btn, LV_STATE_CHECKED);
                }
                // if the defect is good defect also choose 2
                else if (i == 3 && existingDefect) {
                    if( existingDefect->defectType == "GOOD" ){ 
                        lv_obj_add_state(defect_btn, LV_STATE_CHECKED);
                    }
                }

                if( existingDefect ){
                    lv_textarea_set_text( objects.defect_dialog_notes, existingDefect->notes.c_str() );
                }else{
                    lv_textarea_set_text( objects.defect_dialog_notes, "" );
                }
            }
        }

        lv_obj_clear_flag(  objects.defect_dialog, LV_OBJ_FLAG_HIDDEN);     
        lv_obj_clear_flag(  objects.inspection_zones_overlay, LV_OBJ_FLAG_HIDDEN);     
        defectDialogOpen = true;       

        Serial.println("defect click done!");

    }    

//==============================================

    bool isInspectionComplete() {
        domainManagerClass* domain = domainManagerClass::getInstance();
        uint32_t count = lv_obj_get_child_cnt(objects.zone_asset_list);
        for (uint32_t i = 0; i < count; ++i) {
            lv_obj_t* assetButton = lv_obj_get_child(objects.zone_asset_list, i);
            if (!assetButton) continue;
            assetClass* asset = static_cast<assetClass*>(lv_obj_get_user_data(assetButton));
            if (!asset) continue;

            // Find layout for this asset
            const layoutClass* layout = nullptr;
            for (const auto& l : *domain->getLayouts()) {
                if (l.name == asset->layoutName) {
                    layout = &l;
                    break;
                }
            }
            if (!layout) throw std::runtime_error("Layout not found for asset.");

            // For every component, check if there's a defect entry (any severity)
            for (const layoutZoneClass& zone : layout->zones) {
                for (const auto& componentRow : zone.components) {
                    if (componentRow.size() < 2)
                        throw std::runtime_error("Malformed component definition in config: expected at least key and label.");
                    String componentLabel = componentRow[1];

                    bool found = false;
                    for (const defectClass& defect : domain->currentInspection.defects) {
                        if (defect.asset.ID == asset->ID &&
                            defect.zoneName == zone.tag &&
                            defect.componentName == componentLabel)
                        {
                            found = true;
                            break;
                        }
                    }
                    if (!found)
                        return false; // As soon as one missing, inspection incomplete
                }
            }
        }
        return true; // All components for all assets are inspected
    }

    void submitInspection() {
        
        if (!isInspectionComplete()) {
            static const char* btns[] = { "Ok", "Cancel", "" };
            showDialog( "Submit <<incomplete>> inspection?", "Submit", btns );
            return;
        }
        
        static const char* btns[] = { "Ok", "Cancel", "" };
        showDialog( "Submit inspection?", "Submit", btns );
        return;
    }

    void doSubmitInspection(){

        Serial.println("Submit ...");
        spinnerStart();

        domainManagerClass* domain = domainManagerClass::getInstance(); 

        try{            
            // guard            
            if (domain->currentInspection.defects.size() == 0) {
                spinnerEnd(); 
                Serial.println("ERROR: Cannot submit empty inspection.");
                showDialog("ERROR: Cannot submit empty inspection.");
                return;
            } 

            // for the record                        
            domain->currentInspection.submitTime = String(lv_label_get_text(objects.clock_zones));               
            Serial.println( domain->currentInspection.toEDI() );                                      

            String result = "<<TEST NO SUBMIT>>";                
            result =  domain->comms->POST( domain->serverURL, domain->postInspectionsPath + "?company=" + domain->company,  domain->currentInspection.toEDI() );

            domainManagerClass::getInstance()->currentInspection.submitted = true;
            domainManagerClass::getInstance()->currentInspection.serverReply = result;

            // save it
            String filingRecord = 
                domain->currentInspection.toEDI()  +
                domain->currentInspection.toHumanString() ;

            saveInspectionToDisk( filingRecord );

            Serial.println("Submit ... done!");
            spinnerEnd();      

            showDialog( "Submitted!" );

            navigateTo( SCREEN_ID_MAIN );
            
        }catch( const std::runtime_error& error ){

            domainManagerClass::getInstance()->currentInspection.serverReply =error.what();          

            // save it
            String filingRecord = 
                domain->currentInspection.toEDI()  +
                domain->currentInspection.toHumanString() ;
            saveInspectionToDisk( filingRecord );

            spinnerEnd();       
            String chainedError = String( "ERROR: Inspection saved, but possibly not sent:" ) + error.what();           
            showDialog( chainedError.c_str() );

            navigateTo( SCREEN_ID_MAIN );
        }
                       
    }

    void saveInspection() {
        
        if (!isInspectionComplete()) {
            static const char* btns[] = { "Ok", "Cancel", "" };
            showDialog( "Save <<incomplete>> inspection?", "Save", btns );
            return;
        }
        
        static const char* btns[] = { "Ok", "Cancel", "" };
        showDialog( "Save inspection?", "Save", btns );
        return;
    }

    void doSaveInspection(){

        Serial.println("Save ...");
        spinnerStart();

        try{

            domainManagerClass* domain = domainManagerClass::getInstance(); 

            // guard            
            if (domain->currentInspection.defects.size() == 0) {
                spinnerEnd(); 
                Serial.println("ERROR: Cannot save empty inspection.");
                showDialog("ERROR: Cannot save empty inspection.");
                return;
            } 

            domain->currentInspection.submitTime = String(lv_label_get_text(objects.clock_zones));               

            // save it
            String filingRecord = 
                domain->currentInspection.toEDI()  +
                domain->currentInspection.toHumanString() ;

            saveInspectionToDisk( filingRecord );

            Serial.println("Save ... done!");
            spinnerEnd();      

            showDialog( "Saved!" );

            navigateTo( SCREEN_ID_MAIN );
            
        }catch( const std::runtime_error& error ){
            spinnerEnd();       
            String chainedError = String( "ERROR: Could not SAVE: " ) + error.what();           
            showDialog( chainedError.c_str() );
        }
                       
    }


//==============================================    

    void saveInspectionToDisk(const String& edi) {

        // Slot selection logic as above (find empty or oldest based on parsed DISPLAYHEADER* timestamp)
        int oldestSlot = 1;  // the one to delete
        uint32_t oldestTime = UINT32_MAX;  //oldest to start

        // for each slot
        Serial.println( "SAVE: find slot...." );
        for (int i = 1; i <= NUM_INSPECTION_SLOTS; ++i) {

            String path = "/kv/insp" + String(i);
            uint32_t ts = 0;

            try {

                // parse the time stamp
                std::vector<String> file = loadFromKVStore(path);
                for (const String& line : file) {
                    if (line.startsWith("DISPLAYHEADER*")) {
                        int firstStar = line.indexOf('*');
                        int secondStar = line.indexOf('*', firstStar + 1);
                        if (firstStar >= 0 && secondStar > firstStar) {
                            String tsStr = line.substring(firstStar + 1, secondStar);
                            ts = (uint32_t)tsStr.toInt();
                        }
                        break;
                    }
                }

            } catch (...) {
                Serial.println( "ERROR!!!: Cannor parse slot ovewrite it!" );
                ts = 0;
            }

            // checking time stamp

            if (ts == 0) {
                oldestSlot = i;
                break;
            }

            if (ts < oldestTime) {
                oldestTime = ts;
                oldestSlot = i;
            }
        }


        String path = "/kv/insp" + String(oldestSlot);
        Serial.println( "SAVE: saving: " + path );
        saveToKVStore(path, edi);

        Serial.print("Saved inspection to slot ");
        Serial.println(oldestSlot);
    }

//==============================================    
   
    
    void modalDialogEvent(const String modalActionTouch, const String button) override {

        Serial.println( "Override Modal event " + modalActionTouch + ":" + button );

        if( modalActionTouch == "Submit" && button == "Ok" ){
            doSubmitInspection();
        }

        if( modalActionTouch == "Save" && button == "Ok" ){
            doSaveInspection();
        }

    }

    virtual void modalDialogKey( String key ){    
        screenClass::modalDialogKey( key );

        Serial.println( "Inspection modal key " + modalAction + ":" + key );

        if( modalAction == "Submit" && key == "#" ){
            doSubmitInspection();
        }

        if( modalAction == "Save" && key == "#" ){
            doSaveInspection();
        }

    }    

};


//----






