#include "Adafruit_SSD1351.h"
#include "ArduinoOTA.h"
#include <chrono>
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>

using namespace std::chrono;
milliseconds start_time_ms;

FakeArduinoOTA ArduinoOTA;

Adafruit_SSD1351::Adafruit_SSD1351(uint16_t width, uint16_t height,
  void *spi, int8_t cs_pin, int8_t dc_pin, int8_t rst_pin)
: _width(width)
, _height(height)
{

}

void Adafruit_SSD1351::setTextColor(int16_t color)
{

}

void Adafruit_SSD1351::println(char* str)
{
    this->printf(str);
    this->printf("\n");
}

void Adafruit_SSD1351::print(const char* str)
{
    this->printf(str);
    this->printf("\n");
}

int Adafruit_SSD1351::printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = 0;
    ret = vprintf(format, args);
    va_end(args);
    return ret;
}

int Adafruit_SSD1351::width() {
    return _width;
}

int Adafruit_SSD1351::height() {
    return _height;
}

void Adafruit_SSD1351::startWrite()
{

}

void Adafruit_SSD1351::endWrite()
{

}

void Adafruit_SSD1351::setAddrWindow(int x, int y, int w, int h)
{

}

void Adafruit_SSD1351::writePixels(uint16_t* pixels, int num_pixels)
{

}

void Adafruit_SSD1351::fillScreen(uint16_t color)
{

}

void Adafruit_SSD1351::setCursor(int x, int y)
{

}

void Adafruit_SSD1351::setTextWrap(bool wrap)
{

}

void Adafruit_SSD1351::setTextSize(int size)
{

}

void Adafruit_SSD1351::begin(uint32_t freq) 
{
    printf("tft begin: %d\n", freq);
}

void FakeSerial::begin(int rate) 
{
    this->printf("rate is %d\n", rate);
}
int FakeSerial::printf(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = 0;
    ret = vprintf(format, args);
    va_end(args);
    return ret;
}

int FakeSerial::print(const char* str)
{
    this->printf(str);
    this->printf("\n");
    return 0;
}

int FakeSerial::println()
{
    printf("\n");
    return 0;
}

int FakeSerial::println(const char* str)
{
    this->printf(str);
    return 0;
}

int FakeSerial::write(uint8_t* data, int bytes)
{
    this->printf("write called %d bytes\n", bytes);
    return 0;
}

void yield()
{

}

void delay(int delay_ms)
{
}

void millis_start()
{
    start_time_ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
}

uint16_t millis()
{
    milliseconds now = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    milliseconds duration = now - start_time_ms;
    
    return (uint16_t)duration.count();
}

void randomSeed(uint32_t seed)
{

}

void displayReset()
{

}

int _sockfd;
struct sockaddr_in servaddr; 

// Driver code 
int open_udp() { 

    printf("opening udp\n");
    // Creating socket file descriptor 
    if ( (_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
  
    memset(&servaddr, 0, sizeof(servaddr)); 
      
    // Filling server information 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(9999); 
    inet_aton("127.0.0.1", (struct in_addr*)&servaddr.sin_addr.s_addr);


    int n, len; 
    return 0; 
} 