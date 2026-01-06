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

#ifndef FS_UTIL_H
#define FS_UTIL_H

#include "misc/lv_color.h"
#include <Arduino.h>
#include "lvgl.h"
#include <functional>
#include "mbed.h"

#include <map>
#include <vector>

#include "KVStore.h"
#include "kvstore_global_api.h"

//-------------------------------------------------


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



//=============================

// QSPI

//=============================



///-----------------

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




    // void openQSPI(){
    //     // 1) Ensure QSPI filesystem is accessible.
    //     //    If /qspi/ can be opened, assume it's already mounted.
    //     DIR* qspiDir = opendir("/qspi/");
    //     if (qspiDir) {
    //         Serial.println("[PICS] /qspi/ already accessible, skipping fs.mount().");
    //         closedir(qspiDir);
    //     } else {
    //         Serial.println("[PICS] /qspi/ not accessible, trying fs.mount()...");
    //         int err = fs.mount(&qspi);
    //         if (err) {
    //             Serial.print("[PICS] QSPI mount failed, code: ");
    //             Serial.println(err);
    //             throw std::runtime_error("syncPics: QSPI mount failed");
    //         }
    //         Serial.println("[PICS] fs.mount() succeeded.");
    //     }
    // }

    DIR* openDirFromQSPI(){

        Serial.println("[STORAGE] open dir...");

        DIR* dir = opendir("/qspi/");
        if (dir == NULL) {
            sosHALT("[ZAP PICS] Failed to open /qspi/ directory, aborting.");
        }

        Serial.println("[STORAGE] open dir...  done");        

        return( dir );
    }

    void closeDirFromQSPI( DIR* dir ){

        Serial.println("[STORAGE] close dir...");        
        closedir(dir);        
        Serial.println("[STORAGE] close dir...  done");                
    }    

    void deleteFileFromQSPI( String fullPath ){

        Serial.println("[STORAGE] delete..");        

        int rc = remove(fullPath.c_str());
        if (rc == 0) {
            Serial.println("[PICS]   OK (deleted)");
        } else {
            sosHALT("[PICS]   ERROR (could not delete)");
        }        

        Serial.println("[STORAGE] delete.. done");                
    }

    FILE* openFileFromQSPI( String path ){

        Serial.println("[STORAGE] fopen..");        
        return fopen(path.c_str(), "rb");
        Serial.println("[STORAGE] fopen.. done");                
    }

    void closeFileFromQSPI( FILE* f ){
        Serial.println("[STORAGE] close..");        
        fclose(f);
        Serial.println("[STORAGE] close..  done");        
    }

//===========================================================================================

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

//===========================================================================================

#include <stdio.h>
#include <stdexcept>
#include <SDRAM.h>

void loadQSPIFileToSDRAMXXX(const String& path, uint8_t* destBuffer, size_t maxCapacity, size_t& outLen) {

    Serial.println("[STORAGE] loadQSPIFileToSDRAM..");        

    outLen = 0;

    // NJ Rule: Hard fail on null
    if (destBuffer == nullptr) throw std::runtime_error("LOAD ERROR: destBuffer is NULL");

    // KEEPING YOUR MOUNT CHECK
    openQSPI();

    FILE* f = fopen(path.c_str(), "rb");
    if (f == NULL) {
        String msg = "loadQSPIFileToSDRAM: cannot open for read: " + path;
        Serial.println(msg);
        throw std::runtime_error(msg.c_str());
    }

    // Get file size
    fseek(f, 0, SEEK_END);
    long fileSize = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (fileSize < 0) {
        fclose(f);
        throw std::runtime_error(String("loadQSPIFileToSDRAM: ftell failed for: " + path).c_str());
    }

    // Check capacity vs passed maxCapacity
    if ((size_t)fileSize > maxCapacity) {
        fclose(f);
        Serial.print("loadQSPIFileToSDRAM: file too big. size=");
        Serial.print((size_t)fileSize);
        Serial.print(" > cap=");
        Serial.println(maxCapacity);
        throw std::runtime_error("loadQSPIFileToSDRAM: file too big for buffer");
    }

    // Read file into the PROVIDED buffer
    size_t totalRead = fread(destBuffer, 1, (size_t)fileSize, f);
    fclose(f);

    if (totalRead != (size_t)fileSize) {
        throw std::runtime_error(String("loadQSPIFileToSDRAM: short read for: " + path).c_str());
    }

    // MANDATORY NJ RULE: Invalidate cache so the CPU/LVGL sees the new data 
    // that the QSPI driver just dropped into RAM.
    ///SCB_InvalidateDCache_by_Addr((uint32_t *)destBuffer, totalRead);

    outLen = (size_t)fileSize;
    Serial.println("loadQSPIFileToSDRAM: loaded " + String(outLen) + " bytes from " + path);
}

//=============

void saveQSPIFileFromSDRAMXX(const String& path, const uint8_t* data, size_t len) {

    Serial.println("Saving file " + path);

    // Ensure the data is pushed from CPU cache to SDRAM before saving
    SCB_CleanDCache_by_Addr((uint32_t *)data, len);    

    // Ensure QSPI is mounted
    openQSPI();    

    FILE* f = fopen(path.c_str(), "wb");
    if (f == NULL) {
        String msg = "saveQSPIFileFromSDRAM: cannot open for write: " + path;
        Serial.println(msg);
        throw std::runtime_error(msg.c_str());
    }

    size_t written = fwrite(data, 1, len, f);
    fclose(f);

    if (written != len) {
        String msg = "saveQSPIFileFromSDRAM: short write for: " + path;
        Serial.println(msg);
        throw std::runtime_error(msg.c_str());
    }

    Serial.println("Saving file " + path + "Done!!" );    
}

//===========================================================================================


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