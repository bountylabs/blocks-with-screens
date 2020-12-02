# Blocks With Screens PCB Files

## Components

The blocks are made up of 3 components.

### OLED Board

![OLED Back](https://user-images.githubusercontent.com/218876/99321449-b116e980-2822-11eb-9ae4-aa685a7b6b91.png) ![OLED Front](https://user-images.githubusercontent.com/218876/99321454-b1af8000-2822-11eb-80dd-8d2bca46f54a.png)

This board is responsible for all the circuitry to drive a SSD1351 OLED display. This design is heavily based on [Adafruit's breakout board](https://github.com/adafruit/Adafruit-1.5inch-Color-OLED-PCB) for the same display.

It consists of a 13V boost regulator to provide the required voltage to drive the OLED, as well as necessary passive components required by the OLED's datasheet.

### Microcontroller Board

![Microcontroller Back](https://user-images.githubusercontent.com/218876/99321508-d9064d00-2822-11eb-97be-c0dcf3f74503.png) ![Microcontroller Front](https://user-images.githubusercontent.com/218876/99321512-d99ee380-2822-11eb-9604-5819c3f5eadf.png)

This board is a mounting point for the ESP12 microcontroller and the MMA8452Q 3-axis accelerometer. It also has unpopulated pads for a [SparkFun Qwiic](https://www.sparkfun.com/qwiic) connector and a WS2812B RGB LED if you want to add additional functionality.

### USB Board

![USB Back](https://user-images.githubusercontent.com/218876/99321787-7c576200-2823-11eb-9ce4-1becdb05f49a.png) ![USB Front](https://user-images.githubusercontent.com/218876/99321789-7ceff880-2823-11eb-97ea-6fdcfcb138af.png)

This board is responsible for:
* Providing a USB serial interface for programming the microcontroller (with automatic reset)
* Providing regulated 3.3V power to the Microcontroller and OLED
* Providing charging circuitry to charge the LiPo battery

This board is based on various Adafruit [LiPo charging boards](https://learn.adafruit.com/adafruit-pro-trinket-lipoly-slash-liion-backpack) and [USB powered boards](https://learn.adafruit.com/introducing-trinket/downloads) with the simplest (by part count) USB-serial chip we could find.

## PCB Hacking

We hope you'll open up your blocks and add new functionality or boards we never even dreamed of.

The pin layout is compatible with the [Wemos D1 Mini](https://docs.wemos.cc/en/latest/d1/d1_mini.html) (available on your usual internet shopping sites) so any shields made for those should work!

There is un unpopulated [SparkFun Qwiic](https://www.sparkfun.com/qwiic) connector on the microcontroller board. If you solder on a Qwiic connector (just a 4-pin JST connector) you can connect your board to any of the dozens of [Qwiic compatible peripherals](https://www.sparkfun.com/qwiic#products) (e.g. sensors, screens, GPS) out there.

There's an unpopulated spot on the Microcontroller board for a WS2812B RGB LED (or similar). This might come in handy if you want to, say, cut a window in the side of your block and trick it out with colors inside. Note that these LEDs will typically initialize to a random color so if you add the LED, but don't use it in your program, it will probably be lit up to a random color.

You could also add PCBs for different microcontrollers, different screens, or maybe even make a USB-C version! This was meant to be modular. Have fun, and share back what you build!

## Manufacturing Boards

We used [OshPark](https://oshpark.com/) for creating our prototype PCBs which we then soldered by hand or in a [reflow oven](https://github.com/UnifiedEngineering/T-962-improvements). We used [PCBWay](https://www.pcbway.com/) to mass manufacture assembled PCBs.

There's an included [script](https://github.com/bountylabs/blocks-with-screens/blob/main/pcb/eagle_to_pcbway.py) for exporting the Eagle PCB files in a bill-of-materials (BOM) format that PCBWay accepts.

## Changelog

### USB Board V5

* Turn the battery connector counter-clockwise by 90 degrees to avoid wires hanging off the side of the board and not fitting in the block.

### OLED Board V4

* Revert back to using 3.3V for the 13V boost circuit to hopefully avoid issues I've seen twice now with the OLED screens developing bad vertical lines of pixels. This is what Adafruit does in their board; perhaps there's a reason for it.

### USB Board V4

* Use thicker (power) trace for 3.3V
* Make the USB side slots wider to comply with PCBWay
* Add a JST-SH connector (SM02B-SRSS-TB) for the battery

### Microcontroller Board V3

* Attach 3.3V to the ESP12 (doh!)

### OLED Board V3

* Uses `D4/GPIO2` for `OLED DC` instead of `D1/GPIO5` since that pin is I2C `SCK`.

### Microcontroller Board V2

* Fixed disconnected decoupling capacitor.
* Added optional header for MMA8452QR1 accelerometer.
* Added optional [Qwiic](https://www.sparkfun.com/qwiic) header for SparkFun accessories.
* Added optional [5050 NeoPixel](https://www.adafruit.com/product/1655) pad.

### USB Board V3

* Fix width of power trace going to the switch.

### OLED Board V2

* Uses `D1` for `OLED DC` instead of `D6` (`MISO`) so we don’t have to hack SPI.c for hardware SPI.
* Uses `VIN` for power for the 13V boost (possible this will reduce the flicker, especially during low battery).
* Removed the 3-pin SPI jumper since we didn’t use it.
* Move all the screen capacitors closer to the screen connector (even smaller chance of reducing flicker)

### USB Board V2

* Removed oscillator and move to using the `CH340C` instead of `CH340G` USB-UART chip.
* Connect `VCC` on the `CH340C` chip (fix error from V1).
* Move most parts to the front of the board for easier manufacturing.
* Add an optional on LED.
* Add pads for a switch.
