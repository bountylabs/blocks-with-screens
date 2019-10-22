# Blocks With Screens PCB Files

## Changelog

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
