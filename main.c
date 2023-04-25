#include <zlib.h>
#include <stdio.h>
#include <stdlib.h>
#include "png.h"
#include "decompress.h"
#include "bitmap.h"

int main()
{
   char *filename = "/Users/maksimhimcenko/Desktop/9999.png";
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

   if (png.ihdr.colourType == 3)
   {
      printf("\nPALLETE\n");
      printf("bpc = %u\n", png.plte.bytePerColor);
      for (int i = 0; i < png.plte.size; i++)
      {
         printf("%u\n", png.plte.pallete[i]);
      }
   }

   printf("\nraw data\n");
   Bitmap bitmap;
   bitmap.bitDepth = png.ihdr.bitDepth;
   bitmap.colourType = png.ihdr.colourType;
   bitmap.width = png.ihdr.width;
   bitmap.height = png.ihdr.height;

   initBitmap(&bitmap, &decompData);
   fillImageData(&bitmap);
   if (bitmap.colourType == 3)
   {
      fromIndexToRGB(&bitmap, &(png.plte));
   }
   else if (bitmap.colourType == 0)
   {
      //fromGrayScaleToRGB(&bitmap);
   }
   printf("height = %u\n", bitmap.height);
   printf("row length = %u\n", bitmap.rowLength);
   printf("bitmap bpp = %u\n", bitmap.bpp);
   for (int i = 0; i < bitmap.height * bitmap.rowLength; i++)
   {
      printf("%u\n", bitmap.imageData[i]);
   }
   calcMaxColorValue(&bitmap);
   printf("maxColor = %u\n", bitmap.maxColorValue);

   if(!writePNM(&bitmap, "3.pgm"))
      printf("cannot write file\n");
   return 0;
}

// clang main.c png.c decompress.c row.c bitmap.c -l -o main