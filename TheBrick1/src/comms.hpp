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
#define BRICK_HTTP_READ_TIMEOUT 90000
class commsClass{
public:

    // from the config
    String ssid = "irazu2G";
    String pass = "casiocasio";



    commsClass(){
    }
  
    virtual ~commsClass(){
    }


    void GET( String serverURL, String ssid, String pass, String path, std::vector<String>& response ){

            
        SSLClient client;
        client.connect( serverURL, ssid, pass );

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

        // sync clock while WiFi is still powered (before destructor kills it)
        syncClockWithNTP();

        Serial.println( "Wifi shutting down via RAII destructor ***" );            
    }


   String POST(String serverURL, String ssid, String pass, String path, const String& payload) {
        Serial.println("POSTing to server...");
        SSLClient client;
        client.connect(serverURL, ssid, pass);

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
            // No more vague messages. Throw the actual bad response.
            throw std::runtime_error("Transmission error!" ); 
        }

        return response;
    }

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

};


#endif