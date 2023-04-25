#include "png.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "return_codes.h"

int initIHDR(IHDR *ihdr, FILE *fp)
{
  unsigned char buf4[4];
  if (fread(buf4, sizeof(unsigned), 1, fp) != 1)
    return ERROR_DATA_INVALID;
  ihdr->width = bigEndian(buf4);
  if (fread(buf4, sizeof(unsigned), 1, fp) != 1)
    return ERROR_DATA_INVALID;
  ihdr->height = bigEndian(buf4);
  if (fread(&(ihdr->bitDepth), sizeof(unsigned char), 1, fp) != 1)
    return ERROR_DATA_INVALID;
  if (fread(&(ihdr->colourType), sizeof(unsigned char), 1, fp) != 1)
    return ERROR_DATA_INVALID;
  if (fread(&(ihdr->compressionMethod), sizeof(unsigned char), 1, fp) != 1)
    return ERROR_DATA_INVALID;
  if (fread(&(ihdr->filterMethod), sizeof(unsigned char), 1, fp) != 1)
    return ERROR_DATA_INVALID;
  if (fread(&(ihdr->interlaceMethod), sizeof(unsigned char), 1, fp) != 1)
    return ERROR_DATA_INVALID;
  return SUCCESS;
}

int initIDAT(IDAT *idat, FILE *fp)
{
  unsigned char buf4[4];
  if (fread(buf4, sizeof(unsigned), 1, fp) != 1)
    return ERROR_DATA_INVALID;
  idat->offset = bigEndian(buf4);
  unsigned buf;
  if (fread(&buf, sizeof(unsigned), 1, fp) != 1)
    return ERROR_DATA_INVALID;
  idat->toDecompress = malloc(sizeof(unsigned char) * idat->offset);
  if (!idat->toDecompress)
    return ERROR_OUT_OF_MEMORY;
  if (fread(idat->toDecompress, sizeof(unsigned char), idat->offset, fp) != idat->offset)
    return ERROR_DATA_INVALID;
  return SUCCESS;
}

int initPLTE(PLTE *plte, FILE *fp)
{
  plte->bytePerColor = 3;
  unsigned char buf4[4];
  if (fread(buf4, sizeof(unsigned), 1, fp) != 1)
    return ERROR_DATA_INVALID;
  plte->size = bigEndian(buf4);
  unsigned buf;
  if (fread(&buf, sizeof(unsigned), 1, fp) != 1)
    return ERROR_DATA_INVALID;
  plte->pallete = malloc(sizeof(unsigned char) * plte->size);
  if (!plte->pallete)
    return ERROR_OUT_OF_MEMORY;
  if (fread(plte->pallete, sizeof(unsigned char), plte->size, fp) != plte->size)
    return ERROR_DATA_INVALID;
  return SUCCESS;
}

int initPNG(PNG *png, char *filename)
{
  int code;
  FILE *fp = fopen(filename, "rb");
  if (!fp)
    return ERROR_CANNOT_OPEN_FILE;
  if ((code = validSignature(fp)) != SUCCESS)
    return code;
  if ((code = findIHDR(fp)) != SUCCESS)
    return code;
  if ((code = initIHDR(&(png->ihdr), fp)) != SUCCESS)
    return code;
  if (png->ihdr.colourType == 3)
  {
    if ((code = findPLTE(fp)) != SUCCESS)
      return code;
    if ((code = initPLTE(&(png->plte), fp)) != SUCCESS)
      return code;
  }
  if ((code = findIDAT(fp)) != SUCCESS)
    return code;
  if ((code = initIDAT(&(png->idat), fp)) != SUCCESS)
    return code;
  fclose(fp);
  return SUCCESS;
}

int validSignature(FILE *fp)
{
  unsigned char buf[8];
  if (fread(buf, sizeof(unsigned char), 8, fp) != 8)
    return ERROR_DATA_INVALID;
  unsigned char signature[8] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
  for (int i = 0; i < 8; i++)
    if (signature[i] != buf[i])
      return ERROR_DATA_INVALID;
  return SUCCESS;
}

int findIHDR(FILE *fp)
{
  unsigned char buf[4];
  if (fread(buf, sizeof(unsigned char), 4, fp) != 4)
    return ERROR_DATA_INVALID;
  if (fread(buf, sizeof(unsigned char), 4, fp) != 4)
    return ERROR_DATA_INVALID;
  unsigned char ihdr_tag[4] = {'I', 'H', 'D', 'R'};
  for (int i = 0; i < 4; i++)
  {
    if (ihdr_tag[i] != buf[i])
      return ERROR_DATA_INVALID;
  }
  return SUCCESS;
}

int findIDAT(FILE *fp)
{
  unsigned char idat_tag[4] = {0x49, 0x44, 0x41, 0x54};
  int idat_tag_index = 0;
  unsigned char buf;
  while (1)
  {
    if (fread(&buf, sizeof(unsigned char), 1, fp) != 1)
      return ERROR_DATA_INVALID;
    if (buf == idat_tag[idat_tag_index])
      idat_tag_index++;
    else
      idat_tag_index = 0;
    if (idat_tag_index == 4)
      break;
  }
  fseek(fp, -8, SEEK_CUR);
  return SUCCESS;
}

int findPLTE(FILE *fp)
{
  unsigned char plte_tag[4] = {0x50, 0x4C, 0x54, 0x45};
  int idat_tag_index = 0;
  unsigned char buf;
  while (1)
  {
    if (fread(&buf, sizeof(unsigned char), 1, fp) != 1)
      return ERROR_DATA_INVALID;
    if (buf == plte_tag[idat_tag_index])
      idat_tag_index++;
    else
      idat_tag_index = 0;
    if (idat_tag_index == 4)
      break;
  }
  fseek(fp, -8, SEEK_CUR);
  return SUCCESS;
}

int supportedFormat(PNG *png)
{
  if (png->ihdr.bitDepth != 8)
    return ERROR_UNSUPPORTED;
  switch (png->ihdr.colourType)
  {
  case 0:
    break;
  case 2:
    break;
  case 3:
    break;
  case 4:
    return ERROR_UNSUPPORTED;
  case 6:
    return ERROR_UNSUPPORTED;
  default:
    return ERROR_DATA_INVALID;
  }

  return SUCCESS;
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