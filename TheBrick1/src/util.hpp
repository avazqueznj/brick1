/********************************************************************************************
 * CONFIDENTIAL AND PROPRIETARY
 * 
 * The Brick 1.0 
 * © [2025] [Alejandro Vazquez]. All rights reserved.
 * 
 ********************************************************************************************/

#ifndef UTIL_H
#define UTIL_H

#include "misc/lv_color.h"
#include <Arduino.h>
#include "lvgl.h"
#include <functional>
#include "mbed.h"

#include <map>
#include <vector>

#include "KVStore.h"
#include "kvstore_global_api.h"

//----------------------------------------------

// yea , really .... no other way in lvgl 8
lv_obj_t* get_checked_child(lv_obj_t* list) {
    uint32_t child_count = lv_obj_get_child_cnt(list);
    for (uint32_t i = 0; i < child_count; ++i) {
        lv_obj_t* btn = lv_obj_get_child(list, i);
        if (!lv_obj_check_type(btn, &lv_btn_class)) continue; // only buttons
        if (lv_obj_has_state(btn, LV_STATE_CHECKED)) {
            return btn;
        }
    }
    return nullptr;
}

//----------------------------------------------

void delayBlink() {
    digitalWrite(LED_BUILTIN, HIGH);
    thread_sleep_for( 25 );
    digitalWrite(LED_BUILTIN, LOW);
    thread_sleep_for( 25 );    
}

void sosBlink() {
  const int dot = 200;    // ms
  const int dash = 600;
  const int gap = 200;

  // S : dot dot dot
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(dot);
    digitalWrite(LED_BUILTIN, LOW);
    delay(gap);
  }

  delay(gap);

  // O : dash dash dash
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(dash);
    digitalWrite(LED_BUILTIN, LOW);
    delay(gap);
  }

  delay(gap);

  // S : dot dot dot
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(dot);
    digitalWrite(LED_BUILTIN, LOW);
    delay(gap);
  }

  // Gap between repetitions
  delay(1000);
}

//-------------------------------------------

LV_FONT_DECLARE(lv_font_montserrat_28);
static lv_obj_t * overlay = NULL;
static lv_style_t style_font;
static bool style_ready = false;

static void btn_event_cb(lv_event_t * e)
{
    if (lv_event_get_code(e) == LV_EVENT_VALUE_CHANGED) {
        if (overlay) {
            lv_async_call((lv_async_cb_t)lv_obj_del, overlay); // deletes the window
            overlay = NULL;
        }
    }
}


void createDialog( const char* message )
{
    if (overlay != nullptr) {
        Serial.println("Dialog already open, ignoring createDialog call.");
        return;
    }

    static const char * btns[] = { "OK", "" };

    // Create overlay
    overlay = lv_obj_create(lv_scr_act());
    lv_obj_set_size(overlay, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(overlay, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(overlay, LV_OPA_50, 0);
    lv_obj_clear_flag(overlay, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(overlay, LV_OBJ_FLAG_CLICKABLE); 

    // Create message box
    lv_obj_t * mbox = lv_msgbox_create(overlay, "", message, btns, false);
    lv_obj_center(mbox);

    // One-time style init
    if (!style_ready) {
        lv_style_init(&style_font);
        lv_style_set_text_font(&style_font, &lv_font_montserrat_28);
        style_ready = true;
    }

    // Apply style to text and buttons
    lv_obj_add_style(lv_msgbox_get_text(mbox), &style_font, 0);
    lv_obj_t * btnm = lv_msgbox_get_btns(mbox);
    lv_obj_add_style(btnm, &style_font, 0);

    // Set callback on button matrix
    lv_obj_add_event_cb(btnm, btn_event_cb, LV_EVENT_ALL, NULL);
}



//----------------------------------------------

#include <vector>

std::vector<String> tokenize(String input, char delimiter) {

    Serial.println( input );

    std::vector<String> result;
    int start = 0;
    int end = input.indexOf(delimiter);

    while (end != -1) {
        result.push_back(input.substring(start, end));
        start = end + 1;
        end = input.indexOf(delimiter, start);
    }

    result.push_back(input.substring(start));
    return result;
}

//----------------------------------------------

// Declare a global pointer for the spinner so we can remove it later
static lv_obj_t * spinner = NULL;

// Timer callback to remove the spinner
static void spinnerEnd() {

    if (spinner) {
        lv_obj_del(spinner);  // Remove (delete) the spinner from screen
        spinner = NULL;
    }

}

void spinnerStart() {

    if (spinner) {
        lv_obj_del(spinner);  // Remove (delete) the spinner from screen
        spinner = NULL;
    }

    // Create a spinner with 1000ms rotation and 60 degree arc
    spinner = lv_spinner_create(lv_scr_act(), 1000, 60);

    // Set spinner size and center it
    lv_obj_set_size(spinner, 100, 100);
    lv_obj_center(spinner);

    // Customize appearance (optional)
    lv_obj_set_style_arc_color(spinner, lv_palette_main(LV_PALETTE_YELLOW), LV_PART_MAIN);
    lv_obj_set_style_arc_width(spinner, 8, LV_PART_MAIN);

    lv_timer_handler(); // Trigger one UI update
    delay(20);          // Give display time to actually render

    lv_refr_now(NULL);

}


    //------------------------------

    void saveToKVStore( const String path, const std::vector<String>* file) {

        Serial.print("Save to KVStore....");
        Serial.println( path );

        String joined = "";
        for (size_t i = 0; i < file->size(); i++) {
            joined += file->at(i);
            joined += '\n';
        }

        int ret = kv_set(path.c_str(), joined.c_str(), joined.length(), 0);
        if (ret != MBED_SUCCESS) {
            throw std::runtime_error("Failed to save  to KVStore!");
        }

        Serial.println("Saved to KVStore.");
    }

    //-----

    #define KV_BUFFER_SIZE  10240
    std::vector<String> loadFromKVStore( const String path ) {
        Serial.print("Load from KVStore   ....");
        Serial.print(path);

        size_t actual_size = 0;
        char buffer[ KV_BUFFER_SIZE ]; 

        int ret = kv_get( path.c_str(), buffer, sizeof(buffer), &actual_size);
        if (ret != MBED_SUCCESS || actual_size == 0) {
            throw std::runtime_error("Failed to read file from disk!");
        }

        String joined = String(buffer).substring(0, actual_size);

        std::vector<String> file;
        int start = 0;
        int end = joined.indexOf('\n');
        while (end >= 0) {
            String line = joined.substring(start, end);
            line.trim();
            if (line.length() > 0) {
                file.push_back(line);
            }
            start = end + 1;
            end = joined.indexOf('\n', start);
        }

        return( file );

        Serial.println("Config loaded from KVStore and parsed!");
    }

//-------------------------------------------------

class configClass : public std::map<String, String> {
public:

    // Throwing operator[]
    String& operator[](const String& key) {
        auto it = this->find(key);
        if (it == this->end())
            throw std::out_of_range(("Missing config key: " + key).c_str());
        return it->second;
    }
    
    void defaultKey( String k, String v ){
        std::map<String, String>::operator[](k) = v;
    }

    void load(const String& filename) {
        this->clear(); //<------------- note
        try {
            auto lines = loadFromKVStore(filename); // may throw
            for (const String& line : lines) {
                String s = line; s.trim();
                if (s.length() == 0 || s.startsWith("//")) continue;
                int eq = s.indexOf('=');
                if (eq > 0) {
                    String k = s.substring(0, eq); k.trim();
                    String v = s.substring(eq + 1); v.trim();
                    std::map<String, String>::operator[](k) = v;
                }
            }
        } catch (const std::exception& e) {
            // Add more context and rethrow
            String msg = String("Could not load config: ") + filename + " :: " + e.what();
            Serial.println(msg); // optional: log before throw
            throw std::runtime_error(msg.c_str());
        }
    }



    void save(const String& filename) const {
        try {
            std::vector<String> lines;
            for (const auto& it : *this)
                lines.push_back(it.first + "=" + it.second);
            saveToKVStore(filename, &lines); // may throw
        } catch (const std::exception& e) {
            String msg = String("Could not save config: ") + filename + " :: " + e.what();
            Serial.println(msg); // optional
            throw std::runtime_error(msg.c_str());
        }
    }



};

//-------------------------------------------------

#endif

