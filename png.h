#ifndef PNG_H
#define PNG_H

#include <stdio.h>

typedef struct IHDR
{
    unsigned width;
    unsigned height;
    unsigned char bitDepth;
    unsigned char colourType;
    unsigned char compressionMethod;
    unsigned char filterMethod;
    unsigned char interlaceMethod;
} IHDR;

typedef struct IDAT
{
    unsigned offset;
    unsigned char *toDecompress;
} IDAT;

typedef struct PNG
{
    FILE *fp;
    IHDR ihdr;
    IDAT idat;
} PNG;

int initIHDR(IHDR *ihdr, FILE *fp);
int initIDAT(IDAT *idat, FILE *fp);
int initPNG(PNG *png, char *filename);

int validSignature(FILE *fp);
int findIHDR(FILE *fp);
int findIDAT(FILE *fp);

unsigned bigEndian(unsigned char *bytes4);

#endif