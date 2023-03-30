#include <assert.h>
#include "filter.h"

#define COLORS 256

static void hist(int histo[COLORS], const Image *img, int tnum);

static void otsu3(int thresh[3], const int histo[COLORS], int imgsize, int tnum);

void filter4(Image *img, const int tnum)
{
    assert(img);
    int histo[COLORS] = {};
    hist(histo, img, tnum);
    int thresh[3];
    otsu3(thresh, histo, img->w * img->h, tnum);
    printf("%u %u %u\n", thresh[0], thresh[1], thresh[2]);


    const uint8_t FILTER[3 + 1] = {0, 84, 170, 255};
    const int MSIZE = img->w * img->h;

    #pragma omp parallel for schedule(dynamic, 4096) if (tnum > 0)
    for (int i = 0; i < MSIZE; ++i)
    {
        uint8_t color = img->matr[i];
        uint8_t fcolor = FILTER[0];
        for (int t = 0; t < 3; ++t)
        {
            if (color > thresh[t])
            {
                fcolor = FILTER[t + 1];
            }
        }
        img->matr[i] = fcolor;
    }
}

static void hist(int histo[COLORS], const Image *img, int tnum)
{
    assert(histo);
    assert(img);
    const int imgsize = img->w * img->h;

    #pragma omp parallel if (tnum > 0)
    {
        int thisto[COLORS] = {}; // hist for each thread
        #pragma omp for schedule (dynamic, 4096)
        for (int i = 0; i < imgsize; ++i) {
            ++thisto[img->matr[i]];
        }
        #pragma omp critical
        {
            for (int i = 0; i < COLORS; ++i) {
                histo[i] += thisto[i];
            }
        }
    }
}

static void otsu3(int thresh[3], const int histo[COLORS], const int imgsize, const int tnum)
{
    assert(thresh);
    assert(histo);

    double prob[COLORS];  // brightness probabilities
    for (int i = 0; i < COLORS; ++i) {
        prob[i] = 1.0 * histo[i] / imgsize;
    }
    double prob_prsum[COLORS + 1]; // prob prefix sum
    prob_prsum[0] = 0.0;
    for (int i = 0; i < COLORS; ++i) {
        prob_prsum[i + 1] = prob_prsum[i] + prob[i];
    }

    double exp[COLORS]; // brightness expectations
    for (int i = 0; i < COLORS; ++i) {
        exp[i] = prob[i] * (i + 1);
    }
    double exp_prsum[COLORS + 1]; // exp prefix sum
    exp_prsum[0] = 0.0;
    for (int i = 0; i < COLORS; ++i) {
        exp_prsum[i + 1] = exp_prsum[i] + exp[i];
    }

    double maxdisp = 0.0;
    #pragma omp parallel if (tnum > 0)
    {
        double tmaxdisp = 0.0;
        int tthresh1, tthresh2, tthresh3;
        #pragma omp for schedule(dynamic)
        for (int t1 = 0; t1 < COLORS; ++t1)
        {
            for (int t2 = t1 + 1; t2 < COLORS; ++t2)
            {
                for (int t3 = t2 + 1; t3 < COLORS; ++t3)
                {
                    // cluster brightness probabilities
                    const double clprob1 = prob_prsum[t1 + 1] - prob_prsum[0],
                            clprob2 = prob_prsum[t2 + 1] - prob_prsum[t1 + 1],
                            clprob3 = prob_prsum[t3 + 1] - prob_prsum[t2 + 1],
                            clprob4 = prob_prsum[COLORS - 1] - prob_prsum[t3 + 1];

                    // cluster brightness expectations
                    const double clexp1 = exp_prsum[t1 + 1] - exp_prsum[0],
                            clexp2 = exp_prsum[t2 + 1] - exp_prsum[t1 + 1],
                            clexp3 = exp_prsum[t3 + 1] - exp_prsum[t2 + 1],
                            clexp4 = exp_prsum[COLORS - 1] - exp_prsum[t3 + 1];

                    // average cluster brightnesses
                    const double clavg1 = clprob1 ? clexp1 / clprob1 : 0,
                            clavg2 = clprob2 ? clexp2 / clprob2 : 0,
                            clavg3 = clprob3 ? clexp3 / clprob3 : 0,
                            clavg4 = clprob4 ? clexp4 / clprob4 : 0;

                    // image brightness expectation
                    const double imgexp = clexp1 + clexp2 + clexp3 + clexp4;

                    const double clexpdiff1 = clavg1 - imgexp,
                            clexpdiff2 = clavg2 - imgexp,
                            clexpdiff3 = clavg3 - imgexp,
                            clexpdiff4 = clavg4 - imgexp;

                    // intercluster dispersion
                    const double disp = clprob1 * clexpdiff1 * clexpdiff1
                                  + clprob2 * clexpdiff2 * clexpdiff2
                                  + clprob3 * clexpdiff3 * clexpdiff3
                                  + clprob4 * clexpdiff4 * clexpdiff4;

                    if (disp > tmaxdisp)
                    {
                        tthresh1 = t1;
                        tthresh2 = t2;
                        tthresh3 = t3;
                        tmaxdisp = disp;
                    }
                }
            }
        }
        #pragma omp critical
        {
            if (tmaxdisp > maxdisp)
            {
                maxdisp = tmaxdisp;
                thresh[0] = tthresh1;
                thresh[1] = tthresh2;
                thresh[2] = tthresh3;
            }
        }
    }
}