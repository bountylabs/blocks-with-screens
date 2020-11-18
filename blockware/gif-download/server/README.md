# Gif Download Server

This is the server component that converts a gif to the binary 565 format that is best for the blocks.

## Prerequisites

You'll need Node.js and Yarn installed to run this locally. On macOS this might be something like:

```bash
# Install Node Version Manager
brew install nvm
# Don't forget to add the necessary things to your .zshrc/.bashrc for NVM that Homebrew tells you to!
# Install Node
nvm install node
# Install yarn
curl -o- -L https://yarnpkg.com/install.sh | bash
```

## Running locally

To run this locally, you can run the following.

```
yarn
HTTP=1 yarn dev
```

Your server should now be availble at http://localhost:3000. Make sure to change the `serverURL` on the block (`main.cpp:174`) to point to your new server.

## Deploying

Left for reference, though you probably don't want to do this unless youre [@magus](github.com/magus).

```
# deploy a staging instance
yarn deploy
# deploy to production (https://blockswithscreens.now.sh)
yarn deploy:prod
```

## Debugging

URL containing RGB565 for Block 3

```
http://localhost:3000/api/gif?block=3
```

Convert RGB565 to PNG with FFMPEG

```bash
ffmpeg -vcodec rawvideo -f rawvideo -pix_fmt rgb565be -s 128x128 -i ~/Downloads/b3 -f image2 -vcodec png ~/Downloads/b3.png
```
