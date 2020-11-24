# Video Display

This is an advanced example of streaming uncompressed video over UDP to the block. It consists of a client program running on a computer that sends the video and a server program running on the block that displays the video on its screen.

# Running this Blockware

This Blockware has two components -- a UDP server running on the block, and a UDP sender running on your computer (on the same network). You'll need to install `ffmpeg` (`brew install ffmpeg` on macOS using Homebrew) to convert videos to the proper format for the block. You'll also need a video file handy to send to the block.

## Running the Server (on the block)

Build and upload the code for the Blockware as detailed in the [Blockware README.md](https://github.com/bountylabs/blocks-with-screens/blob/main/blockware/README.md). Make sure to create a `src/secrets.h` based on the `src/secrets.h.example` and edit to include your WiFi credentials.

After programming your block should connect to your WiFi then show an IP address on the screen. Take note of the IP for the next step.

## Running the client (on your computer)

Compile the client by running

```bash
make client/client
```

Then you can use `ffmpeg` to convert a video file into the block-friendly 565 pixel format and send it using the client to the IP of your block. Replace `10.0.0.150` with the IP of your block and `/path/to/input/video.mp4` with the path to your video file.

```bash
ffmpeg -i /path/to/input/video.mp4 -vf scale=128x128 -f rawvideo -pix_fmt rgb565 - | ./client/client 10.0.0.150
```

You should now see your video on your block! You can also probably stream your webcam with something like this (this is for macOS, but you can probably do something similar with ffmpeg's video4linux support on Linux).

```bash
ffmpeg -f avfoundation -framerate 10 -video_device_index 0 -i ":0" -vf scale=128x128 -f rawvideo -pix_fmt rgb565 - | ./client/client 10.0.0.138
```

If you happen to have multiple blocks for some reason you can send video to multiple blocks at once using `tee`:

```bash
ffmpeg -i /path/to/input/video.mp4 -vf scale=128x128 -f rawvideo -pix_fmt rgb565 - | tee >(./client/client 10.0.0.150) >(./client/client 10.0.0.138)
```
