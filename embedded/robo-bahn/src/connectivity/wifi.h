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

#include "esp_system.h"
#include "esp_ota_ops.h"

#include <config.h>


#define WIFI_STATE_UNINITIALIZED 0
#define WIFI_STATE_CONNECTING 1
#define WIFI_STATE_CONNECTED 2


static void CheckWifiStateTaskFunction (void* parameter);

class WifiConnector
{

  private:
  char wifiState = WIFI_STATE_UNINITIALIZED;
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

  WifiConnector() {}
  
  void Run() {
    xTaskCreatePinnedToCore(
      CheckWifiStateTaskFunction,
      "Check Wifi state",
      4096,
      NULL,//( void * ) &buttonController,
      1, // Priority
      NULL,
      1 // Core affinity
    );
  }

  void setup() {
    WiFi.setHostname(DEFAULT_WIFI_HOSTNAME);

    // Station mode: Connect as a wifi client to a wifi router
    WiFi.mode(WIFI_STA);

    Serial.print("Connecting to ");
    Serial.println(DEFAULT_WIFI_SSID);

    WiFi.begin(DEFAULT_WIFI_SSID, DEFAULT_WIFI_PASSWORD);

    wifiState = WIFI_STATE_CONNECTING;

    // Log ota running partition
    const esp_partition_t* runningPartition = esp_ota_get_running_partition();
    if (runningPartition != NULL) {
      Serial.printf("Boot-Partition Label: %s\n", runningPartition->label);
      Serial.printf("Boot-Partition Typ: %d\n", runningPartition->type);
      Serial.printf("Boot-Partition Adresse: 0x%X\n", runningPartition->address);
      Serial.printf("Boot-Partition Größe: %d Bytes\n", runningPartition->size);
    } else {
      Serial.printf("Fehler beim Abrufen der Boot-Partition-Informationen\n");
    }

    /*  if( running_partition != ota0 ) {
    // copy partition ota1 to ota0
    // set boot partition to ota0
    //restart
    }*/
  }

  bool isInOTAUpdate(){
    return this->inOTAUpdate;
  }

  void updateWifiState() {
    
    //ArduinoOTA.handle();

   
     // Was not connected and trying to reconnect since last check
    if(wifiState == WIFI_STATE_CONNECTING) {

      // Is now connected
      if(WiFi.status() == WL_CONNECTED) {
        Serial.print("WiFi connected: ");
        Serial.println(WiFi.localIP());

        Serial.println("Checking for newer OTA version...");
        initialize_ota();
    
        wifiState = WIFI_STATE_CONNECTED;     
        if(wifiConnectedCallback != nullptr) {
          wifiConnectedCallback();
        }

        vTaskDelay(WIFI_CHECK_INTERVAL / portTICK_PERIOD_MS);
      }
      else { // Still not connected
        Serial.printf("Wifi not connected. Will retry in %d ms.", WIFI_RETRY_INTERVAL);
        Serial.println();
        
        WiFi.disconnect();
        WiFi.reconnect();
        
        vTaskDelay(WIFI_RETRY_INTERVAL / portTICK_PERIOD_MS);
        
        Serial.println("Retrying WiFi connection...");                
      }
    }
    // Wifi was connected on last check
    else if(wifiState == WIFI_STATE_CONNECTED) {
      
      // Wifi is now disconnected
      if ((WiFi.status() != WL_CONNECTED)) {          
        Serial.println("Lost WiFi Connection.");                  
        wifiState = WIFI_STATE_CONNECTING;        
        // Immediately start on top in the next task iteration
      }
      else {
        // Wifi is still connected: Recheck after waiting time
        vTaskDelay(WIFI_CHECK_INTERVAL / portTICK_PERIOD_MS);
      }
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
} extern wifi;

static void CheckWifiStateTaskFunction (void* parameter) {  
  wifi.setup();
  vTaskDelay(WIFI_INITIAL_CONNECTING_TIME / portTICK_PERIOD_MS);

  while(1) {    
    wifi.updateWifiState();
  }
}
#endif