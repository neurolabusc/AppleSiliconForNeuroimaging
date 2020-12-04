## About

Modern computers have multiple cores which can accelerate some tasks by running them in parallel. [OpenMP](https://www.openmp.org) is a library that simplifies this process. Both the gcc and Clang/LLVM compilers support OpenMP. This project evaluates OpenMP. It applies a [Gaussian blur](https://en.wikipedia.org/wiki/Gaussian_blur) to a each 1D row of a large 4D image (simulating a resting state fMRI dataset, though note we actually blur all 3 spatial dimensions). The computation can be done independently for each row, so this is an example of an `embarrassingly parallel` task. The identical code is compiled can be compiled with both gcc and Clang. 

## OpenMP with Apple Clang

Apple's Clang does not come with OpenMP. However, you can add this library.

 1. You can get a [compiled](https://mac.r-project.org/openmp/) library.
 2. Alternatively, you can compile your own copy:
 
```
git clone https://github.com/llvm/llvm-project/tree/master/openmp
cd openmp; mkdir build; cd build
cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES='arm64;x86_64' .. 
make install
```

  Once you have installed the OpenMP libraries, you can compile using this library by including the `-Xclang -fopenmp -lomp` compiler arguments.
  
## Comparisons

Here the performance is evaluated for a single thread versus all those provided by the computer (e.g. 24 for the Ryzen 3900X, 8 for the others). However, you can also manually set the number of threads. Doing so will reveal that running four threads on the 8259u and M1 and 12 threads on the Ryzen 3900X perform similarly to using all available threads. This suggests that the SMT (Intel/AMD) and efficiency cores (Apple) do not have much impact on this test.

Clang 11, macOS, Intel 8259U (8 threads)

```
g++ -O3 -o omp main.cpp; time ./omp 4
 Reporting minimum time for 4 repetitions with 1 threads
blur: min/mean	1083	1088	ms
real	0m6.884s
g++ -Xclang -fopenmp -O3 -o omp main.cpp -lomp; time ./omp 4
 Reporting minimum time for 4 repetitions with 8 threads
blur: min/mean	234	236	ms
real	0m2.444s

```

Clang 10.0, Ubuntu, AMD 3900X (24 threads)

```
>clang++ -O3 -o omp main.cpp; time ./omp 4
blur: min/mean	655	658	ms
real	0m3.815s
>clang++ -O3 -fopenmp -o omp main.cpp; time ./omp 4
 Reporting minimum time for 4 repetitions with 24 threads
blur: min/mean	51	51	ms
real	0m0.819s
```

clang 12.0, macOS, Apple M1 (8 threads)


```
> g++ -O3 -o omp main.cpp; time ./omp 4
 Reporting minimum time for 4 repetitions with 1 threads
blur: min/mean	573	576	ms
./omp 4  3.09s user 0.38s system 85% cpu 4.073 total
g++ -Xclang -fopenmp -O3 -o omp main.cpp -lomp; time ./omp 4
 Reporting minimum time for 4 repetitions with 8 threads
blur: min/mean	127	128	ms
./omp 4  5.40s user 0.74s system 515% cpu 1.191 total
```

## Links

 - This repository provides the minimal code to describe the issue. It is based on the more complicated but practical [niimath](https://github.com/rordenlab/niimath) software.
