# Standard common makefile for platformio-based blockware

src/secrets.h:
	cp src/secrets.h.example src/secrets.h

test: src/secrets.h
	pio run
