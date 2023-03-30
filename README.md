# omp4

This is a Computer Architecture class assignment I got in my university.
This program was written with intent of learning multithread programming basics using C and OpenMP API.  

This program reads an image in PGM format and runs Otsu's image segmentation algorithm on it. 

The results are listed below:

![Original image!](/test_data/in.pgm)
![Processed image!](/test_data/out.pgm)

Program calculates a histogram from given image, then brute-forces search of 3 segment thresholds.
Calculation time is reduced significantly by using OpenMP's directives such as `#pragma omp parallel for` on loops.

## How to use it

### Running

Run `omp4` binary and provide number of threads you want to run it on, as well as paths to input and output images in PGM format, as an example below:

```
./omp4 -1 test_data/in.pgm test_data/out.pgm
```

Program will update or create output image with the result of pre-set segment brightnesses and provide measured runtime of the program 
(only algorithm, excluding reading and writing files).

Launch first argument can also accept additional values:

* `0: Run with disabled OpenMP directives`

* `-1: Run with all threads enabled`

### Testing

Run `test.sh` shell script and provide amount of threads it will run progam on and number of launches respectively:

```
./test.sh 8 5
```

Script will run program on two files `in.pgm` and `in8k.pgm` and produce verbose output of runtime measures.
The first image contains 500x500 dots, the second one contains 7680x4320 dots.
As a result, with the first image threads will be most loaded while counting thresholds, with the second image it will be histogram.

##

Repository also contains a report in Russian with more precise description of the program and its results.
