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

extern RTC_DS3231* rtc;

#define BEARER_TOKEN "eyJhbGciOiJSUzI1NiJ9.eyJ1c2VySWQiOiJhdmF6cXVleiIsImNvbXBhbnlJZCI6Ik5FQyIsImlzU3VwZXJ1c2VyIjpmYWxzZSwiaXNEZXZpY2VUb2tlbiI6dHJ1ZSwiaWF0IjoxNzYzMTg1OTYwLCJpc3MiOiJpbnNwZWN0aW9uLWJyaWNrIiwiYXVkIjoiaW5zcGVjdGlvbi1icmljay1hcGkiLCJleHAiOjIwNzg1NDU5NjAsInN1YiI6ImF2YXpxdWV6In0.xPVELWZnBdfFz5ZwvmzLEgFoWAruTfklyjgZzx8T82BJy2mDNu06X33lovyqWOp3JDpBkE1jdTgnHAHuATTdr85UYl2IYUvG672imIAcEVCHDljuW21nLmScDbiqHcbHKcx8oboF5k4LcRZluC6M_0v2BGrcGLWb0jjzXWgmmfd_Q5DxUrwyVy7KiLAz64JQK-_H6vLUZJsnMdAIQ1JD-fWyF_JsTFrC_vxxFTcnxTDTlwzGQ6YR8hL-3swC2_Z_-FSKvtZPUJ8xDvHG1quqC12ToL9EATqGNc1iRoe8I8Dj0N5xYdvx1QPH3QI-mnND-Qtb8hwL8aBi2lDRNPK_Eg"

class commsClass{
public:

    WiFiSSLClient  client;

    // from the config
    String ssid = "irazu2G";
    String pass = "casiocasio";


    commsClass(){
    }
  
    virtual ~commsClass(){
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

    void connectToWifi(){

        Serial.println("WIFI connect....");  

        if (WiFi.status() == WL_NO_SHIELD) {
            throw std::runtime_error("Fatal WL_NO_SHIELD" );
        }

        if( WiFi.status() == WL_CONNECTED ){ 
            Serial.println("WIFI connect.... CONNECTED");  
            return;
        }

        Serial.println("Attempting to connect to SSID: ");
        Serial.println( ssid.c_str() );
        Serial.println( pass.c_str() );
        
        int status = 4;
        for( int i = 0 ; i < 20; i++){
            status = WiFi.begin( ssid.c_str(), pass.c_str() );
            if( ( status ) != WL_CONNECTED ){
                Serial.print("!WL_CONNECTED ... status: ");             
                Serial.println( status );        
                delayBlink();     
                delay(1000);
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


    WiFiSSLClient& connectToServer( String serverURL ){

        connectToWifi();

        bool serverConnected = false;
        Serial.print("Connecting to server... ");        
        Serial.println(serverURL);        
        for( int i = 0 ; i < 3; i++){
            if( !client.connect( serverURL.c_str(), 443 ) ){
                Serial.println("Cannto connect, retrying...");                
                delay( 3000 );  
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

        return client;
    }

    std::vector<String> GET( String serverURL, String path ){

        try{
                 
            connectToServer( serverURL );

            // ok send content request
            Serial.print("GET ");
            Serial.println( path );

            // send request
            client.print("GET ");
            client.print(path);         
            client.println(" HTTP/1.1");
            client.print("Host: ");
            client.println(serverURL);  
            client.println("Accept: */*");
            client.print("Authorization: Bearer ");
            client.println(BEARER_TOKEN); 
            client.println("Connection: close");             
            client.println();             
            
            // read the response ...
            int wait = 0;
            std::vector<String> response;        
            String currentRow;
            while (true) {            

                if( !client.connected() ){
                    Serial.println( "Server closed the connection" );
                    break;
                }

                if (client.available()) {

                    // read pending data
                    char c = client.read();              

                    // end of row   ...                 
                    if( c == '\r' or c == '\n' ){                    
                        if( currentRow == "" ) continue;  // null line
                        response.push_back( currentRow );  // add, next ...       
                        if( response.size() > 10000 ){
                            throw std::runtime_error( "Error more than 10,000 rows read" );
                        }                
                        Serial.println(currentRow );
                        currentRow = "";
                        continue;
                    }

                    // keep reading
                    currentRow += c;
                    if( currentRow.length() > 1000 ){
                        throw std::runtime_error( "Error more than 1000 chars in line" );
                    }                

                }else{

                    // no data ... wait
                    Serial.println("{no data wait}");
                    delayBlink();
                    wait += 100;
                    if( wait >= 10000 ) throw std::runtime_error( "*** ERROR *** read timeout -" );
                }                        
            }

            if( currentRow != "" ){
                response.push_back( currentRow );
            }

            Serial.println("GET done!");

            // one every get? 
            syncClockWithNTP();

            Serial.println( "Wifi shut down ***" );            
            client.stop();  
            WiFi.end();                               

            return response;

        } catch (...) { 
            WiFi.end();         
            throw;             
        }        
    }


    // redo all this in domain
    String POST( String serverURL, String path, const String& payload) {

        try{

            Serial.println("POSTing to server...");

            connectToServer( serverURL );   

            // Compose HTTP POST request
            String request = "";
            request += "POST " + path + " HTTP/1.1\r\n";
            request += "Host: " + serverURL + "\r\n";
            request += "Content-Type: text/plain\r\n";
            request += "Authorization: Bearer " BEARER_TOKEN "\r\n";            
            request += "Content-Length: " + String(payload.length()) + "\r\n";
            request += "Connection: close\r\n";
            request += "\r\n";
            request += payload;
            
            //Serial.println( request ); // leaks the token!!!
            Serial.println( "Sending ..." );

            client.print(request); // send - 
            // wait reply
            unsigned long timeout = millis();
            while (client.available() == 0) {
                if (millis() - timeout > 20000) {                    
                    client.stop();
                    throw std::runtime_error("Server did not respond!"); 
                }
            }

            // Read response
            String response = "";
            while (client.available()) {
                String line = client.readStringUntil('\n');
                response += line + "\n";
            }

            Serial.println( "read reply done!:" );                        
            Serial.println( response );            
            Serial.println( "Wifi shut down ***" );            
            client.stop();
            WiFi.end();                     

            //Extract only body , ie remove headers
            int bodyIndex = response.indexOf("\r\n\r\n");
            if (bodyIndex != -1) {
                response = response.substring(bodyIndex + 4);
            }

            if (response.indexOf("\"success\":true") != -1) {
                Serial.println( "Success !!!" );            
            } else {
                throw std::runtime_error("ERROR: Transmission error."); 
            }

            return response;

        } catch (...) {
            WiFi.end();         
            throw;             
        }
    }

    void syncClockWithNTP() {


        // only call from inside get with wifi on

        WiFiUDP ntpUDP;
        NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);

        Serial.println("Starting NTP sync...");
        timeClient.begin();

        int tries = 0;
        while (!timeClient.update()) {
            timeClient.forceUpdate();
            delay(500);
            tries++;
            if (tries > 5) {
                throw std::runtime_error("NTP sync failed: timeout");
            }
        }

        unsigned long epochTime = timeClient.getEpochTime();
        DateTime ntpTime(epochTime);

        rtc->adjust(ntpTime);  // Uses the global rtc

        Serial.println("RTC synced!");

    }

};


#endif