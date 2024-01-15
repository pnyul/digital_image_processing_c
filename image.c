#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "image.h"

#define RGB_BIT_DEPTH 24

BMPImage *read_bmp_image(char *path) {

    FILE *input = fopen(path, "rb");

    if (input == NULL) {
        printf("Can't open the file.\n");
        return NULL;
    }

    BMPImage *image = (BMPImage *) malloc(sizeof(BMPImage));

    if (image == NULL) {
        printf("Unable to allocate memory.");
        return NULL;
    }

    fread(image, sizeof(BMPHeader) + sizeof(InfoHeader), 1, input);

    if (image->bmpHeader.signature != 0x4D42) {
        printf("Wrong image format, not BMP.\n");
        fclose(input);
        free(image);
        input = NULL;
        image = NULL;
        return NULL;
    }

    if (image->infoHeader.bitsPerPixel != RGB_BIT_DEPTH) {
        printf("Not an RGB image.\n");
        fclose(input);
        free(image);
        input = NULL;
        image = NULL;
        return NULL;
    }

    image->pixelData = (Pixel **) malloc(image->infoHeader.height * sizeof(void *));

    if (image->pixelData == NULL) {
        printf("Unable to allocate memory.");
        free(image);
        fclose(input);
        image = NULL;
        input = NULL;
        return NULL;
    }

    for (int i = image->infoHeader.height - 1; 0 <= i; i--) {

        image->pixelData[i] = (Pixel *) malloc(image->infoHeader.width * sizeof(Pixel));

        if (image->pixelData[i] == NULL) {
            printf("Unable to allocate memory.");
            for (int j = 0; j < i; j++)
                free(image->pixelData[j]);
            free(image);
            fclose(input);
            image->pixelData = NULL;
            image = NULL;
            input = NULL;
            return NULL;
        }

        fread(image->pixelData[i], image->infoHeader.width, sizeof(Pixel), input);

    }

    fclose(input);

    return image;

}

void write_bmp_image(BMPImage *image, char *path) {

    FILE *output = fopen(path, "wb");

    if (output == NULL) {
        printf("Can't open the file.\n");
        return;
    }

    fwrite(&image->bmpHeader, sizeof(BMPHeader), 1, output);
    fwrite(&image->infoHeader, sizeof(InfoHeader), 1, output);

    for (int i = image->infoHeader.height - 1; 0 <= i; i--)
        fwrite(image->pixelData[i], image->infoHeader.width, sizeof(Pixel), output);

    fclose(output);
    output = NULL;

}

BMPImage *copy_bmp_image(BMPImage *orig) {

    if (orig == NULL)
        return NULL;

    BMPImage *copy = (BMPImage *) malloc(sizeof(BMPImage));

    if (copy == NULL) {
        printf("Unable to allocate memory.");
        return NULL;
    }

    memcpy(&copy->bmpHeader, &orig->bmpHeader, sizeof(BMPHeader));
    memcpy(&copy->infoHeader, &orig->infoHeader, sizeof(InfoHeader));

    copy->pixelData = (Pixel **) malloc(orig->infoHeader.height * sizeof(Pixel *));

    if (copy->pixelData == NULL) {
        printf("Unable to allocate memory.");
        free(copy);
        copy = NULL;
        return NULL;
    }


    for (int i = 0; i < orig->infoHeader.height; i++) {

        copy->pixelData[i] = (Pixel *) malloc(orig->infoHeader.width * sizeof(Pixel));

        if (copy->pixelData[i] == NULL) {

            printf("Unable to allocate memory.");

            for (int j = 0; j < i; j++)
                free(copy->pixelData[j]);

            free(copy->pixelData);
            free(copy);
            copy = NULL;

            return NULL;

        }

        memcpy(copy->pixelData[i], orig->pixelData[i], orig->infoHeader.width * sizeof(Pixel));

    }

    return copy;

}

void free_bmp_image(BMPImage *image) {

    for (int i = 0; i < image->infoHeader.height; i++)
        free(image->pixelData[i]);

    free(image->pixelData);
    image->pixelData = NULL;
    free(image);
    image = NULL;

}