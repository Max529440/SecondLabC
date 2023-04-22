#ifndef DECOMPRESS_H
#define DECOMPRESS_H

typedef struct DecompressedData
{
 unsigned char *data;
 unsigned size;
} DecompressedData;

int decompressData(unsigned char *src, unsigned srclen, DecompressedData *dst);

#endif