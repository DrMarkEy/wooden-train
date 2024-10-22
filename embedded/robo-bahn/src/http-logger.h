#ifndef http_logger_h
#define http_logger_h

#include <Arduino.h>
#include <connectivity/wifi.h>
#include <config.h>

//#define ENABLE_LOG_DURATION


#define LOGGING_QUEUE_LENGTH 10
#define LOGGING_QUEUE_ENTRY_LENGTH 255
static QueueHandle_t loggingQueue;



class Logger {
  private:

  WifiConnector* wifiConnector = nullptr;

  public:

  Logger() {
    
  }

  void Setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    loggingQueue = xQueueCreate(LOGGING_QUEUE_LENGTH, LOGGING_QUEUE_ENTRY_LENGTH);

  }
  
  void setWifiConnector(WifiConnector* _wifiConnector) {
    this->wifiConnector = _wifiConnector;
  }

  void Log(String str) {
    if(str.length() >= LOGGING_QUEUE_ENTRY_LENGTH) {
      Serial.println("MESSAGE EXCEEDED CHAR LIMIT:");
    Serial.println(str);    

    if(wifiConnector != nullptr)
      wifiConnector->sendHttpLog(str);

    if(xQueueSend(loggingQueue, (void*) &str, 10) != pdTRUE) {
      Serial.println("Queue Full!"); // Absurd using serial here...
    }    

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

} extern logger;

#ifdef ENABLE_LOG_DURATION
    #define LOG_DURATION(logger, name, lambda) logger->LogDuration(name, [](){lambda;})
#else
    #define LOG_DURATION(logger, name, code) code
#endif

#endif