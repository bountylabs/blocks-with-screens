/*************************************************** 
  Example sketch for the Block with Screen
 ****************************************************/

// Screen dimensions
#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 128 // Change this to 96 for 1.27" OLED.

// Pin setup
#define SCLK_PIN 14 // D5 -- ESP8266 Hardware SCLK
#define MOSI_PIN 13 // D7 -- ESP8266 Hardware MOSI
// Note this is also the ESP8266 Hardware MISO pin! That seems to be a bad thing that we should fix in V2.
// See the bit about the hack below
#define DC_PIN   12 // D6
#define CS_PIN   16 // D0
#define RST_PIN  15 // D8

// 15MHz SPI
// Note, the ESP8266 can supposedly can go up to 30MHz but seems to cause instability
#define SPI_SPEED 15000000

#include <Adafruit_SSD1351.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

// NETWORK STUFF
IPAddress local_ip(192,168,4,22);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255, 255, 255, 0);

WiFiUDP udp;
unsigned int localUdpPort = 4210;
char incomingPacket[1025];

const char* ssid = "Boiles";
const char* password = "stinaissohot";


// Software bit-banged SPI mode works just fine but is slow
// Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, CS_PIN, DC_PIN, MOSI_PIN, SCLK_PIN, RST_PIN);  

// Hardware SPI mode only works if you make a hack in SPI.c under the hood. In short, find the two
// lines with `pinMode(MISO` in them and comment them out
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
  Serial.begin(115200);
  Serial.print("setup");
  // 15MHz SPI
  tft.begin(15000000);

  joinNetwork();
  udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
  expectedPacket = 0;
}

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
