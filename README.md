# Otsu's segmentation algorithm

This is a Computer Architecture class assignment I got in my university.
This program was written with intent of learning multithread programming basics using C and OpenMP API.  

This program reads an image in PGM format and runs Otsu's image segmentation algorithm on it. 

![Original image!](/test_data/in.png?sanitize=true&raw=true)
![Processed image!](/test_data/out-1.pgm.png?sanitize=true&raw=true)

The program calculates a histogram from given image, then brute-forces search of 3 segment thresholds.
Calculation time is reduced significantly by using OpenMP's directives such as `#pragma omp parallel for` on loops.

## How to use it

### Running

Run `omp4` binary and provide number of threads you want to run it on, as well as paths to input and output images in PGM format, as an example below:

```
./omp4 -1 test_data/in.pgm test_data/out.pgm
```

The program will update or create an output image file and provide measured runtime of the algorithm (excluding reading to files and writing from them).

The first launch argument can also accept additional values:

* `-1: Run with disabled OpenMP directives`

* `0: Run with all threads enabled`

### Testing

Run a `test.sh` shell script and provide the amount of threads it will run the program on and the number of launches respectively:

```
./test.sh 8 5
```

The script will run the program on two files `in.pgm` and `in8k.pgm` and produce a verbose output of runtime measures.
The first given image `test_data/in.pgm` contains 500x500 dots, the second, `test_data/in8k.pgm`, contains 7680x4320 dots.
As a result, with the first image most of the runtime calculations will be represent counting thresholds, with the second image - counting the histogram.

##

This repository also contains a report in Russian, providing more precise description of the program and runtime measurements analysis.
