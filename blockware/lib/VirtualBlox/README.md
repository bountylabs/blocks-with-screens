# VirtualBlox

<img width="368" alt="Screen Shot 2021-11-23 at 4 39 33 PM" src="https://user-images.githubusercontent.com/218876/143145863-e977eede-6408-4976-b632-aada3ba01758.png">

A system for cross-compiling blockware to run natively on a desktop computer using SDL2 or in a web browser using SDL1.2+WASM.

## Running natively

To build the native version you'll need SDL2 installed. On macOS you can install it with Homebrew (or `apt install libsdl2-dev` on some Linux distros):

```bash
brew install SDL2
```

Native blockware builds can be added to a blockware's `platformio.ini` file. See the [`curvy-snake/platformio.ini`](https://github.com/bountylabs/blocks-with-screens/blob/main/blockware/curvy-snake/platformio.ini) as an example. You can then run the native blockware straight from VSCode by changing the target to `VirtualBlox` and hitting the 'upload' button:

<img width="863" alt="image" src="https://user-images.githubusercontent.com/218876/143146174-a35335f2-7edb-4c5b-a7ab-1b58591f5fb1.png">

Alternately you can run it from the commandline with:

```bash
pio run -e VirtualBlox -t upload
```

## Running the web-assembly builds

To build the WASM version, you'll need to install emscripten:

```bash
brew install emscripten
```

See the `VirtualBlox/Makefile` to see which blockwares are supported. For example you could do the following to run the curvy-snake example.

```bash
make curvy-snake.html
```

Then you can run a webserver (alternately `npx http-server -p 8080` or `php -S localhost:8080`)

```bash
python3 -m http.server 8080 &
```

And then open the wasm at http://localhost:8080/curvy-snake.html

## Supporting new blockwares

VirtualBlox contains a collection of stub classes and functions (in the `VirtualBlox/stubs` directory) to simulate enough Arduino stuff to satisfy the compiler. To work with new blockwares, it's very likely you'll have to add new stubbed libraries.

To add a new blockware, start by copying in the VirtualBlox environment from a known-supported blockware's `platformio.ini` (e.g. [`curvy-snake/platformio.ini`](https://github.com/bountylabs/blocks-with-screens/blob/main/blockware/curvy-snake/platformio.ini)). Then try to build the VirtualBlox environment and see what fails. Add new classes to `VirtualBlox/stubs` as necessary until you're able to get things to build!
