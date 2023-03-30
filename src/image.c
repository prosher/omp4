#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "image.h"

void img_init(Image *p, int w, int h)
{
    assert(p);
    p->w = w;
    p->h = h;
    p->matr = (uint8_t *) malloc(w * h * sizeof(uint8_t));
}

void img_destroy(Image *p)
{
    assert(p);
    free(p->matr);
}

uint8_t *img_point(Image *p, int i, int j)
{
    assert(p);
    return p->matr + (i * p->w + j);
}

int read_pnm(FILE *file, Image *img) {
    assert(file);
    assert(img);
    char format[3];
    if (fscanf(file, "%2s", format) == -1 || strcmp(format, "P5") != 0) {
        return -1;
    }
    int w, h;
    if (fscanf(file, "%i %i", &w, &h) == -1) {
        return -1;
    }
    int m;
    if (fscanf(file, " %i ", &m) == -1 || m != 255) {
        return -1;
    }
    img_init(img, w, h);
    const int dot_count = img->w * img->h;
    if (fread(img->matr, 1, dot_count, file) < dot_count) {
        return -1;
    }
    return 0;
}

int write_pnm(FILE *file, Image *img) {
    assert(file);
    assert(img);
    if (fprintf(file, "P5\n%d %d\n255\n", img->w, img->h) < 0) {
        return -1;
    }
    const int dot_count = img->w * img->h;
    if (fwrite(img->matr, sizeof(uint8_t), dot_count, file) < dot_count) {
        return -1;
    }
    return 0;
}