/********************************************************************************************
 * CONFIDENTIAL AND PROPRIETARY
 * 
 * The Brick 1.0 
 * © [2025] [Alejandro Vazquez]. All rights reserved.
 * 
 ********************************************************************************************/


#ifndef STATE_H
#define STATE_H

#include <Arduino.h>
#include "src/screens.h"
#include "src/actions.h"
#include <exception>
#include <WiFi.h>
#include "util.hpp"
#include <deque>
#include "src/ui.h"


class screenClass{
public:

    ScreensEnum screenId;    
    lv_group_t* inputGroup = nullptr;

    screenClass( ScreensEnum screenIdParam ): 
        screenId{screenIdParam}{
        inputGroup = lv_group_create();
    }

    virtual void open(){
        loadScreen( screenId );        
    };

    virtual bool modalActive(){
        return false;
    };

    virtual void handleScreenEvents( lv_event_t* e ){
    }

    virtual void rfidEvent( byte *uid, byte length ){
    }

    virtual void clockTic( String time ){
    }

    //---

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
                    lv_obj_update_layout(objects.form_fields);

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

        Serial.print( "Scroll" );                    

        // get target
        lv_obj_t* list = lv_group_get_focused(inputGroup);

        // is it a list ? 
        if (!list || !lv_obj_check_type(list, &lv_list_class) ) return;

        // is it empty ??
        uint32_t count = lv_obj_get_child_cnt(list);
        if( count == 0 ) return;

        // find current selection in the list
        Serial.print( "Sel?" );                            
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


    // base screen class key nav
    virtual void keyboardEvent(String key) {

        // get the focused thing
        lv_obj_t* focused = lv_group_get_focused(inputGroup);
        Serial.print( "*** *** Screen Base: Key Handler: " );            
        Serial.println( key );            

        // list scrolling
        Serial.print( "1" );            
        keyListScrolling( key );

        // tab navi add esc and enter
        Serial.print( "2" );            
        if (key == "C") {
            lv_group_focus_prev(inputGroup);
            checkTextAreaInView();
            return;

        } else if (key == "D") {
            lv_group_focus_next(inputGroup);
            checkTextAreaInView();
            return;

        } else if (key == "#") {
            if (focused) {
                lv_event_send(focused, LV_EVENT_PRESSED, NULL);  // only exception of artificial events

            }
            return;    
        } else if (key == "*") {
            lv_group_send_data(inputGroup, LV_KEY_ESC);
            return;
        }

        Serial.println( "3end" );                    
        // else we are done, child class could do something special to the screen
    }


    virtual ~screenClass(){
        if (inputGroup) {
            lv_group_del(inputGroup);
            Serial.println("mainScreenClass: Screen Class destroyed, inputGroup destroyed");
        }        
    }


};


//-------------------------------------------------------------


class stateManagerClass{
public:

  screenClass* currentScreenState = NULL;

  stateManagerClass( ){            
    currentScreenState =  NULL;  
  }    
          
  virtual ~stateManagerClass(){   
  }

  // BASE DISPATCH ----------------------------------------

   void rfidEvent( byte *uid, byte length ){
    try{
      if( currentScreenState !=  NULL ){
        Serial.println( "*** RFID event ***" );                    
        currentScreenState->rfidEvent( uid, length );
      }
    }catch( const std::runtime_error& error ){
      Serial.println( "*** ERROR while handling RFID event ***" );                    
      Serial.println( error.what() );                    
    }          
  }

  void clockTic( String time ){
    try{
      if( currentScreenState !=  NULL ){
        currentScreenState->clockTic(  time );
      }
    }catch( const std::runtime_error& error ){
      Serial.println( "*** ERROR while handling clock event ***" );                    
      Serial.println( error.what() );                    
    }         
  }  

  // handle touch events
  void handleScreenEvents( lv_event_t* e ){

      try{

        Serial.print("*** Screen event:");
        lv_obj_t *target = lv_event_get_target(e);  // The object that triggered the event

        // windows
        if( currentScreenState != NULL ){ 
          Serial.println("state: ? Forwarding ...");    
          currentScreenState->handleScreenEvents( e );          
        }

      }catch( const std::runtime_error& error ){
        Serial.println( "*** ERROR while handling event ***" );                    
        Serial.println( error.what() );                    
      }      
  }

  // key events -- ENTRY
  void keyboardEvent( String key ){
    try{

      Serial.print("*** Key event:");
      Serial.println(key);

      // are we in system modal ? ignore
      if ( overlay != nullptr ) {    
        Serial.println( "State: System modal" );
        // dismiss dialog
        if (key == "#" ) {    
          Serial.println("Keyboard # -> closing dialog");
          lv_obj_t * mbox = lv_obj_get_child(overlay, 0);  // If overlay only has the mbox.
          lv_msgbox_close(mbox);
          lv_obj_del(overlay);
        }
        return; // eat the events, simulate modal
      }


      // are we in window modal ? ignore
      if( currentScreenState !=  NULL  ){ 
        if( currentScreenState->modalActive() ) {
          Serial.println( "State: window modal" );
          return;
        }          
      }


      // no, well then pass the event to the current screen
      if( currentScreenState !=  NULL ){
        currentScreenState->keyboardEvent( key );
      }

    }catch( const std::runtime_error& error ){
      Serial.println( "*** ERROR while handling keyboard event ***" );                    
      Serial.println( error.what() );                    
    }  
  }

  // SCREEN NAVIGATION
  void openScreen( screenClass* screen ){        
    try{

      // ok well see if it opens
      screen->open();         

      // ok replace and go
      delete currentScreenState;
      currentScreenState = screen;

    }catch( const std::runtime_error& error ){
        // no delete 
        Serial.println( "*** window closed but not recycled ***" );   
        Serial.println( error.what() );            
        createDialog( error.what() );  
    }
  }



};


//------------------------------------------------------

extern stateManagerClass* stateManager;
#include "src/actions.h"
extern "C" void action_main_event_dispatcher(lv_event_t *e) {
  if ( (lv_event_get_code(e) != LV_EVENT_PRESSED) && (lv_event_get_code(e) != LV_EVENT_CLICKED)   )return;    
  
  if (stateManager != NULL) {
        stateManager->handleScreenEvents(e);
  }
}

//-------------------------------------------------




#endif

