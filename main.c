#include <zlib.h>
#include <stdio.h>
#include <stdlib.h>
#include "png.h"
#include "decompress.h"

int main()
{
 char *filename = "/Users/maksimhimcenko/Desktop/454545.png";
 PNG png;
 if (!initPNG(&png, filename))
 {
  printf("png parsing error\n");
  exit(-1);
 }
 printf("png parsing: OK\n");
 printf("bit depth = %d\n", png.ihdr.bitDepth);
 printf("filter method = %d\n", png.ihdr.filterMethod);
 printf("interlace method = %d\n", png.ihdr.interlaceMethod);
 printf("colour type = %d\n", png.ihdr.colourType);
 printf("compressDataSize = %u\n", png.idat.offset);
 printf("compressed data:\n");
 for (int i = 0; i < png.idat.offset; i++)
 {
  printf("%x\n", png.idat.toDecompress[i]);
 }

 DecompressedData decompData;
 if (!decompressData(png.idat.toDecompress, png.idat.offset, &decompData))
 {
  printf("zlib decompressing error\n");
  exit(-1);
 }
 printf("decompressed data: size = %d\n", decompData.size);
 for (int i = 0; i < decompData.size; i++)
 {
  printf("%u\n", decompData.data[i]);
 }
 return 0;
}

// clang main.c png.c decompress.c -l -o main