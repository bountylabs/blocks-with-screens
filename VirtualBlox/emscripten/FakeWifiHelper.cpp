#include <stdio.h>
#include <emscripten.h>
#include "ESP8266HTTPClient.h"

FakeWiFi WiFi;

FakeWiFi::FakeWiFi()
{

}

FakeWiFi::~FakeWiFi()
{

}

void FakeWiFi::begin(const char* ssid, const char* password)
{
    printf("FakeWiFi begin: %s %s\n", ssid, password);
}

wl_status_t FakeWiFi::status()
{
    return WL_CONNECTED;
}

WiFiClient::WiFiClient()
: _currentByte(0)
{

}

void WiFiClient::setData(void* dataBuffer, int bufferSize)
{
    _dataBuffer = dataBuffer;
    _bufferSize = bufferSize;
}

WiFiClient::~WiFiClient()
{

}

size_t WiFiClient::available()
{
    printf("called wifiClient available()\n");
    return _bufferSize - _currentByte;
}

int WiFiClient::readBytes(uint8_t* buff, int size)
{
    printf("wifiClient readBytes %d\n", size);

    int bytesToRead = size;
    if (_bufferSize - _currentByte < bytesToRead)
    {
        bytesToRead = _bufferSize - _currentByte;
    }

    uint8_t* data = (uint8_t*)_dataBuffer;
    memcpy(buff, &data[_currentByte], bytesToRead);
    _currentByte += bytesToRead;
    return bytesToRead;
}

HTTPClient::HTTPClient()
: _wifiClient(new WiFiClient())
, _dataBuffer(NULL)
, _bufferSize(0)
, _url(NULL)
{

}

HTTPClient::~HTTPClient()
{
    delete _wifiClient;
}

bool HTTPClient::begin(const char* url)
{
    printf("http begin: %s\n", url);
    _url = (char*)malloc(strlen(url) + 1);
    strcpy(_url, url);
    return true;
}
int HTTPClient::GET()
{
    int error;
    printf("GET called, retrieving %s\n", _url);

    emscripten_wget_data(_url, &_dataBuffer, &_bufferSize, &error);
    printf("retrieved %d bytes, error %d\n", _bufferSize, error);

    if (error == 0)
    {
        return 200;
    }
    else
    {
        return 404;
    }
}

int HTTPClient::getSize()
{
    printf("getSize %d\n", _bufferSize);
    return _bufferSize;
}

bool HTTPClient::connected()
{
    return true;
}

void HTTPClient::end()
{
    if (_url)
    {
        free(_url);
        _url = NULL;
    }
}

WiFiClient* HTTPClient::getStreamPtr()
{
    _wifiClient->setData(_dataBuffer, _bufferSize);
    return _wifiClient;
}

string HTTPClient::errorToString(int code)
{
    return string("error test");
}

void ConnectWifi(const char* ssid, const char* password) 
{
    printf("FakeWifi Connected: %s %s\n", ssid, password);
}