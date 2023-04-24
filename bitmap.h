#ifndef BITMAP_H
#define BITMAP_H

#include <stdio.h>
#include "png.h"
#include "row.h"
#include "decompress.h"

typedef struct Bitmap
{
 unsigned width;
 unsigned height;
 unsigned char bitDepth;
 unsigned char colourType;
 unsigned rowLength;
 unsigned char bpp;
 RowData *rows;
 unsigned char *imageData;
 unsigned imageDataSize;
 unsigned char maxColorValue;
} Bitmap;

void initBitmap(Bitmap *bitmap, DecompressedData *decompData); // fields bitDepth, colourType, width, width must initialized
void fillImageData(Bitmap *bitmap);
void fromIndexToRGB(Bitmap *bitmap, PLTE *plte);
void fromGrayScaleToRGB(Bitmap *bitmap);
void calcMaxColorValue(Bitmap *bitmap);
void freeBitmap(Bitmap *bitmap);
int writePNM(Bitmap *bitmap, char *filename);

#endif