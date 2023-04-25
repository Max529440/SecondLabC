#ifndef ROW_H
#define ROW_H

typedef struct RowData
{
 unsigned char bitDepth;
 unsigned char colourType;
 unsigned width;
 unsigned char filterType;
 unsigned char bpp;
 unsigned size;
 unsigned char *data;
 unsigned char *rawData;
} RowData;

int initRowData(RowData *rowData); // fields bitDepth, colourType, width, filterType and data must initialized
unsigned char getBPP(unsigned char bitDepth, unsigned char colourType);
void filterRecover(RowData *rowData, RowData *priorRowData);
int filterRecoverNone(RowData *rowData);
void filterRecoverSub(RowData *rowData);
void filterRecoverUp(RowData *rowData, RowData *priorRowData);
void filterRecoverAverage(RowData *rowData, RowData *priorRowData);
void filterRecoverPaeth(RowData *rowData, RowData *priorRowData);
void freeRowData(RowData *rowData);

unsigned char floorHalfSum(unsigned char a, unsigned char b);
unsigned char paethPredictor(unsigned char a, unsigned char b, unsigned char c);
unsigned char uabs(unsigned char x);

#endif