#include <stdio.h>
#include <string.h>

int main(int argc, char** argv)
{
    int w = 128;
    int h = 64;
    
    unsigned char pixel[2];
    unsigned char r,g,b;
    
    unsigned char* bitmap;
    
    bitmap = new unsigned char[w*h/8];
    memset(bitmap, 0, w*h/8);
    
    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            fread(pixel, 1, 2, stdin);
            //r = pixel[0] & 0xF8;
            g = (((pixel[0] & 0x07) << 3) + ((pixel[1] & 0xE0) >> 5)) << 2;
            //b = pixel[1] & 0x1F;
            
            fprintf(stderr, "x %d y %d g %d\n", x, y, g);
            
            if (g >= 128)
            {
                int vblock = y / 8;
                int bit = y % 8;
                
                int index = (vblock * w) + x;
                fprintf(stderr, "vblock %d bit %d index %d\n", vblock, bit, x);
                unsigned char b = bitmap[index];
                b = b | 1 << bit;
                bitmap[index] = b;
            }
        }
    }
    
    fwrite(bitmap, 1, w*h/8, stdout);
}
