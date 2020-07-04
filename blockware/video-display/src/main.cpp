/*************************************************** 
  Example sketch for the Block with Screen
 ****************************************************/

//#define USE_TCP 1
#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "secrets.h"
#include <DefaultConfig.h>

// NETWORK STUFF
IPAddress local_ip(192,168,4,22);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255, 255, 255, 0);

#ifdef USE_TCP
WiFiServer wifiServer(80);
#else
WiFiUDP udp;
unsigned int localUdpPort = 4210;
char incomingPacket[1025];
#endif

Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, CS_PIN, DC_PIN, RST_PIN);
uint8_t image[128*128*2];

int expectedPacket;

void joinNetwork()
{
  WiFi.begin(ssid, password);
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  int i = 0;
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
    Serial.print(++i); Serial.print(' ');
  }
  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());         // Send the IP address of the ESP8266 to the computer
}

void setup(void) {
  Serial.begin(SERIAL_DATA_RATE);
  Serial.print("setup");
  // 15MHz SPI
  tft.begin(SPI_SPEED);

  joinNetwork();

  #ifdef USE_TCP
  wifiServer.begin();
  Serial.printf("TCP server listening IP %s, TCP port %d\n", WiFi.localIP().toString().c_str(), 8888);
  #else
  udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
  #endif
  expectedPacket = 0;
}

#ifdef USE_TCP
void loop() {
  WiFiClient client = wifiServer.available();
  if (client) {
    while (client.connected()) {
      Serial.printf("connected!\n");
      uint8_t* imgptr = image;
      int bytesLeft = 128*128*2;
      while (client.available() > 0) {
        size_t bytesRead = client.readBytes(imgptr, bytesLeft);
        bytesLeft -= bytesRead;
        imgptr += bytesRead;
        if (bytesLeft == 0)
        {
          Serial.printf("got frame!\n");
          imgptr = image;
          bytesLeft = 128*128*2;
          tft.drawRGBBitmap(0, 0, (uint16_t*)image, 128, 128);
        }
      }
    }
  }
}

#else
void loop() {
  int packetSize = udp.parsePacket();
  if (packetSize)
  {
    int len = udp.read(incomingPacket, 1025);
    if (len > 0)
    {
      //Serial.printf("packet %d\n", incomingPacket[0]);
      int position = incomingPacket[0] * 1024;
      memcpy(&image[position], &incomingPacket[1], 1024);

      if (incomingPacket[0] != expectedPacket)
      {
        Serial.printf("missing packet %d\n", expectedPacket);
      }
      
      if (incomingPacket[0] == 31)
      {
        //Serial.printf("got frame!");
        tft.drawRGBBitmap(0, 0, (uint16_t*)image, 128, 128);
        expectedPacket = 0;
      }
      else
      {
        expectedPacket = incomingPacket[0]+1;
      }
    }
  }

  yield();
}
#endif