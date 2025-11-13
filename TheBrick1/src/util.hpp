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

extern RTC_DS3231* rtc;

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

void sosBlink( String fatal ) {
    const int dot = 100;    // ms
    const int dash = 500;
    const int gap = 300;

    while( true ){

        Serial.println( "FATAL:" + fatal );

        // S : dot dot dot
        for (int i = 0; i < 3; i++) {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(dot);
            digitalWrite(LED_BUILTIN, LOW);
            delay(dot);
        }

        delay(gap);

        // O : dash dash dash
        for (int i = 0; i < 3; i++) {
            digitalWrite(LED_BUILTIN, HIGH);
            delay(dash);
            digitalWrite(LED_BUILTIN, LOW);
            delay(dash);
        }

        delay(1000);   
                
    }
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

    void saveToKVStore( const String path, const String &data) {
        
        std::vector<String> lines;
        int start = 0;
        while (true) {
            int end = data.indexOf('\n', start);
            if (end < 0) {
                String lastLine = data.substring(start);
                if (lastLine.length() > 0) lines.push_back(lastLine);
                break;
            }
            String line = data.substring(start, end);
            lines.push_back(line);
            start = end + 1;
        }

        saveToKVStore(path, &lines);
    }    

    //-----

    #define KV_BUFFER_SIZE  10240
    std::vector<String> loadFromKVStore( const String path ) {
        Serial.print("Load from KVStore   ....");
        Serial.print( path + " " );

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

class settingsClass : public std::map<String, String> {
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

void scroll_widget_into_view( lv_obj_t* widget )
{
    if(!widget) return;

    // Find the first scrollable ancestor
    lv_obj_t* scrollable = lv_obj_get_parent(widget);
    lv_obj_t* target = widget;
    while(scrollable) {
        if(lv_obj_has_flag(scrollable, LV_OBJ_FLAG_SCROLLABLE)) {
            // Found the scrollable container
            break;
        }
        target = scrollable;
        scrollable = lv_obj_get_parent(scrollable);
    }

    if(scrollable) {
        // Find the Y position of the target within the scrollable
        lv_coord_t y = lv_obj_get_y(target);
        lv_obj_scroll_to_y(scrollable, y, LV_ANIM_OFF);
    }
    // else: not in a scrollable, do nothing
}

//-------------------------------------------------

String newUUID() {
    uint8_t uuid[16];
    for (int i = 0; i < 16; ++i) {
        uuid[i] = random(0, 256); // On Arduino, random(max) is [0, max)
    }
    // Set the UUID version (4) and variant (RFC 4122)
    uuid[6] = (uuid[6] & 0x0F) | 0x40; // Version 4
    uuid[8] = (uuid[8] & 0x3F) | 0x80; // Variant 1 (10xxxxxx)

    char buf[37];
    snprintf(buf, sizeof(buf),
        "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        uuid[0], uuid[1], uuid[2], uuid[3],
        uuid[4], uuid[5],
        uuid[6], uuid[7],
        uuid[8], uuid[9],
        uuid[10], uuid[11], uuid[12], uuid[13], uuid[14], uuid[15]
    );
    return String(buf);
}

//-------------------------------------------------

#define NUM_INSPECTION_SLOTS 10
std::vector<String> getInspectionHistory(){

    std::vector<String> result;

    // for each slot
    for (int i = 1; i <= NUM_INSPECTION_SLOTS; ++i) {
        
        String path = "/kv/insp" + String(i);
        Serial.print(" Try read"  + path + " -> " );

        // try decode
        try {
            std::vector<String> file = loadFromKVStore(path);
            for (size_t j = 0; j < file.size(); ++j) {
                if (file[j].startsWith("DISPLAYHEADER*")) {
                    Serial.print("Slot ");
                    Serial.print(i);
                    Serial.print(": ");
                    Serial.println(file[j]);

                    result.push_back( file[j] );

                    break; // Only one per file, skip rest
                }
            }
        } catch (...) {
            Serial.println( "ERROR Cant read slot..." );
            // Slot empty or unreadable; skip
        }
    }    
    
    return result;
}

void zapInspectionHistory(){

    for (int i = 1; i <= NUM_INSPECTION_SLOTS; ++i) {
        String path = "/kv/insp" + String(i);
        kv_remove(path.c_str());
    }
    Serial.println("All inspection slots deleted.");
}

//-------------------------------------------------

#endif

