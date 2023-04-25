#include "row.h"
#include <stdlib.h>
#include <string.h>
#include "return_codes.h"

int initRowData(RowData *rowData)
{
    rowData->bpp = getBPP(rowData->bitDepth, rowData->colourType);
    rowData->size = rowData->width * rowData->bpp;
    rowData->rawData = malloc(sizeof(unsigned char) * rowData->size);
    if(!rowData->rawData)
        return ERROR_OUT_OF_MEMORY;
    return SUCCESS;
}

unsigned char getBPP(unsigned char bitDepth, unsigned char colourType)
{
    unsigned channels;
    switch (colourType)
    {
    case 0:
        channels = 1;
        break;
    case 2:
        channels = 3;
        break;
    case 3:
        channels = 1;
        break;
    case 4:
        channels = 2;
        break;
    case 6:
        channels = 4;
        break;
    }

    return channels * bitDepth / 8;
}

void filterRecover(RowData *rowData, RowData *priorRowData)
{
    switch (rowData->filterType)
    {
    case 0:
        filterRecoverNone(rowData);
        break;
    case 1:
        filterRecoverSub(rowData);
        break;
    case 2:
        filterRecoverUp(rowData, priorRowData);
        break;
    case 3:
        filterRecoverAverage(rowData, priorRowData);
        break;
    case 4:
        filterRecoverPaeth(rowData, priorRowData);
        break;
    }
}

int filterRecoverNone(RowData *rowData)
{
    memcpy(rowData->rawData, rowData->data, rowData->width * rowData->bpp);
}

void filterRecoverSub(RowData *rowData)
{
    for (int i = 0; i < rowData->size; i++)
    {
        if (i - rowData->bpp < 0)
        {
            rowData->rawData[i] = rowData->data[i];
        }
        else
        {
            rowData->rawData[i] = rowData->data[i] + rowData->rawData[i - rowData->bpp];
        }
    }
}

void filterRecoverUp(RowData *rowData, RowData *priorRowData)
{
    if (!priorRowData)
    {
        filterRecoverNone(rowData);
        return;
    }
    for (int i = 0; i < rowData->size; i++)
    {
        rowData->rawData[i] = rowData->data[i] + priorRowData->rawData[i];
    }
}

void filterRecoverAverage(RowData *rowData, RowData *priorRowData)
{
    unsigned char prior;
    unsigned char pos_minus_bpp;
    for (int i = 0; i < rowData->size; i++)
    {
        prior = (priorRowData == NULL) ? 0 : priorRowData->rawData[i];
        pos_minus_bpp = (i - rowData->bpp < 0) ? 0 : rowData->rawData[i - rowData->bpp];
        rowData->rawData[i] = rowData->data[i] + floorHalfSum(pos_minus_bpp, prior);
    }
}

// Paeth(x) + PaethPredictor(Raw(x-bpp), Prior(x), Prior(x-bpp))
void filterRecoverPaeth(RowData *rowData, RowData *priorRowData)
{
    unsigned char prior;
    unsigned char pos_minus_bpp;
    unsigned char prior_pos_minus_bpp;
    unsigned char paethValue;
    for (int i = 0; i < rowData->size; i++)
    {
        prior = (priorRowData == NULL) ? 0 : priorRowData->rawData[i];
        pos_minus_bpp = (i - rowData->bpp < 0) ? 0 : rowData->rawData[i - rowData->bpp];
        if (priorRowData)
            prior_pos_minus_bpp = (i - priorRowData->bpp < 0) ? 0 : priorRowData->rawData[i - rowData->bpp];
        else
            prior_pos_minus_bpp = 0;
        paethValue = paethPredictor(pos_minus_bpp, prior, prior_pos_minus_bpp);
        rowData->rawData[i] = rowData->data[i]; // todo
    }
}

void freeRowData(RowData *rowData)
{
    if (rowData->rawData)
        free(rowData->rawData);
}

unsigned char paethPredictor(unsigned char a, unsigned char b, unsigned char c)
{
    unsigned char p = a + b - c;
    unsigned char pa = uabs(p - a);
    unsigned char pb = uabs(p - b);
    unsigned char pc = uabs(p - c);
    if ((pa <= pb) && (pa <= pc))
        return a;
    else if (pb <= pc)
        return b;
    return c;
}

// floor((a + b) / 2
unsigned char floorHalfSum(unsigned char a, unsigned char b)
{
    if ((a + b) % 2 == 1)
        return (a + b - 1) / 2;
    return (a + b) / 2;
}

unsigned char uabs(unsigned char x)
{
    return (x < 0) ? (-x) : x;
}