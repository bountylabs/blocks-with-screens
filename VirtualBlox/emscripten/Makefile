INCLUDE_DIRS = -I../../blockware/curvy-snake\
-I../../blockware/curvy-snake/lib/CurvySnake\
-I../../blockware/lib/Colors\
-I../../blockware/lib/Random\
-I../../blockware/curvy-snake/lib/Entity\
-I../../blockware/lib/Vec2d\
-I../../blockware/lib/DLog\
-I../../blockware/lib/Text\
-I../../blockware/lib/WifiHelper\
-I../../blockware/lib/DefaultConfig\
-I../../blockware/lib/OTAUpdates\
-I../../blockware/bouncing-logo/src\
-I../../blockware/twitter-text/lib/Particle\
-I../../blockware/fireworks/lib/Particle\
-I.

DEFINES = -DARDUINO=100

VBLOX_OBJECTS = FakeWifiHelper.o\
SimHelpers.o\
sim.o\
Adafruit_GFX.o\
FakeFS.o\
FakeArduinoOTA.o\
FakeOTAUpdates.o\
JPEGDecoder.o

CURVY_SNAKE_OBJECTS = ../../blockware/curvy-snake/lib/CurvySnake/CurvySnake.o\
../../blockware/curvy-snake/lib/Entity/Entity.o\
../../blockware/curvy-snake/src/main.o

JPEG_DOWNLOAD_EXAMPLE_OBJECTS = ../../blockware/jpeg-download-example/src/main.o

BOUNCING_LOGO_OBJECTS = ../../blockware/bouncing-logo/src/main.o

TWITTER_TEXT_OBJECTS = ../../blockware/twitter-text/src/main.o\
../../blockware/twitter-text/lib/Particle/Particle.o

FIREWORKS_OBJECTS = ../../blockware/fireworks/src/main.o\
../../blockware/fireworks/lib/Particle/Particle.o

COMPILECPP = ~/emsdk/upstream/emscripten/em++

curvy-snake.html: $(VBLOX_OBJECTS) $(CURVY_SNAKE_OBJECTS)
	$(COMPILECPP) --bind $(VBLOX_OBJECTS) $(CURVY_SNAKE_OBJECTS) -o $@

jpeg-download-example.html: $(VBLOX_OBJECTS) $(JPEG_DOWNLOAD_EXAMPLE_OBJECTS)
	$(COMPILECPP) --bind $(VBLOX_OBJECTS) $(JPEG_DOWNLOAD_EXAMPLE_OBJECTS) -s ASYNCIFY -o $@

bouncing-logo.html: $(VBLOX_OBJECTS) $(BOUNCING_LOGO_OBJECTS)
	$(COMPILECPP) --bind $(VBLOX_OBJECTS) $(BOUNCING_LOGO_OBJECTS) -o $@

twitter-text.html: $(VBLOX_OBJECTS) $(TWITTER_TEXT_OBJECTS)
	$(COMPILECPP) --bind $(VBLOX_OBJECTS) $(TWITTER_TEXT_OBJECTS) -o $@

fireworks.html: $(VBLOX_OBJECTS) $(FIREWORKS_OBJECTS)
	$(COMPILECPP) --bind $(VBLOX_OBJECTS) $(TWITTER_TEXT_OBJECTS) -o $@

.cpp.o:
	$(COMPILECPP) --bind -c $< $(INCLUDE_DIRS) $(DEFINES) -o $@

clean:
	rm $(VBLOX_OBJECTS) $(CURVY_SNAKE_OBJECTS) $(JPEG_DOWNLOAD_EXAMPLE_OBJECTS)