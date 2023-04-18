#include <zlib.h>
#include <stdio.h>
#include "png.h"

int main()
{
 char *filename = "/Users/maksimhimcenko/Desktop/454545.png";
 PNG png;
 if (initPNG(&png, filename))
  printf("OK\n");
 else
  printf("NOT OK\n");
 return 0;
}

// clang main.c png.c -o main
