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

static String sanitizeEDIValue(const String &in) {
    String s = in;  // make a copy so we don't mutate the original        
    s.replace('*', '_');      
    s.replace('\n', ' ');
    s.replace('\r', ' ');
    return s;
}

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

void sosHALT( String fatal ) {
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
static lv_obj_t * progress_arc = NULL;
static int progress_arc_progress = 0;
static bool progress_arc_yellow =  true;

void spinnerReset() {
    progress_arc_progress = 0;
}

void spinnerContinue() {
    if (progress_arc) {

        lv_obj_set_style_opa(progress_arc, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_opa(progress_arc, LV_OPA_COVER, LV_PART_INDICATOR);

        progress_arc_progress += 10;
        if( progress_arc_progress >= 100 ){
            progress_arc_progress = 0;
            if( progress_arc_yellow ){
                progress_arc_yellow = false;                     
                lv_obj_set_style_arc_color(
                    progress_arc,
                    lv_palette_main(LV_PALETTE_BLUE),
                    LV_PART_MAIN
                );
                lv_obj_set_style_arc_color(
                    progress_arc,
                    lv_palette_darken(LV_PALETTE_YELLOW, 3),
                    LV_PART_INDICATOR
                );            
            }else{
                progress_arc_yellow = true;
                lv_obj_set_style_arc_color(
                    progress_arc,
                    lv_palette_darken(LV_PALETTE_YELLOW, 3),
                    LV_PART_MAIN
                );

                lv_obj_set_style_arc_color(
                    progress_arc,
                    lv_palette_main(LV_PALETTE_BLUE),
                    LV_PART_INDICATOR
                );                
            }       
        }

        lv_arc_set_value(progress_arc, progress_arc_progress);

        lv_timer_handler(); // Trigger one UI update
        delay(20);          // Give display time to actually render
        lv_refr_now(NULL);
    }
}


static void spinnerEnd() {
    if (progress_arc) {
        lv_obj_set_style_opa(progress_arc, LV_OPA_TRANSP, LV_PART_MAIN);
        lv_obj_set_style_opa(progress_arc, LV_OPA_TRANSP, LV_PART_INDICATOR);
    }
}

void spinnerStart() {

    if (progress_arc) {
        lv_obj_move_foreground(progress_arc);
        spinnerContinue();
        return;
    }

    lv_obj_t * parent = lv_layer_top();
    progress_arc = lv_arc_create(parent);

    /* Size & position */
    lv_obj_set_size(progress_arc, 100, 100);
    lv_obj_center(progress_arc);

    /* Progress semantics */
    lv_arc_set_range(progress_arc, 0, 100);
    lv_arc_set_value(progress_arc, 0);

    /* Full circular background */
    lv_arc_set_bg_angles(progress_arc, 0, 360);

    /* Start at 12 o’clock */
    lv_arc_set_rotation(progress_arc, 270);

    /* Display-only (no knob) */
    lv_obj_remove_style(progress_arc, NULL, LV_PART_KNOB);

    /* Styling */
    lv_obj_set_style_arc_width(progress_arc, 14, LV_PART_MAIN);
    lv_obj_set_style_arc_width(progress_arc, 14, LV_PART_INDICATOR);

    lv_obj_set_style_arc_color(
        progress_arc,
        lv_palette_darken(LV_PALETTE_YELLOW, 3),
        LV_PART_MAIN
    );

    lv_obj_set_style_arc_color(
        progress_arc,
        lv_palette_main(LV_PALETTE_BLUE),
        LV_PART_INDICATOR
    );

    /* Start invisible but alive */
    lv_obj_set_style_opa(progress_arc, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_opa(progress_arc, LV_OPA_COVER, LV_PART_INDICATOR);

    // do not steal click
    lv_obj_clear_flag(progress_arc, LV_OBJ_FLAG_CLICKABLE);


    // ---

    spinnerContinue();

}




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

String urlEncode(const String& str) {
    String encoded = "";
    const char hex[] = "0123456789ABCDEF";
    for (size_t i = 0; i < str.length(); ++i) {
        char c = str.charAt(i);
        if ((c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') ||
            c == '-' || c == '_' || c == '.' || c == '~') {
            encoded += c;
        } else {
            encoded += '%';
            encoded += hex[(c >> 4) & 0xF];
            encoded += hex[c & 0xF];
        }
    }
    return encoded;
}





#endif

