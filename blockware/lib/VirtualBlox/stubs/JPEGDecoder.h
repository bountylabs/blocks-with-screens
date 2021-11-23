#ifndef __JPEGDECODER_H__
#define __JPEGDECODER_H__

#include <stdint.h>

class JPEGDecoder {
public:
    JPEGDecoder();
    ~JPEGDecoder();

    int decodeFsFile (const char *pFilename);
    int read(void);
    void abort(void);

    uint16_t *pImage;

    int width;
    int height;
    int MCUWidth;
    int MCUHeight;
    int MCUx;
    int MCUy;

};

extern JPEGDecoder JpegDec;

#endif