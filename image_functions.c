#include "image_functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define EPS 1e-9
#define MIN_INTENSITY 0
#define MAX_INTENSITY 255
#define SIGMA_MIN 0.5
#define SIGMA_MAX 3.0

int compare_double_values(double a, double b) {
    return fabs(a - b) < EPS ? 0 : (a < b ? -1 : 1);
}

void set_pixel(Pixel *pixel, uint8_t blue, uint8_t green, uint8_t red) {
    pixel->blue = blue;
    pixel->green = green;
    pixel->red = red;
}

uint8_t calculate_pixel_value(uint8_t pixel_value, int value) {
    return pixel_value + value < MIN_INTENSITY ? MIN_INTENSITY : pixel_value + value >
                                                                 MAX_INTENSITY ? MAX_INTENSITY : pixel_value + value;
}

//handles - (down) and + (up) values
void change_brightness(BMPImage *image, int value) {

    if (value == 0)
        return;

    for (int i = 0; i < image->infoHeader.height; i++) {

        for (int j = 0; j < image->infoHeader.width; j++) {

            image->pixelData[i][j].blue = calculate_pixel_value(image->pixelData[i][j].blue, value);
            image->pixelData[i][j].green = calculate_pixel_value(image->pixelData[i][j].green, value);
            image->pixelData[i][j].red = calculate_pixel_value(image->pixelData[i][j].red, value);

        }

    }

}

void add_frame(BMPImage *image, int size, Pixel *pixel) {

    for (int i = 0; i < size; i++) {

        for (int j = 0; j < image->infoHeader.height; j++) {

            set_pixel(&image->pixelData[j][i], pixel->blue, pixel->green, pixel->red);
            set_pixel(&image->pixelData[j][image->infoHeader.width - i - 1], pixel->blue, pixel->green, pixel->red);

        }

        for (int k = size; k < image->infoHeader.width - size; k++) {

            set_pixel(&image->pixelData[i][k], pixel->blue, pixel->green, pixel->red);
            set_pixel(&image->pixelData[image->infoHeader.height - i - 1][k], pixel->blue, pixel->green, pixel->red);

        }

    }

}

void invert(BMPImage *image) {

    if (image == NULL)
        return;

    for (int i = 0; i < image->infoHeader.height; i++) {

        for (int j = 0; j < image->infoHeader.width; j++) {

            set_pixel(&image->pixelData[i][j], MAX_INTENSITY - image->pixelData[i][j].blue,
                      MAX_INTENSITY - image->pixelData[i][j].green, MAX_INTENSITY - image->pixelData[i][j].red);

        }

    }

}

double **gaussian_kernel(int size, double sigma) {

    if (size <= 0)
        return NULL;

    if (compare_double_values(sigma, SIGMA_MIN) == -1 || compare_double_values(sigma, SIGMA_MAX) == 1) {
        printf("Sigma value must be: [%0.1f, %0.1f]\n", SIGMA_MIN, SIGMA_MAX);
        return NULL;
    }

    double **kernel = (double **) malloc(size * sizeof(double *));

    for (int i = 0; i < size; i++) {

        kernel[i] = malloc(size * sizeof(double));

        if (kernel[i] == NULL) {

            printf("Unable to allocate memory.");

            for (int j = 0; j < i; j++)
                free(kernel[j]);

            free(kernel);
            kernel = NULL;

            return NULL;

        }

    }

    int diff = size / 2;

    double sigmaSq = sigma * sigma;

    for (int i = -diff; i <= diff; i++)
        for (int j = -diff; j <= diff; j++)
            kernel[i + diff][j + diff] =
                    1 / (2 * M_PI * sigmaSq) * pow(M_E, -1 * (i * i + j * j) / (2 * sigmaSq));

    return kernel;

}

BMPImage *blur(BMPImage *original, double sigma) {

    if (original == NULL)
        return NULL;

    if (compare_double_values(sigma, SIGMA_MIN) == -1 || compare_double_values(sigma, SIGMA_MAX) == 1) {
        printf("Sigma value must be: [%0.1f, %0.1f]\n", SIGMA_MIN, SIGMA_MAX);
        return NULL;
    }

    BMPImage *blurred = copy_bmp_image(original);

    if (blurred == NULL) {
        printf("Unable to allocate memory.");
        free(blurred);
        blurred = NULL;
        return NULL;
    }

    int kernelSize = 2 * (int) (3 * sigma) + 1;

    double **kernel = gaussian_kernel(kernelSize, sigma);

    int diff = kernelSize / 2;

    double blue = 0, green = 0, red = 0;

    for (int i = diff; i < original->infoHeader.height - diff; i++) {

        for (int j = diff; j < original->infoHeader.width - diff; j++) {

            for (int k = (-1) * diff; k <= diff; k++) {

                for (int l = (-1) * diff; l <= diff; l++) {

                    blue += kernel[k + diff][l + diff] * original->pixelData[i + k][j + l].blue;
                    green += kernel[k + diff][l + diff] * original->pixelData[i + k][j + l].green;
                    red += kernel[k + diff][l + diff] * original->pixelData[i + k][j + l].red;

                }

            }

            blurred->pixelData[i][j].blue = (uint8_t) blue;
            blurred->pixelData[i][j].green = (uint8_t) green;
            blurred->pixelData[i][j].red = (uint8_t) red;

            blue = 0;
            green = 0;
            red = 0;

        }
    }

    Pixel *pixel = (Pixel *) malloc(sizeof(Pixel));

    if (pixel == NULL)
        return NULL;

    set_pixel(pixel, 0, 0, 0);

    add_frame(blurred, kernelSize / 2, pixel);

    free(pixel);
    pixel = NULL;

    return blurred;

}


void grayscale(BMPImage *image) {

    if (image == NULL)
        return;

    double gray;

    for (int i = 0; i < image->infoHeader.height; i++) {
        for (int j = 0; j < image->infoHeader.width; j++) {

            gray = 0.299 * image->pixelData[i][j].red + 0.587 * image->pixelData[i][j].green +
                   0.114 * image->pixelData[i][j].blue;

            set_pixel(&image->pixelData[i][j], (uint8_t) gray, (uint8_t) gray, (uint8_t) gray);

        }
    }

}

void generate_ascii(BMPImage *image, char *outputPath) {

    FILE *output = fopen(outputPath, "w");

    if (output == NULL) {
        printf("Can't open the file.\n");
        return;
    }

    char text[] = {'$', '*', 'Z', 'u', 'c', '+', '.', ' '};

    for (int i = 0; i < image->infoHeader.height; i++) {

        for (int j = 0; j < image->infoHeader.width; j++) {

            fputc(text[image->pixelData[i][j].blue / 32], output);

        }

        fputc('\n', output);

    }

    fclose(output);

}

void swap(uint8_t *a, uint8_t *b) {
    uint8_t temp = *a;
    *a = *b;
    *b = temp;
}

//rotates by 90 degrees clockwise: first transposes the pixelData matrix then reverses the columns (first->last...)
BMPImage *rotate(BMPImage *image) {

    if (image == NULL)
        return NULL;

    BMPImage *rotated = (BMPImage *) malloc(sizeof(BMPImage));

    if (rotated == NULL) {
        printf("Unable to allocate memory.");
        return NULL;
    }

    memcpy(&rotated->bmpHeader, &image->bmpHeader, sizeof(BMPHeader));
    memcpy(&rotated->infoHeader, &image->infoHeader, sizeof(InfoHeader));

    rotated->pixelData = (Pixel **) malloc(image->infoHeader.width * sizeof(Pixel *));

    if (rotated->pixelData == NULL) {
        printf("Unable to allocate memory.");
        free(rotated);
        rotated = NULL;
        return NULL;
    }

    for (int i = 0; i < image->infoHeader.width; i++) {

        rotated->pixelData[i] = (Pixel *) malloc(image->infoHeader.height * sizeof(Pixel));

        if (rotated->pixelData[i] == NULL) {

            printf("Unable to allocate memory.");

            for (int j = 0; j < i; j++)
                free(rotated->pixelData[j]);

            free(rotated->pixelData);
            free(rotated);
            rotated = NULL;

            return NULL;

        }

    }

    rotated->infoHeader.height = image->infoHeader.width;
    rotated->infoHeader.width = image->infoHeader.height;

    //set the rotated->pixelData matrix with the transposed original pixelData matrix
    for (int i = 0; i < image->infoHeader.height; i++) {

        for (int j = 0; j < image->infoHeader.width; j++) {

            rotated->pixelData[j][i].blue = image->pixelData[i][j].blue;
            rotated->pixelData[j][i].green = image->pixelData[i][j].green;
            rotated->pixelData[j][i].red = image->pixelData[i][j].red;

        }

    }

    //reverse columns
    for (int i = 0; i < rotated->infoHeader.height; i++) {

        for (int j = 0; j < rotated->infoHeader.width / 2; j++) {

            swap(&rotated->pixelData[i][j].blue, &rotated->pixelData[i][rotated->infoHeader.width - j - 1].blue);
            swap(&rotated->pixelData[i][j].green, &rotated->pixelData[i][rotated->infoHeader.width - j - 1].green);
            swap(&rotated->pixelData[i][j].red, &rotated->pixelData[i][rotated->infoHeader.width - j - 1].red);

        }

    }

    return rotated;

}

