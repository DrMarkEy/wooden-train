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

  void LogDuration(String msg, void (*runnable) ()) {
    long startTime = micros();
    runnable();
    long endTime = micros();

    msg.concat(": ");
    msg.concat(endTime - startTime);
    Log(msg);
  }

  void Logf(const char *format, ...) {
    va_list arg;
    va_start(arg, format);
    char temp[64];
    char* buffer = temp;
    size_t len = vsnprintf(temp, sizeof(temp), format, arg);
    va_end(arg);
    if (len > sizeof(temp) - 1) {
        buffer = (char *) malloc(len+1);
        //buffer = new char[len + 1];
        if (!buffer) {
            return;
        }
        va_start(arg, format);
        vsnprintf(buffer, len + 1, format, arg);
        va_end(arg);
    }
    //len = write((const uint8_t*) buffer, len);
    this->Log(buffer);
    if (buffer != temp) {
        free(buffer);
        //delete[] buffer;
    }    
  }

  void LogBuffer(byte* buffer, int len) {
    Serial.write(buffer, len);    
  }

};

static Logger* logger;

#endif