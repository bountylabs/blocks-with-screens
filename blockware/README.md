# Blockware

A collection of programs meant to run on the blocks.

## Table of contents
- [Getting Started](#getting-started)
- [PlatformIO CLI](#using-the-platformio-cli)
- [Protips](#protips)
- [Demos](#demo-blockwares)

## Getting Started

All of these blockwares are configured to be compiled and uploaded using [PlatformIO](https://platformio.org/). You can probably also use the Arduino IDE or any other tool that works to compile and upload Arduino-compatible code with ESP8266 processors.

The easiest way to get started is using **[Visual Studio Code (VSCode)](https://code.visualstudio.com)** with the **[PlatformIO extension](https://platformio.org/install/ide?install=vscode)**.

1. Install **VSCode** either from the [website](https://code.visualstudio.com/) or (on macOS) using Homebrew `brew cask install visual-studio-code`.
2. With VSCode open select **View -> Extensions** and search for `PlatformIO IDE`.
3. Click the extension and click **Install**.

    ![PlatformIO IDE Instructions](https://user-images.githubusercontent.com/218876/99602553-86f63080-29b6-11eb-854a-f876df2631f7.png)

4. `File -> Open Workspace...` and select the `workspace.code-workspace` file in one of the blockware folders (e.g. `blockware/curvy-snake`)

    **_NOTE_**: In the workspace, you should have the **PlatformIO actions** available at the bottom of your VSCode

    <img width="413" alt="VSCode PlatformIO Toolbar" src="https://user-images.githubusercontent.com/218876/99601884-2fa39080-29b5-11eb-8d12-8d2f14bf3690.png">

  5. Connect your block to the computer using a Micro USB cable
  6. Turn it on using the switch in the back
  7. Press the **Upload** button (looks like a right arrow) in the Platform IO actions (bottom of your VSCode)

     NOTE: The program should compile and then upload to your block. It can also be handy to use the Serial Monitor to view any logs from the program that are output over the USB serial port.

     If the upload fails with an error message like `Error: Please specify upload_port for environment` or `Timed out waiting for packet header`, then you may need to edit the [`platformio.ini`](https://github.com/bountylabs/blocks-with-screens/blob/main/blockware/curvy-snake/platformio.ini) in the blockware's directory to set a specific `upload_port`. Look in `/dev/` for a `tty` device named something like `usbserial` or `ttyUSB` or `wchusbserial` and change `upload_port` to match the path to that device exactly.

  8. That's it, you should see the demo running on your block!

     For more info on using the PlatformIO VSCode extension check out the [PlatformIO Quick Start guide](https://docs.platformio.org/en/latest/integration/ide/vscode.html#quick-start).

## Using the PlatformIO CLI

You can alternately use the PlatformIO CLI if that's more your style. Follow one of the [installation methods on the PlatformIO site](https://platformio.org/install/cli). As of this writing, they recommend using their installer script:

```bash
python3 -c "$(curl -fsSL https://raw.githubusercontent.com/platformio/platformio/master/scripts/get-platformio.py)"
```

The script will probably instruct you to add something to your `PATH` in order to use the `pio` binary. Something like the following (which you could add to your `.bashrc` / `.zshrc`)

```bash
export PATH=$HOME/.platformio/penv/bin:$PATH
```

You can now build and upload blockware from the command line like this:

```bash
cd blockware/curvy-snake
pio run --target upload
```

Or if you want to override the `upload_port` (e.g. for blockwares that support WiFi OTA programming)

```bash
cd blockware/gif-download
pio run --target upload --upload-port gif-example.local
```

## Protips

#### Serial Monitor Reset

In the VSCode PlatformIO Serial Monitor, you can quickly restart the block by pressing `Ctrl-T Ctrl-D` twice. This toggles the `DTR` pin on the USB-serial chip and that pin is connected to the `RST` pin on the microcontroller.

#### Logging using `Dlog.h`

If using the `DLog` library for logging over the serial monitor or telnet you need to add `#define SERIALLOG 1` or `#define TELNETLOG 1` respectively before including `DLog.h` to enable serial logging. For example, for logging to the serial port:

```C
#define SERIALLOG 1
#include <DLog.h>
```

If using `TELNETLOG` then from your computer you can do something like the following to get the logs to your terminal over the network.

```bash
telnet homeassistant-display.local
```

## Demo Blockwares

This repo contains a handful of example blockwares you could run directly or use as a starting point to make your own blockware! Please share back any neat things you make so that others can benefit from it!

### 90fps

![90fps](https://user-images.githubusercontent.com/218876/99465118-7b3f3700-28ee-11eb-94da-636e3e2f11c1.gif)

A demo of how to squeeze every last bit of graphics performance out of your block by making better use of the hardware SPI APIs in the microcontroller. The interesting bits are in the [README.md](https://github.com/bountylabs/blocks-with-screens/tree/main/blockware/90fps#hacking-the-graphics-library).

### Adafruit Example

![adafruit-example](https://user-images.githubusercontent.com/218876/99464803-cf95e700-28ed-11eb-87c7-8762d6e9e07f.gif)

A fork of Adafruit's [SSD1351 OLED example code](https://github.com/adafruit/Adafruit-SSD1351-library/blob/master/examples/test/test.ino) with the `delay`s removed. Useful as an overview of the available graphics APIs in the Adafruit GFX library or as a graphics benchmark when tweaking the Adafruit GFX Library code (as detailed in the [90fps README.md](https://github.com/bountylabs/blocks-with-screens/tree/main/blockware/90fps#hacking-the-graphics-library)).

### Bouncing Logo

![Bouncing Logo](https://user-images.githubusercontent.com/218876/99464704-93fb1d00-28ed-11eb-9b47-bea17df132fd.gif)

The most 90s-est demo we've got. Bounce the Twitter logo around your screen just like your DVD player did to prevent burn-in on your CRT TV in 1999.

### Curvy Snake

![curvy-snake](https://user-images.githubusercontent.com/218876/99465293-e983f980-28ee-11eb-9117-dd2728ebf45f.gif)

It's kinda like those old pipes screensavers, or snake with curves. Watch randomly generated curves snake around your block's screen.

### Gif Download

<img alt="Gif Download" src="https://user-images.githubusercontent.com/218876/99469032-c78e7500-28f6-11eb-89ca-39b789fa7b69.gif"><img alt="Server Interface" src="https://user-images.githubusercontent.com/218876/99469116-e42aad00-28f6-11eb-905b-9868b231443c.jpeg" height="300">


This demo shows a QR code that takes you to a web page where you can upload a gif, have it translated to a block-friendly format, then the block can download it and begin to display it.

Requires a Node.js-based server component which you can find in in the `gif-download/server` directory.

### Homeassistant Display

![Home Assistant Display](https://user-images.githubusercontent.com/218876/99464995-3c10e600-28ee-11eb-9a1c-7cb2d36f5040.jpg)

An example of a program that can connect to a [Home Assistant](https://www.home-assistant.io/) home automation interface and display sensor values on the screen.

### JPEG Download Example

![JPEG Download](https://user-images.githubusercontent.com/218876/99464878-07049380-28ee-11eb-974f-b514e6732e7b.JPG)

An example that downloads a JPEG and displays it on the block.

### Pixel Dust

![pixel-dust](https://user-images.githubusercontent.com/218876/99464446-05869b80-28ed-11eb-9feb-f6383f0c54b5.gif)

An example that uses the [Adafruit Pixel Dust](https://github.com/adafruit/Adafruit_PixelDust) to create animated dust that interacts with the accelerometer and a static Twitter Logo.

### RGB LED

![rgb-led](https://user-images.githubusercontent.com/218876/99465399-2f40c200-28ef-11eb-95ad-9ac03f76da1b.gif)

An example that uses the (normally not-included) internal RGB LED. This requires modification of the PCB by soldering on an RGB LED. This is probably not the blockware you're looking for.

### Snake

<img alt="Snake intro" src="https://user-images.githubusercontent.com/218876/99464084-200c4500-28ec-11eb-8e72-86c5eae3395f.gif"><img alt="Gameplay" src="https://user-images.githubusercontent.com/218876/99464241-83967280-28ec-11eb-9b92-48f0d2924c97.gif"><img alt="User Interface" src="https://user-images.githubusercontent.com/218876/99464142-429e5e00-28ec-11eb-8148-25e78940c3be.jpeg" height="311">

The game of snake! This demo creates a WiFi access point that you can connect to on your phone to control the snake.

### Stocks and Weather

<img alt="Stocks and Weather" src="https://user-images.githubusercontent.com/218876/99464537-3e267500-28ed-11eb-8335-58b114818232.png">

An example that downloads stocks and weather.

### Video Display

An example of real-time video streaming from your computer to your block.

<a href="https://twitter.com/samhaves/status/1177639088540635141"><img width="303" alt="Blockware Live Video Demo" src="https://user-images.githubusercontent.com/290084/99889518-ea789c00-2c0a-11eb-8a01-a325acf3eb22.png"></a>
