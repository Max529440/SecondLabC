#include "bitmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "return_codes.h"

int initBitmap(Bitmap *bitmap, DecompressedData *decompData)
{
  int code;
  bitmap->bpp = getBPP(bitmap->bitDepth, bitmap->colourType);
  bitmap->rowLength = bitmap->width * bitmap->bpp;
  bitmap->rows = malloc(sizeof(RowData) * bitmap->height);
  if (!bitmap->rows)
    return ERROR_OUT_OF_MEMORY;
  bitmap->imageDataSize = bitmap->bpp * bitmap->height * bitmap->width;
  bitmap->imageData = malloc(sizeof(unsigned char) * bitmap->imageDataSize);
  if (!bitmap->imageData)
    return ERROR_OUT_OF_MEMORY;

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

    if ((code = initRowData(&(bitmap->rows[i]))) != SUCCESS)
      return code;
    if (i == 0)
      filterRecover(&(bitmap->rows[i]), NULL);
    else
      filterRecover(&(bitmap->rows[i]), &(bitmap->rows[i - 1]));
  }

  return SUCCESS;
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

int fromIndexToRGB(Bitmap *bitmap, PLTE *plte)
{
  unsigned imageDataSize = bitmap->bpp * bitmap->height * bitmap->width;
  unsigned rgbImageSize = bitmap->bpp * bitmap->height * bitmap->width * plte->bytePerColor;
  unsigned char *rgbImageData = malloc(sizeof(unsigned char) * rgbImageSize);
  if (!rgbImageData)
    return ERROR_OUT_OF_MEMORY;
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

  return SUCCESS;
}

int fromGrayScaleToRGB(Bitmap *bitmap)
{
  unsigned char bytePerColor = 3;
  unsigned imageDataSize = bitmap->bpp * bitmap->height * bitmap->width;
  unsigned rgbImageSize = bitmap->bpp * bitmap->height * bitmap->width * bytePerColor;
  unsigned char *rgbImageData = malloc(sizeof(unsigned char) * rgbImageSize);
  if (!rgbImageData)
    return ERROR_OUT_OF_MEMORY;
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

  return SUCCESS;
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
    return ERROR_CANNOT_OPEN_FILE;
  int code;
  if (bitmap->colourType == 0)
  {
    if ((code = fprintf(fp, "P5\n")) != 3)
    {
      return -1;
    }
  }
  else if (fprintf(fp, "P6\n") != 3)
    return -1;
  fprintf(fp, "%u %u\n", bitmap->width, bitmap->height);
  fprintf(fp, "%u\n", bitmap->maxColorValue);
  for (int i = 0; i < bitmap->imageDataSize; i++)
  {
    if (fwrite(&(bitmap->imageData[i]), sizeof(unsigned char), 1, fp) != 1)
      return -1;
  }

  return SUCCESS;
}