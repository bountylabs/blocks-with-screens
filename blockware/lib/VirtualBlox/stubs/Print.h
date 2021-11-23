#ifndef __print_h__
#define __print_h__

#define boolean bool
#define String string
#define __FlashStringHelper void

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <string>

using std::string;

class Print {
public:
    virtual size_t write(uint8_t) = 0;
    virtual void print(char* str) {
        for (int i = 0; i < strlen(str); i++)
        {
            write(str[i]);
        }
    };
};

#endif