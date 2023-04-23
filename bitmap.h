#ifndef BITMAP_H
#define BITMAP_H

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
} Bitmap;

void initBitmap(Bitmap *bitmap, DecompressedData *decompData); // fields bitDepth, colourType, width, width must initialized

#endif