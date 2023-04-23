#include "bitmap.h"
#include <stdlib.h>

void initBitmap(Bitmap *bitmap, DecompressedData *decompData)
{
 bitmap->bpp = getBPP(bitmap->bitDepth, bitmap->colourType);
 bitmap->rowLength = bitmap->width * bitmap->bpp;
 bitmap->rows = malloc(sizeof(RowData) * bitmap->height);

 unsigned decompDataIndex = 0;
 for (int i = 0; i < bitmap->height; i++)
 {
  bitmap->rows[i].bitDepth = bitmap->bitDepth;
  bitmap->rows[i].colourType = bitmap->colourType;
  bitmap->rows[i].width = bitmap->width;
  bitmap->rows[i].filterType = decompData->data[decompDataIndex];
  decompDataIndex++;
  bitmap->rows[i].data = decompData->data[decompDataIndex];
  decompDataIndex += bitmap->rowLength;

  initRowData(&(bitmap->rows[i]));
  if(i == 0)
    filterRecover(&(bitmap->rows[i]), NULL);
  else
    filterRecover(&(bitmap->rows[i]), &(bitmap->rows[i - 1]));
 }
}