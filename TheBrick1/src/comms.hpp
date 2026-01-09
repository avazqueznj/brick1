/********************************************************************************************
 * CONFIDENTIAL AND PROPRIETARY
 * 
 * The Brick 1.0 
 * © [2025] [Alejandro Vazquez]. All rights reserved.
 * 
 ********************************************************************************************/


#ifndef COMMS_H
#define COMMS_H

#include <Arduino.h>
#include <exception>
#include <WiFiSSLClient.h>

#include <NTPClient.h>
#include "RTClib.h"


#define SSL_CLIENT_RET_COUNT 10
#define SSL_CLIENT_RET_DELAY 2000
class SSLClient {
public:
    WiFiSSLClient client;

    SSLClient() = default;

    void connect( String serverURL, String ssid, String pass ){

        Serial.println("********************************************************");        
        Serial.println("Connecting to server... ");        
        Serial.println("********************************************************");        
        Serial.println(serverURL);     

        {

            Serial.println("WIFI connect....");  

            if (WiFi.status() == WL_NO_SHIELD) {
                throw std::runtime_error("Fatal WL_NO_SHIELD" );
            }


            Serial.println("Attempting to connect to SSID: ");
            Serial.println( ssid );
            Serial.println( pass );
            
            int status = 4;
            for( int i = 0 ; i < 20; i++){
                status = WiFi.begin( ssid.c_str(), pass.c_str() );
                if( ( status ) != WL_CONNECTED ){
                    Serial.print(" not WL_CONNECTED ... status: ");             
                    Serial.println( status );        
                    delayBlink();   
                    spinnerContinue();  
                }else{
                    break;
                }
            }

            if( status != WL_CONNECTED ){
                throw std::runtime_error("While connecting: not [WL_CONNECTED] after 10 sec"); 
            } else
            {
                Serial.println("Connected !!");             
                printWifiStatus();
            }        
        }        

        bool serverConnected = false;
        for( int i = 0 ; i < SSL_CLIENT_RET_COUNT; i++){

            client.stop();       
            delay(500);        
            spinnerContinue();   

            if( !client.connect( serverURL.c_str(), 443 ) ){
                Serial.println("Cannto connect 2, retrying...");                
                delay( SSL_CLIENT_RET_DELAY );  
                delayBlink();
            }else{
                serverConnected = true; 
                Serial.println("Connected to server!");     
                break;
            }
        }

        if( !serverConnected ){
            throw std::runtime_error( "Error could not connect to server !" );
        }
    }


    bool connected(){ // case server drops us
        return client.connected();
    }

    int available(){
        // if(!connected()){
        //     throw std::runtime_error( "COMM ERROR - NOT CONNECTED!" );
        // }
        return client.available();
    }
    char read(){
        if(!connected()){
            throw std::runtime_error( "COMM ERROR - NOT CONNECTED!" );
        }
        return client.read();
    }
    String readStringUntil(char terminator){
        if(!connected()){
            throw std::runtime_error( "COMM ERROR - NOT CONNECTED!" );
        }
        return client.readStringUntil(terminator);
    }

    
    void print( const char* data ){
        if(!connected()){
            throw std::runtime_error( "COMM ERROR - NOT CONNECTED!" );
        }
        client.print( data );
    }
    void println( const char* data ){
        if(!connected()){
            throw std::runtime_error( "COMM ERROR - NOT CONNECTED!" );
        }
        client.println( data );
    }
    void print( String& data ){
        if(!connected()){
            throw std::runtime_error( "COMM ERROR - NOT CONNECTED!" );
        }
        client.print( data.c_str() );
    }
    void println( String& data ){
        if(!connected()){
            throw std::runtime_error( "COMM ERROR - NOT CONNECTED!" );
        }
        client.println( data.c_str() );
    }
    void println( ){
        if(!connected()){
            throw std::runtime_error( "COMM ERROR - NOT CONNECTED!" );
        }
        client.println( );
    }

    int read(uint8_t* buffer, size_t size){
        if(!connected()){
            throw std::runtime_error( "COMM ERROR - NOT CONNECTED!" );
        }
        return client.read( buffer, size );
    }

    
    virtual ~SSLClient(){
        client.stop();  
        delay(200); // Give Mbed OS time to send the FIN packet

        // THE FIX: Instead of just end(), check status and force a hardware reset
        if (WiFi.status() != WL_NO_SHIELD) {
            WiFi.disconnect(); 
            delay(100);
            WiFi.end();
        }
        
        // Final grace period for the RPC bus to clear
        delay(200);
        
        Serial.println("                [CLEAN SHUTDOWN] Hardware released");
    }


    void printWifiStatus() {

        Serial.print("SSID: ");
        Serial.println(WiFi.SSID());

        IPAddress ip = WiFi.localIP();
        Serial.print("IP Address: ");
        Serial.println(ip);

        long rssi = WiFi.RSSI();
        Serial.print("signal strength (RSSI):");
        Serial.print(rssi);
        Serial.println(" dBm");
    }
};

//=================================================================================
//=================================================================================
//=================================================================================

extern RTC_DS3231* rtc;
extern String BEARER_TOKEN;
#define BRICK_HTTP_READ_TIMEOUT 30000
class commsClass{
public:

    // from the config
    String ssid = "irazu2G";
    String pass = "casiocasio";

    commsClass(){}
    virtual ~commsClass(){}

    //---------------------------------------------------------------

    uint8_t* GETImageToSDRAM(
        String serverURL,
        const String& uuid,
        size_t& outLen
    ) {

        SSLClient client;
        client.connect( serverURL, ssid, pass );
        spinnerContinue();        

        outLen = 0;
        String path = "/api/device/images/" + uuid;

        Serial.println("==============================");
        Serial.println("[IMG] Starting image download (allocate-once)");
        Serial.print("[IMG] Server: "); Serial.println(serverURL);
        Serial.print("[IMG] Path: "); Serial.println(path);
        Serial.println("==============================");
    
        uint8_t* buffer = 0;
        int httpStatus = 0;
        String statusLine;
        try{
            Serial.println("[IMG] Sending HTTP request...");
            client.print("GET " + path + " HTTP/1.1\r\n");
            client.print("Host: " + serverURL + "\r\n");
            client.print("Authorization: Bearer " + BEARER_TOKEN + "\r\n");
            client.print("Accept: */*\r\n");
            client.print("Connection: close\r\n");
            client.print("\r\n");

            int contentLength = -1;
            String line;
            String contentType = "";
            bool isStatusLine = true;

            Serial.println("[IMG] Reading HTTP headers...");

            int headerTries = 0;
            const int HEADER_MAX_TRIES = 100; // 100 * 100ms = 10s

            Serial.println("[IMG] Reading HTTP headers...");
            while (client.connected()) {
                if (!client.available()) {
                    Serial.println("wait .... "); 
                    delay(100);
                    headerTries++;
                    if (headerTries >= HEADER_MAX_TRIES) {                        
                        Serial.println("[FATAL] HTTP header read timeout!");
                        throw std::runtime_error("HTTP header read timeout");
                    }
                    continue;
                }
                headerTries = 0; // Reset on progress
                line = client.readStringUntil('\n');
                line.trim();
                if (line.length() == 0) break;

                Serial.print("[HDR] "); Serial.println(line);

                if (isStatusLine) {
                    statusLine = line;
                    isStatusLine = false;
                    int firstSpace = line.indexOf(' ');
                    int secondSpace = line.indexOf(' ', firstSpace + 1);
                    if (firstSpace > 0 && secondSpace > firstSpace) {
                        httpStatus = line.substring(firstSpace + 1, secondSpace).toInt();
                        Serial.print("[HDR] HTTP Status = "); Serial.println(httpStatus);
                    }
                }
                if (line.startsWith("Content-Length:")) {
                    contentLength = line.substring(15).toInt();
                    Serial.print("[HDR] Content-Length = ");
                    Serial.println(contentLength);
                }
                if (line.startsWith("Content-Type:")) {
                    contentType = line.substring(13);
                    contentType.trim();
                    Serial.print("[HDR] Content-Type = ");
                    Serial.println(contentType);
                }
                if (line.startsWith("Transfer-Encoding:") && line.indexOf("chunked") >= 0) {
                    Serial.println("[FATAL] Chunked transfer not allowed!");
                    throw std::runtime_error("Image download error: chunked transfer not supported");
                }
            }

            // --- Check HTTP Status ---
            if (httpStatus < 200 || httpStatus >= 300) {
                // Not OK! Read and log the body as error message
                Serial.print("[FATAL] HTTP error code: "); Serial.println(httpStatus);
                String errorMsg;
                while (client.available()) {
                    char c = client.read();
                    errorMsg += c;
                }
                Serial.print("[FATAL] Server error message: ");
                Serial.println(errorMsg);
                String error = "Image download failed with HTTP status " +  String(httpStatus) + errorMsg;
                throw std::runtime_error(
                    error.c_str()
                );
            }

            if (!contentType.startsWith("image/jpeg")) {
                Serial.print("[FATAL] Bad Content-Type: "); Serial.println(contentType);
                throw std::runtime_error("Image download error: not a JPEG (Content-Type mismatch)");
            }

            if (contentLength <= 0) {
                Serial.println("[FATAL] No Content-Length!");
                throw std::runtime_error("Image download error: missing Content-Length");
            }

            Serial.print("[IMG] Allocating SDRAM: ");
            Serial.println(contentLength);
            buffer = (uint8_t*)SDRAM.malloc(contentLength);
            if (!buffer) {
                Serial.println("[FATAL] SDRAM alloc failed!");
                throw std::runtime_error("SDRAM allocation failed");
            }

            size_t totalRead = 0;
            size_t tries = 0;
            const size_t MAX_TRIES = 100; 
            while (totalRead < (size_t)contentLength) {
                int n = client.read(buffer + totalRead, contentLength - totalRead);
                if (n > 0) {
                    totalRead += n;
                    Serial.print("[IMG] Bytes read: "); Serial.println(totalRead);
                    tries = 0;
                } else if (!client.connected()) {
                    Serial.println("[FATAL] Connection closed before download complete.");
                    break;
                } else {
                    delay(100);
                    Serial.print("[IMG] No data, wait .... ");        
                    tries++;
                    if (tries >= MAX_TRIES) {
                        Serial.println("[FATAL] Read timeout: no progress for 10 seconds.");
                        throw std::runtime_error("Image download failed: network timeout (no progress for 10 seconds)");
                        break;
                    }
                }
            }


            if (totalRead != (size_t)contentLength) {
                SDRAM.free(buffer);
                Serial.print("[FATAL] Only read "); Serial.print(totalRead); Serial.print(" of "); Serial.println(contentLength);
                throw std::runtime_error("Image download incomplete (Content-Length mismatch)");
            }

            outLen = totalRead;
            Serial.println("[IMG] Download complete");
            Serial.print("[IMG] Total bytes: "); Serial.println(outLen);
            Serial.println("==============================");

            spinnerContinue();
            return buffer;

        } catch(...) {
            if (buffer) {
                SDRAM.free(buffer);
                buffer = nullptr;
            }        
            throw;
        }
    }    


    //---------------------------------------------------------------

    void GET( String serverURL, String ssid, String pass, String path, std::vector<String>& response ){
            
        SSLClient client;
        client.connect( serverURL, ssid, pass );
        spinnerContinue();

        // send content request
        Serial.print("GET ");
        Serial.println( path );

        // send request headers
        client.print("GET ");
        client.print(path);         
        client.println(" HTTP/1.1");
        client.print("Host: ");
        client.println(serverURL);  
        client.println("Accept: */*");
        client.print("Authorization: Bearer ");
        client.println(BEARER_TOKEN); 
        client.println("User-Agent: InspectionBrick/1.0 ( ARM; " + String( lv_label_get_text(objects.version_label) ) + ")"  );            
        client.println("Connection: close");             
        client.println();             
        
        // read the response ...
        int wait = 0;
        String currentRow;
        currentRow.reserve(128); // Pre-allocate internal string buffer to avoid char-by-char churn

        while (client.connected() || client.available()) {            

            if (client.available()) {

                // read pending data line-by-line for efficiency
                currentRow = client.readStringUntil('\n');
                currentRow.trim(); // Remove \r and whitespace

                // end of row check
                if( currentRow.length() > 0 ){      
                    
                    Serial.println(currentRow);
                    
                    response.push_back( currentRow );
                    
                    if( response.size() > 10000 ){
                        Serial.println( "Error more than 10,000 rows read !!!" );            
                        throw std::runtime_error( "Error more than 10,000 rows read" );
                    }                

                    // Optional: limit serial spam but show progress
                    if( response.size() % 100 == 0 ) {
                        Serial.print("Read rows: ");
                        Serial.println(response.size());
                    }
                }
                
                wait = 0; // reset timeout on successful read

            } else {

                // or no data ... wait
                Serial.print(".");
                delayBlink();
                wait += 100;
                if( wait >= BRICK_HTTP_READ_TIMEOUT ) throw std::runtime_error( "*** ERROR *** read timeout -" );
            }                        
        }

        Serial.println("GET done!");

        spinnerContinue();

        // sync clock while WiFi is still powered (before destructor kills it)
        syncClockWithNTP();

        spinnerContinue();        

        Serial.println( "Wifi shutting down via RAII destructor ***" );            
    }

    //---------------------------------------------------------------
    
    String POST(String serverURL, String ssid, String pass, String path, const String& payload) {
        Serial.println("POSTing to server...");
        SSLClient client;
        client.connect(serverURL, ssid, pass);
        spinnerContinue();   

        String request = "";
        request.reserve(payload.length() + 1024); 
        
        request += "POST " + path + " HTTP/1.1\r\n";
        request += "Host: " + serverURL + "\r\n";
        request += "Content-Type: text/plain\r\n";
        request += "Authorization: Bearer " + BEARER_TOKEN + "\r\n";            
        request += "User-Agent: InspectionBrick/1.0 (ARM; " + String(lv_label_get_text(objects.version_label)) + ")\r\n";
        request += "Content-Length: " + String(payload.length()) + "\r\n";
        request += "Connection: close\r\n";
        request += "\r\n";
        request += payload;
        
        Serial.println("--- POST PAYLOAD START ---");
        Serial.println(payload);
        Serial.println("--- POST PAYLOAD END ---");

        client.print(request); 
        spinnerContinue();   

        unsigned long timeout = millis();
        while (client.available() == 0) {
            if (millis() - timeout > BRICK_HTTP_READ_TIMEOUT) throw std::runtime_error("Server did not respond!");
            Serial.print(".");
            delayBlink();
        }

        String response = "";
        response.reserve(1024);
        while (client.available()) {
            String line = client.readStringUntil('\n');
            response += line + "\n";
            Serial.print(line);
            delayBlink();
        }

        int bodyIndex = response.indexOf("\n\n");
        if (bodyIndex == -1) bodyIndex = response.indexOf("\r\n\r\n");

        if (bodyIndex != -1) {
            response = response.substring(bodyIndex + (response.charAt(bodyIndex) == '\r' ? 4 : 2));
        }

        if (response.indexOf("\"success\":true") != -1 || response.indexOf("Duplicate inspection ID") != -1) {
            Serial.println("Success !!!");            
        } else {
            Serial.println("!!!!! POST FAILED !!!!!");
            Serial.println("RAW RESPONSE:");
            Serial.println(response);

            throw std::runtime_error("Transmission error!" ); 
        }

        return response;
    }

    //---------------------------------------------------------------
   #define POST_JPG_AK_TIMEOUT 10000
   bool POSTJPEG(
    String serverURL, String ssid, String pass, String path,
    const String& uuid, 
    uint8_t type, 
    uint8_t* data, 
    size_t len  
    ) {
        if (data == nullptr || len == 0) {
            throw std::runtime_error("POST_FATAL: Attempted to upload NULL or empty buffer for PK: " + std::string(uuid.c_str()));
        }

        SSLClient client;
        
        try {
            spinnerContinue();   
            client.connect(serverURL, ssid, pass);
            spinnerContinue();

            Serial.println("==============================");
            Serial.println("[UPLOAD] Starting Binary POST");
            Serial.print("[UPLOAD] PK: "); Serial.println(uuid);
            Serial.print("[UPLOAD] Size: "); Serial.print(len); Serial.println(" bytes");
            Serial.println("==============================");

            // 1. Send Request Line
            client.print("POST " + path + " HTTP/1.1\r\n");
            
            // 2. Send Headers
            client.print("Host: " + serverURL + "\r\n");
            client.print("Authorization: Bearer " + BEARER_TOKEN + "\r\n");
            client.print("x-uuid: " + uuid + "\r\n");
            client.print("x-type: " + String(type) + "\r\n");
            client.print("Content-Type: image/jpeg\r\n");
            client.print("Content-Length: " + String(len) + "\r\n");
            client.print("Connection: close\r\n");
            client.print("\r\n");

            // 3. Send Payload from SDRAM (NJ STYLE: LOOP THE WRITE)
            size_t totalWritten = 0;
            int attempts = 0;

            while (totalWritten < len) {
                // Attempt to write the remainder
                size_t remaining = len - totalWritten;
                size_t written = client.client.write(data + totalWritten, remaining);
                
                if (written > 0) {
                    totalWritten += written;
                    Serial.print("[WRITE] Progress: "); 
                    Serial.print(totalWritten); Serial.print("/"); Serial.print(len);
                    Serial.print(" (+"); Serial.print(written); Serial.println(")");
                    attempts = 0; // Reset failsafe
                } else {
                    // Buffer full or hardware busy
                    attempts++;
                    delay(50); // Give the radio a serious gap to clear the 16KB record
                    Serial.print("."); // Visual indicator of a "Wait" state
                    if (attempts > 100){
                        Serial.println("Socket stalled writting");
                        throw std::runtime_error("Socket stalled for too long at " + std::to_string(totalWritten));
                    }                    
                }
                spinnerContinue();   
            }


            // THE FIX: Manual terminators to force the proxy to flush the buffer
            client.print("\r\n"); 
            client.print("\r\n"); 
            Serial.println("[UPLOAD] upload complete, terminators sent, wait for response...");

            //---

            unsigned long timeout = millis();
            while (client.available() == 0) {                
                if (millis() - timeout > POST_JPG_AK_TIMEOUT ){ // shorten bcs stupid replit issue with errors
                    throw std::runtime_error("[UPLOAD] NO AK! , ABORT !!!!!!!!!!!!!!");                     
                }
                Serial.print(".");
                delayBlink();
            }

            String response = "";
            response.reserve(1024);
            while (client.available()) {
                String line = client.readStringUntil('\n');
                response += line + "\n";
                Serial.print(line);
                delayBlink();
            }

            // all of hthe below will zap the picures

            if (response.indexOf("200 OK") != -1 ) {
                Serial.print("[UPLOAD] 200 SUCCESS Final Result: "); Serial.println(response);
                return true;
            }

            if (response.indexOf("409 Conflict") != -1) {
                Serial.print("[UPLOAD] << 409 DUPLICATED >> "); Serial.println(response);
                return true;
            }            
            

            Serial.print("ERROR!!!! Server rejected upload, NO RETRY! "); Serial.println(response);
            return true;                


        } catch (const std::exception& e) {
            String chainMsg = "SYNC_EXCEPTION [" + uuid + "] -> " + String(e.what());
            Serial.print("[CRITICAL] "); Serial.println(chainMsg);
            throw std::runtime_error(chainMsg.c_str());
        } catch (...) {
            throw std::runtime_error("SYNC_EXCEPTION [" + std::string(uuid.c_str()) + "] -> Unknown exception");
        }
    }

    //---------------

    void syncClockWithNTP() {
        
        WiFiUDP ntpUDP;
        NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);

        try{

            Serial.println("Starting NTP sync...");
            timeClient.begin();

            int tries = 0;
            while ( !timeClient.forceUpdate() ) {
                delay(500);
                tries++;
                if (tries > 5) {
                    throw std::runtime_error("NTP sync failed: timeout");
                }
            }

            unsigned long epochTime = timeClient.getEpochTime();
            DateTime ntpTime(epochTime);

            rtc->adjust(ntpTime);  // Uses the global rtc

            timeClient.end();
            ntpUDP.stop();

            Serial.println("RTC synced!");

        }catch(...){
            timeClient.end();
            ntpUDP.stop();
        }
    }


    // ---

};


#endif