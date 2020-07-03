# pixel-dust

Demo that uses the onboard accelerometer along with [Adafruit's Pixel Dust library](https://github.com/adafruit/Adafruit_PixelDust) to generate a physics demo where particles move with the direction of gravity.

## Getting set up for development

1. Download [VSCode](https://code.visualstudio.com/)
1. Install the [PlatformIO IDE](http://docs.platformio.org/en/latest/ide/vscode.html) plugin for VSCode
1. If programming over USB/serial, change the `upload_port` line in the [platformio.ini](https://github.com/johnboiles/EspHeart/blob/master/platformio.ini) file to match your local serial port (something like `/dev/cu.usbserial123`)
