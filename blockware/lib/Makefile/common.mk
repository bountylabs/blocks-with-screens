# Standard common makefile for platformio-based blockware

.PHONY: src/secrets.h
src/secrets.h:
# If secrets.h.example exists, but not secrets.h, generate secrets.h
# TODO: There is probably a simpler way to do this
ifneq ("$(wildcard src/secrets.h.example)","")
ifeq ("$(wildcard src/secrets.h)","")
	echo "Creating secrets.h from secrets.h.example"
	cp src/secrets.h.example src/secrets.h
else
	echo "Skipping creation of secrets.h (already exists)"
endif
else
	echo "Skipping secrets.h (no secrets.h.example)"
endif

test: src/secrets.h
	pio run
