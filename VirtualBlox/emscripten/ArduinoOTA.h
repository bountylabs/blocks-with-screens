#ifndef __ARDUINO_OTA_H__
#define __ARDUINO_OTA_H__

#include <functional>

typedef enum {
  OTA_AUTH_ERROR,
  OTA_BEGIN_ERROR,
  OTA_CONNECT_ERROR,
  OTA_RECEIVE_ERROR,
  OTA_END_ERROR
} ota_error_t;


class FakeArduinoOTA 
{
public:
    typedef std::function<void(void)> THandlerFunction;
    typedef std::function<void(ota_error_t)> THandlerFunction_Error;
    typedef std::function<void(unsigned int, unsigned int)> THandlerFunction_Progress;

    FakeArduinoOTA();
    ~FakeArduinoOTA();

    void setHostname(const char* hostname);
    void onStart(THandlerFunction fn);
    void onEnd(THandlerFunction fn);
    void onError(THandlerFunction_Error fn);
    void onProgress(THandlerFunction_Progress fn);
    void begin(bool useMDNS = true);
    void handle();
};

extern FakeArduinoOTA ArduinoOTA;

#endif
