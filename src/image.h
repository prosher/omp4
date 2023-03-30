#ifndef IMAGE_H
#define IMAGE_H

#include <stdio.h>
#include <stdint.h>

typedef struct {
    int w;
    int h;
    uint8_t *matr;
} Image;

void img_init(Image *p, int w, int h);

void img_destroy(Image *p);

uint8_t *img_point(Image *p, int i, int j);

int read_pnm(FILE *file, Image *img);

int write_pnm(FILE *file, Image *img);

#endif