#include "decompress.h"
#include <zlib.h>
#include <zconf.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "return_codes.h"

int decompressData(unsigned char *src, unsigned srclen, DecompressedData *dst)
{
    unsigned full_len = srclen;
    dst->data = (unsigned char *)calloc(sizeof(unsigned char), full_len);
    if (!dst->data)
    {
        return ERROR_OUT_OF_MEMORY;
    }

    z_stream strm;
    strm.next_in = (unsigned char *)src;
    strm.avail_in = srclen;
    strm.total_out = 0;
    strm.opaque = Z_NULL;
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;

    int done = 0;

    if (inflateInit(&strm) != Z_OK)
    {
        free(dst->data);
        return ERROR_DATA_INVALID;
    }

    while (!done)
    {
        if (strm.total_out >= full_len)
        {
            dst->data = (unsigned char *)realloc(dst->data, strm.total_out);
            if (!dst->data)
            {
                return ERROR_OUT_OF_MEMORY;
            }
        }
        strm.next_out = (unsigned char *)(dst->data + strm.total_out);
        strm.avail_out = full_len - strm.total_out;

        int err = inflate(&strm, Z_NO_FLUSH);
        if (err == Z_STREAM_END)
        {
            done = 1;
        }
        else if (err != Z_OK)
        {
            break;
        }
    }

    if (inflateEnd(&strm) != Z_OK)
    {
        free(dst->data);
        return ERROR_DATA_INVALID;
    }

    dst->size = strm.total_out;

    return SUCCESS;
}

void freeDecompressedData(DecompressedData *decompData)
{
    if (decompData->data)
        free(decompData->data);
}