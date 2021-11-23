# VirtualBlox

A system for cross-compiling blockware to run natively on a desktop computer using SDL2 or in a web browser using SDL1.2+WASM.

# Dependencies

To build the native version you'll need SDL2 installed. On macOS you can install it with Homebrew:

```bash
brew install SDL2
```

To build the WASM version, youj'll need to install emscripten:

```bash
brew install emscripten
```

# Running

See the `Makefile` to see which blockwares are support. For example you could do the following to run the curvy-snake example.

```bash
make curvy-snake
./curvy-snake
```

To test the wasm examples you can do.

```bash
make curvy-snake.html
```

Then you can run a webserver (alternately `npx http-server -p 8080` or `php -S localhost:8080`)

```bash
python3 -m http.server 8080 &
```

And then open the wasm at http://localhost:8080/curvy-snake.html
