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

#include <http-logger.h>
#include <config.h>


#define WIFI_STATE_UNINITIALIZED 0
#define WIFI_STATE_CONNECTING 1
#define WIFI_STATE_CONNECTED 2

static void checkOtaTaskFunction(void* parameter);
static void CheckWifiStateTaskFunction (void* parameter);
class WifiConnector;
extern WifiConnector wifi;

class WifiConnector: public IWifiLogger
{

  private:
  char wifiState = WIFI_STATE_UNINITIALIZED;
  bool inOTAUpdate = false;
  void (*wifiConnectedCallback)() = nullptr;

  void initialize_ota() {

    ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      wifi.inOTAUpdate = true;

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      logger.Log("Start updating " + type);
    })
    .onEnd([]() {
      logger.Log("\nOTA-Update finished!");
      wifi.inOTAUpdate = false;
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      logger.Logf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      wifi.inOTAUpdate = false;

      logger.Logf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) logger.Log("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) logger.Log("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) logger.Log("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) logger.Log("Receive Failed");
      else if (error == OTA_END_ERROR) logger.Log("End Failed");
    });

    ArduinoOTA.begin();

    xTaskCreatePinnedToCore(
      checkOtaTaskFunction,
      "Check OTA Update",
      2048,
      NULL,// parameter
      5, // HIGH Priority
      NULL,
      1 // Core affinity
    );
  }

  public:

  WifiConnector() {}

  void Run() {
    xTaskCreatePinnedToCore(
      CheckWifiStateTaskFunction,
      "Check Wifi state",
      2048,
      NULL,//parameter
      1, // Priority
      NULL,
      1 // Core affinity
    );
  }

  void setup() {
    WiFi.setHostname(DEFAULT_WIFI_HOSTNAME);

    // Station mode: Connect as a wifi client to a wifi router
    WiFi.mode(WIFI_STA);

    logger.Log("Connecting to ");
    logger.Log(DEFAULT_WIFI_SSID);

    WiFi.begin(DEFAULT_WIFI_SSID, DEFAULT_WIFI_PASSWORD);

    wifiState = WIFI_STATE_CONNECTING;

    // Log ota running partition
    const esp_partition_t* runningPartition = esp_ota_get_running_partition();
    if (runningPartition != NULL) {
      logger.Logf("Boot-Partition Label: %s\n", runningPartition->label);
      logger.Logf("Boot-Partition Typ: %d\n", runningPartition->type);
      logger.Logf("Boot-Partition Adresse: 0x%X\n", runningPartition->address);
      logger.Logf("Boot-Partition Größe: %d Bytes\n", runningPartition->size);
    } else {
      logger.Logf("Fehler beim Abrufen der Boot-Partition-Informationen\n");
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

  void checkOtaUpdate() {
    ArduinoOTA.handle();

    if(!this->inOTAUpdate) {
      vTaskDelay(OTA_CHECK_INTERVAL / portTICK_PERIOD_MS);
    }
  }

  void updateWifiState() {

     // Was not connected and trying to reconnect since last check
    if(wifiState == WIFI_STATE_CONNECTING) {

      // Is now connected
      if(WiFi.status() == WL_CONNECTED) {
        logger.Log("WiFi connected: " + String(WiFi.localIP().toString()));
        logger.Log("Checking for newer OTA version...");
        initialize_ota();

        wifiState = WIFI_STATE_CONNECTED;
        if(wifiConnectedCallback != nullptr) {
          wifiConnectedCallback();
        }

        vTaskDelay(WIFI_CHECK_INTERVAL / portTICK_PERIOD_MS);
      }
      else { // Still not connected
        logger.Logf("Wifi not connected. Will retry in %d ms.", WIFI_RETRY_INTERVAL);

        WiFi.disconnect();
        WiFi.reconnect();

        vTaskDelay(WIFI_RETRY_INTERVAL / portTICK_PERIOD_MS);

        logger.Log("Retrying WiFi connection...");
      }
    }
    // Wifi was connected on last check
    else if(wifiState == WIFI_STATE_CONNECTED) {

      // Wifi is now disconnected
      if ((WiFi.status() != WL_CONNECTED)) {
        logger.Log("Lost WiFi Connection.");
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

    // Send HTTP GET request
    int httpResponseCode = http.GET();

    if (httpResponseCode<0) {
      logger.Logf("Error code: %d", httpResponseCode);
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

static void checkOtaTaskFunction (void* parameter) {
  while(1) {
    wifi.checkOtaUpdate();
  }
}
#endif