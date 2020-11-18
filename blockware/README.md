# Blockware

A collection of programs meant to run on the blocks.

## App descriptions

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
