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

#define settingsFileName "/kv/settings"

class stateManagerClass{
public:

  configClass settings;


  //-------------------------------------------------


  stateManagerClass(  ){            
    currentScreenState =  NULL;  
  }    



  void init(){
    try{      
      settings.load( settingsFileName );
    } catch (const std::exception& e) {
        String msg = String( "Could not load config, defaulting: " ) + e.what() ;
        Serial.println(msg); // optional
        resetSettingsFile();
    }
    applySettingsFile();
  }
     

  void updateSettingsFile() {

    Serial.println( "*** Config has been update !! ***" );

    // Domain
    settings["company"] = domainManagerClass::getInstance()->company;

    settings["timeZoneIndex"] = String(domainManagerClass::getInstance()->timeZoneIndex);
    settings["timeOffsetFromUTC"] = String(domainManagerClass::getInstance()->timeOffsetFromUTC);
    settings["DST"] = String( domainManagerClass::getInstance()->DST );

    settings["serverURL"] = domainManagerClass::getInstance()->serverURL;
    settings["getConfigPath"] = domainManagerClass::getInstance()->getConfigPath;
    settings["postInspectionsPath"] = domainManagerClass::getInstance()->postInspectionsPath;

    // Comms
    if(domainManagerClass::getInstance()->comms) {
        settings["wifi_ssid"] = domainManagerClass::getInstance()->comms->ssid;
        settings["wifi_password"] = domainManagerClass::getInstance()->comms->pass;
    }    

    settings.save( settingsFileName ); 
  }


  void applySettingsFile(){

    domainManagerClass::getInstance()->company  = settings[ "company" ] ;

    domainManagerClass::getInstance()->timeZoneIndex = settings[ "timeZoneIndex" ].toInt(); 
    domainManagerClass::getInstance()->timeOffsetFromUTC = settings[ "timeOffsetFromUTC" ].toInt();
    domainManagerClass::getInstance()->DST = settings[ "DST" ].toInt() ;

    domainManagerClass::getInstance()->serverURL = settings[ "serverURL" ] ;
    domainManagerClass::getInstance()->getConfigPath = settings[ "getConfigPath" ] ;
    domainManagerClass::getInstance()->postInspectionsPath = settings[ "postInspectionsPath" ] ;

    domainManagerClass::getInstance()->comms->ssid = settings[ "wifi_ssid" ] ;
    domainManagerClass::getInstance()->comms->pass = settings[ "wifi_password" ] ;
  }

  void resetSettingsFile(){

        // domain
        settings.defaultKey( "company" , "DEMO" );

        settings.defaultKey( "timeZoneIndex" , "2" );
        settings.defaultKey( "timeOffsetFromUTC" , "-420" );
        settings.defaultKey( "DST" , "0" );                        

        settings.defaultKey( "serverURL" , "10.0.0.32" );        
        settings.defaultKey( "getConfigPath" , "/brickServer1/config" );        
        settings.defaultKey( "postInspectionsPath" , "/brickServer1/inspections" );                        

        // commos        
        settings.defaultKey( "wifi_ssid" , "irazu2G" );
        settings.defaultKey( "wifi_password" , "casiocasio" );  
        
        applySettingsFile();
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

  void clockTic( DateTime dttime ){
    try{

      char buffer[30];
      DateTime local = dttime + TimeSpan(domainManagerClass::getInstance()->timeOffsetFromUTC * 60);
      snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d:%02d",
              local.year(), local.month(), local.day(), local.hour(), local.minute(), local.second());

      if( currentScreenState !=  NULL ){
        currentScreenState->clockTic(  String( buffer ) );
      }

    }catch( const std::runtime_error& error ){
      Serial.println( "*** ERROR while handling clock event ***" );                    
      Serial.println( error.what() );                    
    }         
  }  


  // handle touch events
  void handleScreenEvents( lv_event_t* e ){

    Serial.print("*** Screen event : ");
    lv_obj_t *target = lv_event_get_target(e);
    if (lv_obj_check_type( target, &lv_btn_class)) {
        lv_obj_t *label = lv_obj_get_child( target, 0);
        if (label && lv_obj_check_type(label, &lv_label_class)) {
            const char *text = lv_label_get_text(label);
            Serial.println(text);
        }
    }else{
      Serial.println("");
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


  void processPendingScreenTransition() {
      int nextScreen = setOrGetPendingScreenId();
      if (nextScreen == 0) return;
      setOrGetPendingScreenId(0);

      try {
          lv_obj_t* oldRoot = lv_scr_act();

          // ---- Minimal "just check for null" pattern ----
          bool isNew = (screenStates[nextScreen] == nullptr);
          if (isNew) {
              switch (nextScreen) {
                  
                  case SCREEN_ID_LOGIN_SCREEN:        screenStates[nextScreen] = new loginScreenClass( &settings ); break;
                  case SCREEN_ID_SETTINGS:            screenStates[nextScreen] = new settingsScreenClass( &settings ); break;
                  default:
                      throw std::runtime_error("Unknown screen ID in getOrCreateScreen()");
              }
              if (!screenStates[nextScreen])
                  throw std::runtime_error("Failed to create screen instance for ID: ");
          }

          // close previous / save
          if( currentScreenState != nullptr ){
            currentScreenState->stop();
          }
          
          // load / display new
          currentScreenState = screenStates[nextScreen];
          loadScreen((ScreensEnum)nextScreen);

          // wait for it to display
          int waitCount = 0;
          while (lv_scr_act() == oldRoot && waitCount++ < 50) {
              Serial.println("New screen not active.  tick..");
              delayBlink();
              lv_timer_handler();
              ui_tick();
          }
          if (lv_scr_act() == oldRoot) throw std::runtime_error("Timeout: LVGL did not swap root!");

          if (isNew) currentScreenState->init();

          // always
          currentScreenState->start();

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
  
  lv_event_code_t code = lv_event_get_code(e);
  if (code != LV_EVENT_PRESSED && code != LV_EVENT_CLICKED && code != LV_EVENT_VALUE_CHANGED) return;    
  
  if (stateManager != NULL) {
        stateManager->handleScreenEvents(e);
  }
}

//-------------------------------------------------


#endif



