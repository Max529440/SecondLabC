#include <zlib.h>
#include <stdio.h>
#include "png.h"
#include "decompress.c"

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
 DecompressedData decompData;
 if (!decompress(png.idat.data, png.idat.datasize, &decompData))
 {
  printf("zlib decompressing error\n");
  exit(-1);
 }
 printf("decompressed data:\n");
 for (int i = 0; i < decompData.size; i++)
 {
  printf("%u\n", decompData.data[i]);
 }
 return 0;
}


//clang main.c png.c decompress.c -l -o main