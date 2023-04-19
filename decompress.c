#include "decompress.h"
#include <zlib.h>
#include <zconf.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int decompress(unsigned char *src, unsigned srclen, DecompressedData *dst)
{
 unsigned full_len = srclen;
 unsigned half_len = srclen / 2;
 dst->data = calloc(sizeof(unsigned char), full_len);
 if (!dst->data)
 {
  printf("can't allocate memory for dst\n");
  return 0;
 }

 z_stream strm;
 strm.next_in = (Bytef *)src;
 strm.avail_in = srclen;
 strm.total_out = 0;
 strm.zalloc = Z_NULL;
 strm.zfree = Z_NULL;

 int done = 0;

 if (inflateInit(&strm) != Z_OK)
 {
  free(dst->data);
  return 0;
 }

 while (!done)
 {
  if (strm.total_out >= full_len)
  {
   dst->data = realloc(dst->data, full_len + half_len);
   if (!dst->data)
   {
    printf("can't allocate memory for dst\n");
    return 0;
   }
   full_len += half_len;
   half_len += full_len / 2;
   strm.next_out = (Bytef *)(dst->data + strm.total_out);
   strm.avail_out = full_len - strm.total_out;

   int err = inflate(&strm, Z_NO_FLUSH);
   if (err == Z_STREAM_END)
   {
    return 1;
   }
   else if (err != Z_OK)
   {
    break;
   }
  }
 }

 if (inflateEnd(&strm) != Z_OK)
 {
  free(dst->data);
  return 0;
 }

 dst->size = strm.total_out;

 return 1;
}