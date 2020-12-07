## Introduction

Fortran plays a pivotal role in science. Open source [scientific tools](https://walkingrandomly.com/?p=6696) like [R](github.com/wch/r-source) and [Python's numerical libraries](https://github.com/scipy/scipy) leverage Fortran. Vast numbers of robust scientific toolsuse this language. While original developed in the 1950's, Fortran's use of arrays makes it well-suited for numerical arrays, and has features that allow high performance optimization. 

Unfortunately, the open source Fortran compilers do not yet support the ARM-based architecture used by Apple's M1 CPUs. Fortunately, Iain Sandoe has been working on porting both gcc and gFortran to this architecture. François-Xavier Coudert has created [an experimental release for the M1](https://github.com/fxcoudert/gfortran-for-macOS/releases). Here we assess the performance of this nascent tool.

Here we test a [couple of popular benchmarks](https://benchmarksgame-team.pages.debian.net/benchmarksgame/fastest/fortran.html). These old benchmarks execute very rapidly, and are probably not a good measure of compute performance. I also include [a simple benchmark I wrote (f400) simulating neuroimaging data](https://github.com/fxcoudert/gfortran-for-macOS/issues/13) (which is probably limited more by memory bandwidth than computation). Due to these factors, I would take these results with a grain of salt. However, the fact that each of these runs to completion suggests that scientific tools which rely on Fortran may soon be available on this promising architecture.


## Installing dependencies

Beyond gfortran and OpenMP, you will need a few libraries

For Linux:
```
sudo apt-get install libapr1 libapr1-dev
sudo apt-get install libgmp3-dev
```

For macOS x86-64:
```
brew install gmp
gfortran -O3 -I/usr/local/Cellar/gmp/6.2.1/include -o fpidigits pidigits.f90 -L/usr/local/Cellar/gmp/6.2.1/lib -lgmp; time ./fpidigits 10000
```

For macOS ARM (at the moment, libraries must be compiled from source):
```
export ARCHFLAGS='-arch arm64'
brew install -s gmp
-I/opt/homebrew/Cellar/gmp/6.2.1/include
gfortran -O3 -I/opt/homebrew/Cellar/gmp/6.2.1/include -o fpidigits pidigits.f90 -L/opt/homebrew/Cellar/gmp/6.2.1/lib -lgmp; time ./fpidigits 10000
```

## Running the scripts

```
gfortran -O3 -o fbinarytrees binarytrees.f90 -lapr-1; time ./fbinarytrees 21
gfortran -O3 -o ffannkuch3 fannkuch3.f90; time ./ffannkuch3 12
gfortran -O3 -o ffasta fasta.f90; time ./ffasta 25000000  > /dev/null
gfortran -O3 -o fmandelbrot mandelbrot.f90 -lapr-1; time ./fmandelbrot 16000  > /dev/null
gfortran -O3 -o fnbody nbody.f90; time ./fnbody 50000000
gfortran -O3 -I/opt/homebrew/Cellar/gmp/6.2.1/include -o fpidigits pidigits.f90 -L/opt/homebrew/Cellar/gmp/6.2.1/lib -lgmp; time ./fpidigits 10000
gfortran -O3 -o fpidigits pidigits.f90 -lgmp; time ./fpidigits 10000
gfortran -O3 -o freverse reverse.f90; time ./freverse 0 < revcomp-input100000000.txt
gfortran -O3 -fopenmp -o fspectral-norm spectral-norm.f90; time ./fspectral-norm 5500
gfortran -O3 -o fpidigits pidigits.f90 -lgmp; time ./fpidigits 10000
gfortran -O3 -o freverse reverse.f90; time ./freverse 0 < revcomp-input100000000.txt
gfortran -O3 -fopenmp -o fspectral-norm spectral-norm.f90; time ./fspectral-norm 5500
gfortran -O3 -o f400 fortran400.f95; ./f400
gfortran -O3 -o f400 fortran400.f95; ./f400
```

## Testing

All of these times are very brief. Since macOS applications often [phone home](https://tidbits.com/2020/11/13/apple-network-failure-destroys-an-afternoon-of-worldwide-mac-productivity/) at launch, such short benchmarks are unlikely to be a reliable measure of performance.

|          Test |    i5-8259u   |    3900X      |      M1       |
| ------------- | ------------- | ------------- | ------------- |
| binarytrees   |  3.226        |  2.903        |  2.679        |
| fannkuch3     | 29.577        | 24.222        | 26.867        |
| fasta         |  1.301        |  0.902        |  2.635        |
| mandelbrot    |  4.539        |  3.405        |  3.029        |
| nbody         |  3.265        |  2.437        |  2.642        |
| pidigits      |  0.861        |  0.511        |  0.574        |
| reverse       |  0.006        |  0.002        |  0.159        |
| spectral-norm |  0.638        |  0.165        |  0.454        |
| f400_fp32     |296.547        | 97.050        | 42.773        |
| f400_fp64     |334.015        |276.307        | 85.466        |
