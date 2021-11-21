BLOCKWARE_DIR = ../blockware
STUBS_DIR = ./stubbed-libs
DEFINES = -DARDUINO=100
COMPILEWASM = em++
SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LDFLAGS := $(shell sdl2-config --libs)
CPPFLAGS += -std=c++17 $(SDL_CFLAGS) $(SDL_LDFLAGS) -I/usr/include -I/opt/homebrew/include -Wno-unused-command-line-argument
WASMFLAGS = -Wno-unused-command-line-argument

INCLUDE_DIRS = \
	-I${BLOCKWARE_DIR}/curvy-snake\
	-I${BLOCKWARE_DIR}/curvy-snake/lib/CurvySnake\
	-I${BLOCKWARE_DIR}/lib/Colors\
	-I${BLOCKWARE_DIR}/lib/Random\
	-I${BLOCKWARE_DIR}/curvy-snake/lib/Entity\
	-I${BLOCKWARE_DIR}/lib/Vec2d\
	-I${BLOCKWARE_DIR}/lib/DLog\
	-I${BLOCKWARE_DIR}/lib/Text\
	-I${BLOCKWARE_DIR}/lib/WifiHelper\
	-I${BLOCKWARE_DIR}/lib/DefaultConfig\
	-I${BLOCKWARE_DIR}/lib/OTAUpdates\
	-I${BLOCKWARE_DIR}/bouncing-logo/src\
	-I${BLOCKWARE_DIR}/twitter-text/lib/Particle\
	-I${BLOCKWARE_DIR}/fireworks/lib/Particle\
	-I${STUBS_DIR}

VBLOX_OBJECTS_SHARED = \
	${STUBS_DIR}/FakeWifiHelper.o\
	${STUBS_DIR}/SimHelpers.o\
	${STUBS_DIR}/Adafruit_GFX.o\
	${STUBS_DIR}/FakeFS.o\
	${STUBS_DIR}/FakeArduinoOTA.o\
	${STUBS_DIR}/FakeOTAUpdates.o\
	${STUBS_DIR}/JPEGDecoder.o
VBLOX_OBJECTS_NATIVE = $(VBLOX_OBJECTS_SHARED) native.o
VBLOX_OBJECTS_WASM = \
	$(VBLOX_OBJECTS_SHARED:.o=.wasm.o) \
	wasm.wasm.o

CURVY_SNAKE_OBJECTS = \
	${BLOCKWARE_DIR}/curvy-snake/lib/CurvySnake/CurvySnake.o\
	${BLOCKWARE_DIR}/curvy-snake/lib/Entity/Entity.o\
	${BLOCKWARE_DIR}/curvy-snake/src/main.o
CURVY_SNAKE_OBJECTS_WASM = $(CURVY_SNAKE_OBJECTS:.o=.wasm.o)

JPEG_DOWNLOAD_EXAMPLE_OBJECTS = ${BLOCKWARE_DIR}/jpeg-download-example/src/main.o
JPEG_DOWNLOAD_EXAMPLE_OBJECTS_WASM = $(JPEG_DOWNLOAD_EXAMPLE_OBJECTS:.o=.wasm.o)

BOUNCING_LOGO_OBJECTS = ${BLOCKWARE_DIR}/bouncing-logo/src/main.o
BOUNCING_LOGO_OBJECTS_WASM = $(BOUNCING_LOGO_OBJECTS:.o=.wasm.o)

TWITTER_TEXT_OBJECTS = \
	${BLOCKWARE_DIR}/twitter-text/src/main.o \
	${BLOCKWARE_DIR}/twitter-text/lib/Particle/Particle.o
TWITTER_TEXT_OBJECTS_WASM = $(TWITTER_TEXT_OBJECTS:.o=.wasm.o)

FIREWORKS_OBJECTS = \
	${BLOCKWARE_DIR}/fireworks/src/main.o \
	${BLOCKWARE_DIR}/fireworks/lib/Particle/Particle.o
FIREWORKS_OBJECTS_WASM = $(FIREWORKS_OBJECTS:.o=.wasm.o)

ALL_OBJECTS = \
	$(VBLOX_OBJECTS_NATIVE) \
	$(CURVY_SNAKE_OBJECTS) \
	$(JPEG_DOWNLOAD_EXAMPLE_OBJECTS) \
	$(BOUNCING_LOGO_OBJECTS) \
	$(TWITTER_TEXT_OBJECTS) \
	$(FIREWORKS_OBJECTS)
ALL_OBJECTS_WASM = $(ALL_OBJECTS:.o=.wasm.o)

ALL_TARGETS = \
	curvy-snake \
	curvy-snake.html \
	bouncing-logo \
	bouncing-logo.html

curvy-snake: $(VBLOX_OBJECTS_NATIVE) $(CURVY_SNAKE_OBJECTS)
	$(CXX) $(VBLOX_OBJECTS_NATIVE) $(CURVY_SNAKE_OBJECTS) $(CPPFLAGS) -o $@

curvy-snake.html: $(VBLOX_OBJECTS_WASM) $(CURVY_SNAKE_OBJECTS_WASM)
	$(COMPILEWASM) $(WASMFLAGS) --bind $(VBLOX_OBJECTS_WASM) $(CURVY_SNAKE_OBJECTS_WASM) -o $@

# Not working right now
# jpeg-download-example: $(VBLOX_OBJECTS_NATIVE) $(JPEG_DOWNLOAD_EXAMPLE_OBJECTS)
# 	$(CXX) $(VBLOX_OBJECTS_NATIVE) $(JPEG_DOWNLOAD_EXAMPLE_OBJECTS) $(CPPFLAGS) -o $@

# jpeg-download-example.html: $(VBLOX_OBJECTS_WASM) $(JPEG_DOWNLOAD_EXAMPLE_OBJECTS_WASM)
# 	$(COMPILEWASM) $(WASMFLAGS) --bind $(VBLOX_OBJECTS_WASM) $(JPEG_DOWNLOAD_EXAMPLE_OBJECTS_WASM) -s ASYNCIFY -o $@

bouncing-logo: $(VBLOX_OBJECTS_NATIVE) $(BOUNCING_LOGO_OBJECTS)
	$(CXX) $(VBLOX_OBJECTS_NATIVE) $(BOUNCING_LOGO_OBJECTS) $(CPPFLAGS) -o $@

bouncing-logo.html: $(VBLOX_OBJECTS_WASM) $(BOUNCING_LOGO_OBJECTS_WASM)
	$(COMPILEWASM) $(WASMFLAGS) --bind $(VBLOX_OBJECTS_WASM) $(BOUNCING_LOGO_OBJECTS_WASM) -o $@

fireworks: $(VBLOX_OBJECTS_NATIVE) $(FIREWORKS_OBJECTS)
	$(CXX) $(VBLOX_OBJECTS_NATIVE) $(FIREWORKS_OBJECTS) $(CPPFLAGS) -o $@

fireworks.html: $(VBLOX_OBJECTS_WASM) $(FIREWORKS_OBJECTS_WASM)
	$(COMPILEWASM) $(WASMFLAGS) --bind $(VBLOX_OBJECTS_WASM) $(FIREWORKS_OBJECTS_WASM) -o $@

%.o: %.cpp
	$(CXX) -c $< $(INCLUDE_DIRS) -L. $(DEFINES) $(CPPFLAGS) -o $@

%.wasm.o: %.cpp
	$(COMPILEWASM) $(WASMFLAGS) --bind -c $< $(INCLUDE_DIRS) $(DEFINES) -o $@

all: $(ALL_TARGETS)

clean:
	rm -f $(ALL_OBJECTS) $(ALL_OBJECTS_WASM) wasm.wasm.o
	rm -f $(ALL_TARGETS)
	rm -rf *.dSYM
	rm -f *.wasm *.js *.html
