# Blocks With Screens PCB Files

## Changelog

### Microcontroller Board V3

* Attach 3.3V to the ESP12 (doh!)

### USB Board V4

* Use thicker (power) trace for 3.3V
* Add a jumper to make the switch unnecessary by default
* Make the slots wider to comply with PCBWay

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

## USB Board V2

* Removed oscillator and move to using the `CH340C` instead of `CH340G` USB-UART chip.
* Connect `VCC` on the `CH340C` chip (fix error from V1).
* Move most parts to the front of the board for easier manufacturing.
* Add an optional on LED.
* Add pads for a switch.
