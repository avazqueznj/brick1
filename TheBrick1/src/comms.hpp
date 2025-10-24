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
#include <WiFi.h>

#include <NTPClient.h>
#include "RTClib.h"

extern RTC_DS3231* rtc;

class commsClass{
public:

    WiFiClient client;


    // String serverURL = "10.0.0.32"; "/server2025/inspections";

    // daday phone
    //String ssid = "DadyPhone";
    //String pass = "Casiopea1";

    // house
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


    WiFiClient& connectToServer( String serverURL ){

        connectToWifi();

        bool serverConnected = false;
        Serial.print("Connecting to server... ");        
        Serial.println(serverURL);        
        for( int i = 0 ; i < 3; i++){
            if( !client.connect( serverURL.c_str(), 8080 ) ){
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

        connectToServer( serverURL );

        // ok send content request
        Serial.print("GET ");
        Serial.println( path );

        // send request
        client.print("GET ");
        client.println( path );
        client.println("Accept: */*");
        client.println();
        
        // read the response ...
        int wait = 0;
        std::vector<String> response;        
        String currentRow;
        while (true) {            

            if( !client.connected() ){
                client.println( "Server closed the connection" );
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

        Serial.println("GET done!");

        Serial.println("Disconnecting socket ....");
        client.stop();            

        return response;
    }


    // redo all this in domain
    String POST( String serverURL, String path, const String& payload) {

        Serial.println("POSTing to server...");

        connectToServer( serverURL );

        // Compose HTTP POST request
        String request = "";
        request += "POST " + path + " HTTP/1.1\r\n";
        request += "Host: " + serverURL + "\r\n";
        request += "Content-Type: text/plain\r\n";
        request += "Content-Length: " + String(payload.length()) + "\r\n";
        request += "Connection: close\r\n";
        request += "\r\n";
        request += payload;

        client.print(request);

        // Wait for server response
        unsigned long timeout = millis();
        while (client.available() == 0) {
            if (millis() - timeout > 5000) {
                createDialog("Client Timeout");
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

        client.stop();

        // Extract only body , ie remove headers
        int bodyIndex = response.indexOf("\r\n\r\n");
        if (bodyIndex != -1) {
            response = response.substring(bodyIndex + 4);
        }

        Serial.println( "Success response ->" );
        Serial.println( response );

        return response;
    }

    void syncClockWithNTP() {

        connectToWifi();

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