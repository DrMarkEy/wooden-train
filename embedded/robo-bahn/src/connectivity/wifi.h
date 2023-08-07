#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>

#include <config.h>

//#include <HTTPClient.h>


#define WIFI_STATE_OFF 0
#define WIFI_STATE_CONNECTING 1
#define WIFI_STATE_CONNECTED 2

char wifiState = WIFI_STATE_OFF;
unsigned long nextWifiUpdate = 0;
unsigned long nextWifiConnectionRetry = 0;




bool inOTAUpdate = false;

void initialize_ota() {

    ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      inOTAUpdate = true;

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nOTA-Update finished!");
      inOTAUpdate = false;
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      inOTAUpdate = false;

      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();
}





void initialize_wifi() {

    WiFi.setHostname(DEFAULT_WIFI_HOSTNAME);

    // Station mode: Connect as a wifi client to a wifi router
    WiFi.mode(WIFI_STA);

    Serial.print("Connecting to ");
    Serial.println(DEFAULT_WIFI_SSID);

    WiFi.begin(DEFAULT_WIFI_SSID, DEFAULT_WIFI_PASSWORD);

    wifiState = WIFI_STATE_CONNECTING;
    nextWifiConnectionRetry = millis() + WIFI_RETRY_INTERVAL;
}


void update_wifi() {

   ArduinoOTA.handle();



  if(millis() > nextWifiUpdate) {
    
    if(wifiState == WIFI_STATE_CONNECTING) {

      if(WiFi.status() == WL_CONNECTED) {
         Serial.print("WiFi connected: ");
         Serial.println(WiFi.localIP());

         // TODO: Connect to Websocket-server!


/*
HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
      http.begin("192.168.10.29/train.html");
      
      // If you need Node-RED/server authentication, insert user and password below
      //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");
      
      // Send HTTP GET request
      int httpResponseCode = http.GET();
      
      if (httpResponseCode>0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        Serial.println(payload);
      }
      else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
*/

        Serial.println("Checking for newer OTA version...");
        initialize_ota();

         
         wifiState = WIFI_STATE_CONNECTED;         
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

