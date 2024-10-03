/*
  wifi.h - Handles connection to a WiFi network and over the air updates (OTA)
  Created by Marc Mendler, August 2023
*/
#ifndef wificonnector_h
#define wificonnector_h


#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <HTTPClient.h>
#include <UrlEncode.h>

#include <config.h>


#define WIFI_STATE_OFF 0
#define WIFI_STATE_CONNECTING 1
#define WIFI_STATE_CONNECTED 2


class WifiConnector
{

  private:
  char wifiState = WIFI_STATE_OFF;
  unsigned long nextWifiUpdate = 0;
  unsigned long nextWifiConnectionRetry = 0;
  bool inOTAUpdate = false;
  void (*wifiConnectedCallback)() = nullptr;


  void initialize_ota() {

    static WifiConnector* me = this;
  
    ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      me->inOTAUpdate = true;

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nOTA-Update finished!");
      me->inOTAUpdate = false;
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      me->inOTAUpdate = false;

      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

    ArduinoOTA.begin();
  }



  public:

  WifiConnector() {
    WiFi.setHostname(DEFAULT_WIFI_HOSTNAME);

    // Station mode: Connect as a wifi client to a wifi router
    WiFi.mode(WIFI_STA);

    Serial.print("Connecting to ");
    Serial.println(DEFAULT_WIFI_SSID);

    WiFi.begin(DEFAULT_WIFI_SSID, DEFAULT_WIFI_PASSWORD);

    wifiState = WIFI_STATE_CONNECTING;
    nextWifiConnectionRetry = millis() + WIFI_RETRY_INTERVAL;
  }

  bool isInOTAUpdate(){
    return this->inOTAUpdate;
  }

  void Loop() {

    ArduinoOTA.handle();



    if(millis() > nextWifiUpdate) {
    
      if(wifiState == WIFI_STATE_CONNECTING) {

        if(WiFi.status() == WL_CONNECTED) {
           Serial.print("WiFi connected: ");
           Serial.println(WiFi.localIP());

           // TODO: Connect to Websocket-server!



          Serial.println("Checking for newer OTA version...");
          initialize_ota();

         
           wifiState = WIFI_STATE_CONNECTED;     
           if(wifiConnectedCallback != nullptr) {
             wifiConnectedCallback();
           }    
        }
        else {

          if(millis() > nextWifiConnectionRetry) {
            Serial.println("Retrying WiFi connection..");
            WiFi.disconnect();
            WiFi.reconnect();
            nextWifiConnectionRetry = millis() + WIFI_RETRY_INTERVAL;
          }

          Serial.print('.');
        }
      }
      else if(wifiState == WIFI_STATE_CONNECTED) {
        
        if ((WiFi.status() != WL_CONNECTED)) {          
          Serial.println("Lost WiFi Connection.");          
          nextWifiConnectionRetry = 0;

          wifiState = WIFI_STATE_CONNECTING;
        }
      }
    
      // In any case: Recheck the WiFi state after 200 ms
      nextWifiUpdate = millis() + WIFI_CHECK_INTERVAL;
    }
  }

  void onWifiConnected(void (*_callback) ())
  {		
    this->wifiConnectedCallback = _callback;	
  }

  void sendHttpLog(String text) {
    
    HTTPClient http;

    text = urlEncode(text);    
    String url = "http://" LOGGING_SERVER_IP ":" LOGGING_SERVER_PORT "/log/"+text;
    http.begin(url);
    Serial.println(url);

    // Send HTTP GET request
    int httpResponseCode = http.GET();
      
    if (httpResponseCode<0) {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }

    // Free resources
    http.end();

  }
};

#endif