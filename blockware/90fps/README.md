# 90 FPS

A demo of how to squeeze every last bit of graphics performance out of your block by making better use of the hardware SPI APIs in the microcontroller.

While the Adafruit GFX Lirary is great, it sometimes uses the lowest-common-denominator for APIs. This demo shows how to modify that library to get full speed from the SPI interface.

TODO: An enterprising soul could probably figure out a way to build this permanently into the Adafruit GFX library and benefit everyone using an ESP12 chip with this library.

## Hacking the graphics library

First build this repository at least once (see instructinos in the Blockware top-level README.md). This will download the libraries. Then you need to modify the libraries in place. To see the before/after results you might as well go ahead and deploy to the block. Then you'll need to make the SPI change in the library.

First find `Adafruit_SPITFT.cpp` using one of two methods:

### Using VSCode + PlatformIO:

- From `Platform IO: Home` select `Libraries` from the left menu
- Click the `Installed` tab to show installed libraries
- Click `Reveal` under `Adafruit GFX Library`

### Or using the command line

- From this directory run `find . -iname "Adafruit_SPITFT.cpp"`

### Then modify `Adafruit_SPITFT.cpp`

- Open `Adafruit_SPITFT.cpp` in your favorite editor
- Replace `while` loop at `L1076`
```cpp
// Removing while loop for massive FPS boost
// while(len--) {
//     SPI_WRITE16(*colors++);
// }
// Write bytes directly all at once
hwspi._spi->writeBytes((uint8_t*)colors, len*2);
```

Now re-deploy to your block and the line should move much faster.
