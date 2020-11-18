# Gif Download

An example of downloading a stream of binary 565 images and rendering them onto the OLED display. Also includes example code for using Arduino OTA for wireless programming.

This also depends on a server component to be deployed to convert a gif to a 565 image stream.

Keep in mind it can take a little while for the block to download the gif. Open up a serial terminal if you want to make sure something's happening!

## Configuring

Create a `secrets.h` file based on `secrets.h.example` that contains your WiFi credentials.

## Running the server

The server accepts a standard gif and converts it to the binary 565 format that's best for the block. See the server's README.md file for more info on running that locally.