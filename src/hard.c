#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include "image.h"
#include "filter.h"

void fail(const char *msg) {
    printf("Application failed with an error: %s\n", msg);
}

int main(int argc, char *argv[]) {

    if (argc != 4){
        fail("invalid arguments");
        printf("expected input: ");
        printf("omp4 <threads count> <PNM input file> <PNM output file>\n");
        return 0;
    }
    char *end;
    int tnum = strtol(argv[1], &end, 10);
    if (argv[1] == end || tnum > omp_get_max_threads()) {
        fail("invalid threads count");
        return 0;
    }
    if (tnum == 0) {
        tnum = omp_get_max_threads();
    }
    omp_set_num_threads(tnum);

    FILE *fin = fopen(argv[2], "rb");
    if (!fin) {
        fail("unable to open input file");
        return 0;
    }
    Image *img = (Image *) malloc(sizeof(Image));
    if (read_pnm(fin, img) == -1) {
        fail("unexpected PNM input file format");
        return 0;
    }
    fclose(fin);

    double tstart = omp_get_wtime();
    filter4(img, tnum);
    double tend = omp_get_wtime();


    FILE *fout = fopen(argv[3], "wb");
    if (!fout) {
        fail("unable to open output file");
        return 0;
    }
    if (write_pnm(fout, img) == -1) {
        fail("unable to write to output file");
    }
    fclose(fout);
    img_destroy(img);
    free(img);

    printf("Time (%i thread(s)): %g ms\n", tnum, (tend - tstart) * 1000.0);

    return 0;
}