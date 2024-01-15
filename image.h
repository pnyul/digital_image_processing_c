#ifndef IMAGE_PROCESSING_IMAGE_H
#define IMAGE_PROCESSING_IMAGE_H

#include <stdint.h>

#pragma pack(push, 2)

//bmp header: 14 bytes
typedef struct bmpHeader {
    uint16_t signature;
    uint32_t fileSize;
    //2-2 bytes reserved
    uint32_t reserved;
    uint32_t offset;
} BMPHeader;

//bmp info header: 40 bytes
typedef struct bmpInfoHeader {
    uint32_t size;
    int32_t width;
    int32_t height;
    uint16_t numberOfPlanes;
    uint16_t bitsPerPixel;
    uint32_t compression;
    uint32_t imageSizeInBytes;
    int32_t horizontalResolutionPPM;
    int32_t verticalResolutionPPM;
    uint32_t numberOfColorsUsed;
    uint32_t importantColors;
} InfoHeader;

#pragma pack(pop)

//24 bit RGB color intensities per pixel
typedef struct pixel {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
} Pixel;

typedef struct bmpImage {
    BMPHeader bmpHeader;
    InfoHeader infoHeader;
    Pixel **pixelData;
} BMPImage;

BMPImage *read_bmp_image(char *name);

void write_bmp_image(BMPImage *image, char *name);

BMPImage *copy_bmp_image(BMPImage *orig);

void free_bmp_image(BMPImage *image);

#endif