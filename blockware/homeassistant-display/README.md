# ha-display

An example of showing some Home Assistant temperature, humidity, and absolute humidity states on the block. Make sure to update your secrets.h, config.h, and adapt the sketch to your Home Assistant entities!

## Getting set up for development

1. Download [VSCode](https://code.visualstudio.com/)
1. Install the [PlatformIO IDE](http://docs.platformio.org/en/latest/ide/vscode.html) plugin for VSCode
1. If programming over USB/serial, change the `upload_port` line in the `platformio.ini` file to match your local serial port (something like `/dev/cu.wchusbserial123`)
