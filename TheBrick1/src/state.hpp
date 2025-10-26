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




//-------------------------------------------------------------

//                 S T A T E    M A N A G E R

//-------------------------------------------------------------



class stateManagerClass{
public:

  

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

    Serial.print("*** Screen event: ");
    lv_obj_t *target = lv_event_get_target(e);
    if (lv_obj_check_type( target, &lv_btn_class)) {
        lv_obj_t *label = lv_obj_get_child( target, 0);
        if (label && lv_obj_check_type(label, &lv_label_class)) {
            const char *text = lv_label_get_text(label);
            Serial.println(text);
        }
    }    

    dispatchEventsToWindows( e , "" );

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


      dispatchEventsToWindows( nullptr , key );

    }catch( const std::runtime_error& error ){
      Serial.println( "*** ERROR while handling keyboard event ***" );                    
      Serial.println( error.what() );                    
    }  
  }

  //********************************* */

  void dispatchEventsToWindows( lv_event_t* e, String key ){        
    
      try{

        if( currentScreenState != NULL ){ 
          currentScreenState->handleEvents( e, key );          
        }

      }catch( const std::runtime_error& error ){
        Serial.println( "*** ERROR while handling event ***" );                    
        Serial.println( error.what() );                    
      }      

  }

  //********************************* */  

  // // SCREEN TO SCREEN NAVIGATION
  // void openScreen( screenClass* screen ){        
  //   try{

  //     // ok well see if it opens
  //     screen->open();         

  //     // ok replace and go
  //     delete currentScreenState;
  //     currentScreenState = screen;

  //   }catch( const std::runtime_error& error ){
  //       // no delete 
  //       Serial.println( "*** window closed but not recycled ***" );   
  //       Serial.println( error.what() );            
  //       createDialog( error.what() );  
  //   }
  // }


  //=--------------------------------------------------------------------------------------------------

  //=--------------------------------------------------------------------------------------------------

  //=--------------------------------------------------------------------------------------------------


  screenClass* screenStates[8] = {nullptr}; // index 1..7
  screenClass* currentScreenState = nullptr;


  static int setOrGetPendingScreenId(int value = -1) {
      static int pendingScreenId = 0;
      if (value != -1) pendingScreenId = value;
      return pendingScreenId;
  }


  screenClass* getOrCreateScreen(int id) {
      if (id < 1 || id > 7)
          throw std::runtime_error("Invalid screen ID");
      if (!screenStates[id]) {
          switch (id) {
              // case SCREEN_ID_MAIN:                screenStates[id] = new mainScreenClass(); break;
              // case SCREEN_ID_SELECT_ASSET_SCREEN: screenStates[id] = new selectAssetScreenClass(); break;
              // case SCREEN_ID_SELECT_INSPECTION_TYPE: screenStates[id] = new selectInspectionTypeScreenClass(); break;
              // case SCREEN_ID_INSPECTION_FORM:     screenStates[id] = new inspectionFormScreenClass(); break;
              // case SCREEN_ID_INSPECTION_ZONES:    screenStates[id] = new inspectionZonesScreenClass(); break;
              case SCREEN_ID_LOGIN_SCREEN:        screenStates[id] = new loginScreenClass(); break;
              case SCREEN_ID_SETTINGS:            screenStates[id] = new settingsScreenClass(); break;
              default:
                  throw std::runtime_error("Unknown screen ID in getOrCreateScreen()");
          }
          if (!screenStates[id])
              throw std::runtime_error("Failed to create screen instance for ID: " );
      }
      return screenStates[id];
  }


  // 4. Transition logic: Reuse cached state object, don’t recreate!
  void processPendingScreenTransition() {
      int nextScreen = setOrGetPendingScreenId();
      if (nextScreen == 0) return;
      setOrGetPendingScreenId(0);

      try {
          screenClass* oldScreenState = currentScreenState;
          lv_obj_t* oldRoot = lv_scr_act();

          screenClass* nextScreenState = getOrCreateScreen(nextScreen);
          if (!nextScreenState) throw std::runtime_error("Unknown screen ID");

          // Set new current state *before* load
          currentScreenState = nextScreenState;

          // Load the new screen
          loadScreen((ScreensEnum)nextScreen);

          // ---- Wait for the new screen to be active ----
          int waitCount = 0;
          while (lv_scr_act() == oldRoot && waitCount++ < 50) { // Optional timeout
              Serial.println("New screen not active.  tick..");
              delayBlink();
              lv_timer_handler();
              ui_tick();
          }

          if (lv_scr_act() == oldRoot)
              throw std::runtime_error("Timeout: LVGL did not swap root!");

          // Now safe to initialize: set keyboard, focus groups, etc
          currentScreenState->init();

      } catch (const std::runtime_error& error) {
          Serial.println("*** Screen transition error ***");
          Serial.println(error.what());
          throw;
      }
  }



}; // class


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



