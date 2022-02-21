#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>

/**
* A simple .bmp loader. Use loadSOIL() instead.
*/
GLuint loadBMP(const char* imagePath);

/**
* A .dds loader.
*/
GLuint loadDDS(const char* imagePath);

/**
* Readable Image Formats:
*
* BMP - non-1bpp, non-RLE (from stb_image documentation)
* PNG - non-interlaced (from stb_image documentation)
* JPG - JPEG baseline (from stb_image documentation)
* TGA - greyscale or RGB or RGBA or indexed, uncompressed or RLE
* DDS - DXT1/2/3/4/5, uncompressed, cubemaps (can't read 3D DDS files yet)
* PSD - (from stb_image documentation)
* HDR - converted to LDR, unless loaded with *HDR* functions (RGBE or RGBdivA or RGBdivA2)
*
* http://www.lonesock.net/soil.html
*/
GLuint loadSOIL(const char* imagePath);

#endif