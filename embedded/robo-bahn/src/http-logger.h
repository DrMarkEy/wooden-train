#ifndef http_logger_h
#define http_logger_h

#include <Arduino.h>
#include <connectivity/wifi.h>
#include <config.h>

class Logger {
  private:

  WifiConnector* wifiConnector = nullptr;

  public:

  Logger() {
    Serial.begin(SERIAL_BAUD_RATE);
  }
  
  void setWifiConnector(WifiConnector* _wifiConnector) {
    this->wifiConnector = _wifiConnector;
  }

  void Log(String str) {
    Serial.println(str);    

    if(wifiConnector != nullptr)
      wifiConnector->sendHttpLog(str);
  }
};

static Logger* logger;

#endif