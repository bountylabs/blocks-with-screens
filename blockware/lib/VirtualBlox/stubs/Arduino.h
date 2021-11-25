#ifndef __ARDUINO_H__
#define __ARDUINO_H__

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define LED_BUILTIN 16
#define OUTPUT            0x01

void yield();
void delay(int delay_ms);
void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, uint8_t val);

long random_arduino(long max);

#endif