# curvy-snakes

## Getting set up for development

1. Download [VSCode](https://code.visualstudio.com/)
1. Install the [PlatformIO IDE](http://docs.platformio.org/en/latest/ide/vscode.html) plugin for VSCode
1. If programming over USB/serial, change the `upload_port` line in the [platformio.ini](https://github.com/johnboiles/EspHeart/blob/master/platformio.ini) file to match your local serial port (something like `/dev/cu.usbserial123`)

_*Note*_: In order to get block with screen pin layout to work you must modify `SPI/SPI.cpp`

```sh
code /Users/$(whoami)/.platformio/packages/framework-arduinoespressif8266/libraries/SPI/SPI.cpp
```

Comment out the lines containing `pinMode(MISO` on `L77` and `L93`

## Hacks

### 90 FPS

- From `Platform IO: Home` select `Libraries` from the left menu
- Click the `Installed` tab to show installed libraries
- Click `Reveal` under `Adafruit GFX Library`
- Modify `Adafruit GFX Library_ID13/Adafruit_SPITFT.cpp`
- Replace `while` loop at `L1076`

```cpp
// Removing while loop for massive FPS boost
// while(len--) {
//     SPI_WRITE16(*colors++);
// }
// Write bytes directly all at once
hwspi._spi->writeBytes((uint8_t*)colors, len*2);
```
