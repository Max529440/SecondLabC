#include "png.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int initIHDR(IHDR *ihdr, FILE *fp)
{
  unsigned char buf4[4];
  if (fread(buf4, sizeof(unsigned), 1, fp) != 1)
    return 0;
  ihdr->width = bigEndian(buf4);
  if (fread(buf4, sizeof(unsigned), 1, fp) != 1)
    return 0;
  ihdr->height = bigEndian(buf4);
  if (fread(&(ihdr->bitDepth), sizeof(unsigned char), 1, fp) != 1)
    return 0;
  if (fread(&(ihdr->colourType), sizeof(unsigned char), 1, fp) != 1)
    return 0;
  if (fread(&(ihdr->compressionMethod), sizeof(unsigned char), 1, fp) != 1)
    return 0;
  if (fread(&(ihdr->filterMethod), sizeof(unsigned char), 1, fp) != 1)
    return 0;
  if (fread(&(ihdr->interlaceMethod), sizeof(unsigned char), 1, fp) != 1)
    return 0;
  return 1;
}

int initIDAT(IDAT *idat, FILE *fp)
{
  unsigned char buf4[4];
  if (fread(buf4, sizeof(unsigned), 1, fp) != 1)
    return 0;
  idat->offset = bigEndian(buf4);
  unsigned buf;
  if (fread(&buf, sizeof(unsigned), 1, fp) != 1)
    return 0;
  idat->toDecompress = malloc(sizeof(unsigned char) * idat->offset);
  if (fread(idat->toDecompress, sizeof(unsigned char), idat->offset, fp) != idat->offset)
    return 0;
  return 1;
}

int initPLTE(PLTE *plte, FILE *fp)
{
  plte->bytePerColor = 3;
  unsigned char buf4[4];
  if (fread(buf4, sizeof(unsigned), 1, fp) != 1)
    return 0;
  plte->size = bigEndian(buf4);
  unsigned buf;
  if (fread(&buf, sizeof(unsigned), 1, fp) != 1)
    return 0;
  plte->pallete = malloc(sizeof(unsigned char) * plte->size);
  if (fread(plte->pallete, sizeof(unsigned char), plte->size, fp) != plte->size)
    return 0;
  return 1;
}

int initPNG(PNG *png, char *filename)
{
  FILE *fp = fopen(filename, "rb");
  if (!fp)
    return 0;
  if (!validSignature(fp))
    return 0;
  if (!findIHDR(fp))
    return 0;
  if (!initIHDR(&(png->ihdr), fp))
    return 0;
  if (png->ihdr.colourType == 3)
  {
    if (!findPLTE(fp))
      return 0;
    if (!initPLTE(&(png->plte), fp))
      return 0;
  }
  if (!findIDAT(fp))
    return 0;
  if (!initIDAT(&(png->idat), fp))
    return 0;
  fclose(fp);
  return 1;
}

int validSignature(FILE *fp)
{
  unsigned char buf[8];
  if (fread(buf, sizeof(unsigned char), 8, fp) != 8)
    return 0;
  unsigned char signature[8] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
  for (int i = 0; i < 8; i++)
    if (signature[i] != buf[i])
      return 0;
  return 1;
}

int findIHDR(FILE *fp)
{
  unsigned char buf[4];
  if (fread(buf, sizeof(unsigned char), 4, fp) != 4)
    return 0;
  if (fread(buf, sizeof(unsigned char), 4, fp) != 4)
    return 0;
  unsigned char ihdr_tag[4] = {'I', 'H', 'D', 'R'};
  for (int i = 0; i < 4; i++)
  {
    if (ihdr_tag[i] != buf[i])
      return 0;
  }
  return 1;
}

int findIDAT(FILE *fp)
{
  unsigned char idat_tag[4] = {0x49, 0x44, 0x41, 0x54};
  int idat_tag_index = 0;
  unsigned char buf;
  while (1)
  {
    if (fread(&buf, sizeof(unsigned char), 1, fp) != 1)
      return 0;
    if (buf == idat_tag[idat_tag_index])
      idat_tag_index++;
    else
      idat_tag_index = 0;
    if (idat_tag_index == 4)
      break;
  }
  fseek(fp, -8, SEEK_CUR);
  return 1;
}

int findPLTE(FILE *fp)
{
  unsigned char plte_tag[4] = {0x50, 0x4C, 0x54, 0x45};
  int idat_tag_index = 0;
  unsigned char buf;
  while (1)
  {
    if (fread(&buf, sizeof(unsigned char), 1, fp) != 1)
      return 0;
    if (buf == plte_tag[idat_tag_index])
      idat_tag_index++;
    else
      idat_tag_index = 0;
    if (idat_tag_index == 4)
      break;
  }
  fseek(fp, -8, SEEK_CUR);
  return 1;
}

unsigned bigEndian(unsigned char *bytes4)
{
  unsigned res = 0;
  unsigned mult = 1;
  for (int j = 0; j < 4; j++)
  {
    res += mult * bytes4[3 - j];
    mult *= 255;
  }

  return res;
}

void freePNG(PNG *png)
{
  if (png->idat.toDecompress)
    free(png->idat.toDecompress);
  if (png->plte.pallete)
    free(png->plte.pallete);
}