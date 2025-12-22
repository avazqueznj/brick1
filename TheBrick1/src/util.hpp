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

//-------------------------------------------------

//++++>>>


#include <kvstore_global_api.h>  

static bool kvKeyExists( const String &path ) {

    kv_info_t info;
    if( kv_get_info(path.c_str(), &info) == MBED_SUCCESS ){
            return true;
    }

    return false;
}

// -----------------------------

// Reference overload (actual impl)
void saveToKVStore(const String path, const std::vector<String> &file) {

    Serial.print("Save to KVStore....");
    Serial.println(path);

    
    String kvFile = "";
    for (const String& line : file) {
        kvFile += line + "\n";
    }

    size_t data_len = (size_t)kvFile.length();

    int ret = kv_set(path.c_str(), kvFile.c_str(), data_len, 0);

    if (ret != MBED_SUCCESS) {
        Serial.println("ERROR: kv_set failed.");
        throw std::runtime_error("Failed to save to KVStore!");
    }

    Serial.println("Saved to KVStore.");
}

// Pointer overload for ompatibility 
void saveToKVStore(const String path, const std::vector<String> *file) {    
    if (file == NULL) {
        throw std::runtime_error("saveToKVStore: file pointer is null");
    }
    saveToKVStore(path, *file);
}

// String overload (normalizes to line format then saves)
void saveToKVStore(const String path, const String &kvFile) {

    Serial.print("Save to KVStore....");
    Serial.println(path);

    size_t data_len = (size_t)kvFile.length();

    int ret = kv_set(path.c_str(), kvFile.c_str(), data_len, 0);

    if (ret != MBED_SUCCESS) {
        Serial.println( String("ERROR: kv_set failed ->")+ ret );
        throw std::runtime_error("Failed to save to KVStore!");
    }

    Serial.println("Saved to KVStore.");

}

// ------------------------------ 

#define KV_BUFFER_SIZE  10000 // 10k 
std::vector<String> loadFromKVStore( const String path ) {

    static char buffer[ KV_BUFFER_SIZE ]; 

    Serial.print("Load from KVStore   ....");
    Serial.println( path + " " );

    // get the raw data
    size_t actual_size = 0;
    int ret = kv_get( path.c_str(), buffer, sizeof(buffer), &actual_size);
    if (ret != MBED_SUCCESS || actual_size == 0) {
        throw std::runtime_error("Failed to read file from disk!");
    }

    // get a limited buffer
    String joined = String(buffer).substring(0, actual_size);

    // make lines
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

    Serial.println("File loaded from KVStore ");
}

// ------------------------------ HISTORY HELPERS

#define NUM_INSPECTION_SLOTS 10
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

void zapKVStore()
{
    Serial.println("[KV] Zapping KVStore...");

    int rc = kv_reset("/kv/");
    if (rc != MBED_SUCCESS) {
        Serial.print("[KV] kv_reset failed, rc=");
        Serial.println(rc);
        throw std::runtime_error("zapKVStore failed");
    }

    Serial.println("[KV] KVStore wiped");
}

//-------------------------------------------------

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


//=============================

// QSPI

    void openQSPI(){
        // 1) Ensure QSPI filesystem is accessible.
        //    If /qspi/ can be opened, assume it's already mounted.
        DIR* qspiDir = opendir("/qspi/");
        if (qspiDir) {
            Serial.println("[PICS] /qspi/ already accessible, skipping fs.mount().");
            closedir(qspiDir);
        } else {
            Serial.println("[PICS] /qspi/ not accessible, trying fs.mount()...");
            int err = fs.mount(&qspi);
            if (err) {
                Serial.print("[PICS] QSPI mount failed, code: ");
                Serial.println(err);
                throw std::runtime_error("syncPics: QSPI mount failed");
            }
            Serial.println("[PICS] fs.mount() succeeded.");
        }
    }

    DIR* openDirFromQSPI(){
        DIR* dir = opendir("/qspi/");
        if (dir == NULL) {
            sosHALT("[ZAP PICS] Failed to open /qspi/ directory, aborting.");
        }

        return( dir );
    }

    void closeDirFromQSPI( DIR* dir ){
        closedir(dir);        
    }    

    void deleteFileFromQSPI( String fullPath ){
        int rc = remove(fullPath.c_str());
        if (rc == 0) {
            Serial.println("[PICS]   OK (deleted)");
        } else {
            sosHALT("[PICS]   ERROR (could not delete)");
        }        
    }

    FILE* openFileFromQSPI( String path ){
        return fopen(path.c_str(), "rb");
    }

    void closeFileFromQSPI( FILE* f ){
        fclose(f);
    }

    //-=-=-=-=

void listQSPIFiles(const char* path) {
  Serial.print("Listing directory: "); Serial.println(path);
  // Use opendir/readdir/closedir (POSIX style, supported by Mbed FS)
  DIR* dir = opendir(path);
  if (!dir) {
    sosHALT("  (failed to open directory)");
  }
  struct dirent* de;
  int found = 0;
  while ((de = readdir(dir)) != NULL) {
    Serial.print("  ");
    Serial.print(de->d_name);
    if (de->d_type == DT_DIR) Serial.print(" [DIR]");
    Serial.println();
    found++;
  }
  closedir(dir);
  if (!found) Serial.println("  (empty)");
}

#include <stdio.h>
#include <stdexcept>
#include <SDRAM.h>

// QSPI FS is handled elsewhere; helpers assume it's already mounted.
void loadQSPIBinaryFileToSDRAM(const String& path, size_t& outLen) {

    outLen = 0;

    // Hard fail if someone forgot to allocate in setup()
    if (jpg_io_buf == NULL) {
        String msg = "loadQSPIBinaryFileToSDRAM: jpg_io_buf is NULL (not allocated in setup)";
        Serial.println(msg);
        throw std::runtime_error(msg.c_str());
    }

    Serial.println("Try mount FS" );
    int err = fs.mount(&qspi);
    if (err) {
        //sosBlink("Mount failed (code: " + err); 
        Serial.println("Err mounted ?");
    }

    // open the file
    FILE* f = fopen(path.c_str(), "rb");
    if (f == NULL) {
        String msg = "loadQSPIBinaryFileToSDRAM: cannot open for read: " + path;
        Serial.println(msg);
        throw std::runtime_error(msg.c_str());
    }

    // Get file size
    if (fseek(f, 0, SEEK_END) != 0) {
        fclose(f);
        String msg = "loadQSPIBinaryFileToSDRAM: fseek end failed for: " + path;
        Serial.println(msg);
        throw std::runtime_error(msg.c_str());
    }
    long fileSize = ftell(f);
    if (fileSize < 0) {
        fclose(f);
        String msg = "loadQSPIBinaryFileToSDRAM: ftell failed for: " + path;
        Serial.println(msg);
        throw std::runtime_error(msg.c_str());
    }

    // rewind
    if (fseek(f, 0, SEEK_SET) != 0) {
        fclose(f);
        String msg = "loadQSPIBinaryFileToSDRAM: fseek set failed for: " + path;
        Serial.println(msg);
        throw std::runtime_error(msg.c_str());
    }

    // Check capacity vs JPG_IO_BUF_SIZE
    if ((size_t)fileSize > JPG_IO_BUF_SIZE) {
        fclose(f);
        Serial.print("loadQSPIBinaryFileToSDRAM: file too big for jpg_io_buf. size=");
        Serial.print((size_t)fileSize);
        Serial.print(" > cap=");
        Serial.println(JPG_IO_BUF_SIZE);
        throw std::runtime_error("loadQSPIBinaryFileToSDRAM: file too big for jpg_io_buf");
    }

    // Read file into the static jpg_io_buf
    size_t totalRead = fread(jpg_io_buf, 1, (size_t)fileSize, f);
    fclose(f);

    if (totalRead != (size_t)fileSize) {
        String msg = "loadQSPIBinaryFileToSDRAM: short read for: " + path;
        Serial.println(msg);
        throw std::runtime_error(msg.c_str());
    }

    outLen = (size_t)fileSize;

    Serial.print("loadQSPIBinaryFileToSDRAM: loaded ");
    Serial.print(outLen);
    Serial.print(" bytes from ");
    Serial.println(path);

    // No return; caller knows to use jpg_io_buf.
}

//--------------------------------------------------
// Save binary buffer → QSPI file
//--------------------------------------------------
void saveQSPIBinaryFileFromBuffer(const String& path, const uint8_t* data, size_t len) {

    Serial.println("Saving file " + path);

    FILE* f = fopen(path.c_str(), "wb");
    if (f == NULL) {
        String msg = "saveQSPIBinaryFileFromBuffer: cannot open for write: " + path;
        Serial.println(msg);
        throw std::runtime_error(msg.c_str());
    }

    size_t written = fwrite(data, 1, len, f);
    fclose(f);

    if (written != len) {
        String msg = "saveQSPIBinaryFileFromBuffer: short write for: " + path;
        Serial.println(msg);
        throw std::runtime_error(msg.c_str());
    }

    Serial.println("Saving file " + path + "Done!!" );    
}

//=============================


void saveTextVecToQSPI(const String& path, const std::vector<String>* file)
{

    if (file == nullptr) {
        throw std::runtime_error("saveTextVecToQSPI: file pointer is null");
    }

    if (file->empty()) {
        throw std::runtime_error("saveTextVecToQSPI: empty config");
    }

    // Ensure QSPI is mounted
    openQSPI();

    Serial.println("[CFG] Saving config to QSPI: " + path);

    FILE* f = fopen(path.c_str(), "wb");
    if (f == NULL) {
        throw std::runtime_error(
            ("saveTextVecToQSPI: cannot open for write: " + path).c_str()
        );
    }

    // Write line-by-line to avoid giant buffers
    for (const String& line : *file) {
        size_t len = line.length();
        if (fwrite(line.c_str(), 1, len, f) != len ||
            fwrite("\n", 1, 1, f) != 1)
        {
            fclose(f);
            throw std::runtime_error("saveTextVecToQSPI: write failed");
        }
    }

    fclose(f);

    Serial.println("[vec] Config saved OK");
}

std::vector<String> readTextVecFromQSPI(const String& path)
{
    std::vector<String> result;

    // Ensure QSPI is mounted
    openQSPI();

    Serial.println("[vec] readTextVecFromQSPI from QSPI: " + path);

    FILE* f = fopen(path.c_str(), "rb");
    if (f == NULL) {
        throw std::runtime_error(
            ("loadFromQSPI: cannot open for read: " + path).c_str()
        );
    }

    char lineBuf[512];   // NJ style: fixed, explicit, predictable

    while (fgets(lineBuf, sizeof(lineBuf), f)) {

        // Strip newline / CRLF
        size_t len = strlen(lineBuf);
        while (len > 0 &&
               (lineBuf[len - 1] == '\n' || lineBuf[len - 1] == '\r')) {
            lineBuf[--len] = '\0';
        }

        // Skip empty lines (optional)
        if (len == 0) {
            continue;
        }

        result.emplace_back(lineBuf);
    }

    fclose(f);

    Serial.print("[vec] Loaded ");
    Serial.print(result.size());
    Serial.println(" lines");

    return result;
}

void clearTextVecQSPI(const String& path)
{
    openQSPI();

    Serial.println("[CFG] Clearing QSPI text file: " + path);

    FILE* f = fopen(path.c_str(), "wb");
    if (f == NULL) {
        throw std::runtime_error(
            ("clearTextVecQSPI: cannot open for write: " + path).c_str()
        );
    }

    // "wb" truncates or creates the file
    fclose(f);

    Serial.println("[CFG] File cleared");
}

#endif

