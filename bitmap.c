#include "bitmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void initBitmap(Bitmap *bitmap, DecompressedData *decompData)
{
  bitmap->bpp = getBPP(bitmap->bitDepth, bitmap->colourType);
  bitmap->rowLength = bitmap->width * bitmap->bpp;
  bitmap->rows = malloc(sizeof(RowData) * bitmap->height);
  bitmap->imageDataSize = bitmap->bpp * bitmap->height * bitmap->width;
  bitmap->imageData = malloc(sizeof(unsigned char) * bitmap->imageDataSize);

  unsigned decompDataIndex = 0;
  for (int i = 0; i < bitmap->height; i++)
  {
    bitmap->rows[i].bitDepth = bitmap->bitDepth;
    bitmap->rows[i].colourType = bitmap->colourType;
    bitmap->rows[i].width = bitmap->width;
    bitmap->rows[i].filterType = decompData->data[decompDataIndex];
    decompDataIndex++;
    bitmap->rows[i].data = decompData->data + decompDataIndex;
    decompDataIndex += bitmap->rowLength;

    initRowData(&(bitmap->rows[i]));
    if (i == 0)
      filterRecover(&(bitmap->rows[i]), NULL);
    else
      filterRecover(&(bitmap->rows[i]), &(bitmap->rows[i - 1]));
  }
}

void fillImageData(Bitmap *bitmap)
{
  unsigned index = 0;
  for (int i = 0; i < bitmap->height; i++)
  {
    memcpy(bitmap->imageData + index, bitmap->rows[i].rawData, bitmap->rowLength);
    index += bitmap->rowLength;
  }
}

void fromIndexToRGB(Bitmap *bitmap, PLTE *plte)
{
  unsigned imageDataSize = bitmap->bpp * bitmap->height * bitmap->width;
  unsigned rgbImageSize = bitmap->bpp * bitmap->height * bitmap->width * plte->bytePerColor;
  unsigned char *rgbImageData = malloc(sizeof(unsigned char) * rgbImageSize);
  for (int i = 0; i < imageDataSize; i++)
  {
    for (int j = 0; j < plte->bytePerColor; j++)
    {
      rgbImageData[i * plte->bytePerColor + j] = plte->pallete[bitmap->imageData[i] * plte->bytePerColor + j];
    }
  }
  unsigned char *tmp = bitmap->imageData;
  bitmap->imageData = rgbImageData;
  bitmap->bpp *= plte->bytePerColor;
  bitmap->rowLength = bitmap->width * bitmap->bpp;
  bitmap->imageDataSize = bitmap->bpp * bitmap->height * bitmap->width;
  free(tmp);
}

void fromGrayScaleToRGB(Bitmap *bitmap)
{
  unsigned char bytePerColor = 3;
  unsigned imageDataSize = bitmap->bpp * bitmap->height * bitmap->width;
  unsigned rgbImageSize = bitmap->bpp * bitmap->height * bitmap->width * bytePerColor;
  unsigned char *rgbImageData = malloc(sizeof(unsigned char) * rgbImageSize);
  for (int i = 0; i < imageDataSize; i++)
  {
    for (int j = 0; j < bytePerColor; j++)
    {
      rgbImageData[i * bytePerColor + j] = bitmap->imageData[i];
    }
  }
  unsigned char *tmp = bitmap->imageData;
  bitmap->imageData = rgbImageData;
  bitmap->bpp *= bytePerColor;
  bitmap->rowLength = bitmap->width * bitmap->bpp;
  bitmap->imageDataSize = bitmap->bpp * bitmap->height * bitmap->width;
  free(tmp);
}

/*
void calcMaxColorValue(Bitmap *bitmap)
{
  bitmap->maxColorValue = 0;
  for (int i = 0; i < bitmap->imageDataSize; i++)
  {
    if (bitmap->imageData[i] > bitmap->maxColorValue)
    {
      bitmap->maxColorValue = bitmap->imageData[i];
    }
  }
}
*/

void calcMaxColorValue(Bitmap *bitmap)
{
  bitmap->maxColorValue = 255;
}

void freeBitmap(Bitmap *bitmap)
{
  if (bitmap->rows)
  {
    for (int i = 0; i < bitmap->height; i++)
      freeRowData(&(bitmap->rows[i]));
    free(bitmap->rows);
  }
  if (bitmap->imageData)
    free(bitmap->imageData);
}

int writePNM(Bitmap *bitmap, char *filename)
{
  FILE *fp = fopen(filename, "wb");
  if (!fp)
    return 0;
  int code;
  if (bitmap->colourType == 0)
  {
    if ((code = fprintf(fp, "P5\n")) != 3)
    {
      return 0;
    }
  }
  else if (fprintf(fp, "P6\n") != 3)
    return 0;
  fprintf(fp, "%u %u\n", bitmap->width, bitmap->height);
  fprintf(fp, "%u\n", bitmap->maxColorValue);
  for (int i = 0; i < bitmap->imageDataSize; i++)
  {
    fprintf(fp, "%u\n", bitmap->imageData[i]);
  }

  return 1;
}