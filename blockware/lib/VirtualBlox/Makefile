# Makefile for building web assembly versions of blockware

BLOCKWARE_DIR = ../../
STUBS_DIR = ./stubs
DEFINES = -DARDUINO=100
COMPILEWASM = em++
WASMFLAGS = -Wno-unused-command-line-argument -s USE_SDL=2

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
	-I${BLOCKWARE_DIR}/fireworks/lib/Particle\
	-I${STUBS_DIR}

VBLOX_OBJECTS_WASM = \
	${STUBS_DIR}/FakeWifiHelper.wasm.o\
	${STUBS_DIR}/SimHelpers.wasm.o\
	${STUBS_DIR}/Adafruit_GFX.wasm.o\
	${STUBS_DIR}/FakeFS.wasm.o\
	${STUBS_DIR}/FakeArduinoOTA.wasm.o\
	${STUBS_DIR}/FakeOTAUpdates.wasm.o\
	${STUBS_DIR}/JPEGDecoder.wasm.o\
	wasm.wasm.o

CURVY_SNAKE_OBJECTS_WASM = \
	${BLOCKWARE_DIR}/curvy-snake/lib/CurvySnake/CurvySnake.wasm.o\
	${BLOCKWARE_DIR}/curvy-snake/lib/Entity/Entity.wasm.o\
	${BLOCKWARE_DIR}/curvy-snake/src/main.wasm.o

JPEG_DOWNLOAD_EXAMPLE_OBJECTS_WASM = ${BLOCKWARE_DIR}/jpeg-download-example/src/main.wasm.o

BOUNCING_LOGO_OBJECTS_WASM = ${BLOCKWARE_DIR}/bouncing-logo/src/main.wasm.o

FIREWORKS_OBJECTS_WASM = \
	${BLOCKWARE_DIR}/fireworks/src/main.wasm.o \
	${BLOCKWARE_DIR}/fireworks/lib/Particle/Particle.wasm.o

ALL_OBJECTS = \
	$(VBLOX_OBJECTS_WASM) \
	$(CURVY_SNAKE_OBJECTS_WASM) \
	$(JPEG_DOWNLOAD_EXAMPLE_OBJECTS_WASM) \
	$(BOUNCING_LOGO_OBJECTS_WASM) \
	$(FIREWORKS_OBJECTS_WASM)

ALL_TARGETS = \
	curvy-snake.html \
	bouncing-logo.html

curvy-snake.html: $(VBLOX_OBJECTS_WASM) $(CURVY_SNAKE_OBJECTS_WASM)
	$(COMPILEWASM) $(WASMFLAGS) --bind $(VBLOX_OBJECTS_WASM) $(CURVY_SNAKE_OBJECTS_WASM) -o $@

# Not working right now
# jpeg-download-example.html: $(VBLOX_OBJECTS_WASM) $(JPEG_DOWNLOAD_EXAMPLE_OBJECTS_WASM)
# 	$(COMPILEWASM) $(WASMFLAGS) --bind $(VBLOX_OBJECTS_WASM) $(JPEG_DOWNLOAD_EXAMPLE_OBJECTS_WASM) -s ASYNCIFY -o $@

bouncing-logo.html: $(VBLOX_OBJECTS_WASM) $(BOUNCING_LOGO_OBJECTS_WASM)
	$(COMPILEWASM) $(WASMFLAGS) --bind $(VBLOX_OBJECTS_WASM) $(BOUNCING_LOGO_OBJECTS_WASM) -o $@

# Not working right now. Needs stubs for the accelerometer
# fireworks.html: $(VBLOX_OBJECTS_WASM) $(FIREWORKS_OBJECTS_WASM)
# 	$(COMPILEWASM) $(WASMFLAGS) --bind $(VBLOX_OBJECTS_WASM) $(FIREWORKS_OBJECTS_WASM) -o $@

%.wasm.o: %.cpp
	$(COMPILEWASM) $(WASMFLAGS) --bind -c $< $(INCLUDE_DIRS) $(DEFINES) -o $@

all: $(ALL_TARGETS)

clean:
	rm -f $(ALL_OBJECTS) wasm.wasm.o
	rm -f $(ALL_TARGETS)
	rm -rf *.dSYM
	rm -f *.wasm *.js *.html
