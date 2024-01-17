#ifndef IMAGE_PROCESSING_IMAGE_FUNCTIONS_H
#define IMAGE_PROCESSING_IMAGE_FUNCTIONS_H

#include "image.h"

int compare_double_values(double a, double b);

void set_pixel(Pixel *pixel, uint8_t blue, uint8_t green, uint8_t red);

uint8_t calculate_pixel_value(uint8_t pixel_value, int value);

void change_brightness(BMPImage *image, int value);

void add_frame(BMPImage *image, int size, Pixel *pixel);

BMPImage *resize(BMPImage *image, double scale);

void generate_ascii(BMPImage *image, char *outputPath);

void invert(BMPImage *image);

double **gaussian_kernel(int size, double sigma);

void grayscale(BMPImage *image);

BMPImage *blur(BMPImage *original, double sigma);

BMPImage *edge_detection(BMPImage *image);

BMPImage *rotate(BMPImage *image);

#endif
