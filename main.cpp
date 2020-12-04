/* ----------------------------------------------------------------------

//clang 11.0.0, gcc 9.2.0, i5-8259u, MacOS 
g++ -O3 -o omp main.cpp; time ./omp 4
 Reporting minimum time for 4 repetitions with 1 threads
blur: min/mean	1083	1088	ms
real	0m6.884s
g++ -Xclang -fopenmp -O3 -o omp main.cpp -lomp; time ./omp 4
 Reporting minimum time for 4 repetitions with 8 threads
blur: min/mean	234	236	ms
real	0m2.444s
g++-9 -O3 -o omp main.cpp; time ./omp 4
 Reporting minimum time for 4 repetitions with 1 threads
blur: min/mean	1035	1072	ms
real	0m6.737s
g++-9 -O3 -fopenmp -o omp main.cpp; time ./omp 4
 Reporting minimum time for 4 repetitions with 8 threads
blur: min/mean	261	269	ms
real	0m2.700s

//gcc 9.3, clang10.0 Ryzen 3900X, Ubuntu 
>g++ -O3 -o omp main.cpp; time ./omp 4
 Reporting minimum time for 4 repetitions with 1 threads
blur: min/mean	639	639	ms
real	0m3.707s
>g++ -O3 -fopenmp -o omp main.cpp; time ./omp 4
 Reporting minimum time for 4 repetitions with 24 threads
blur: min/mean	53	54	ms
real	0m0.815s
>clang++ -O3 -o omp main.cpp; time ./omp 4
blur: min/mean	655	658	ms
real	0m3.815s
>clang++ -O3 -fopenmp -o omp main.cpp; time ./omp 4
 Reporting minimum time for 4 repetitions with 24 threads
blur: min/mean	51	51	ms
real	0m0.819s

//clang 12.0.0, M1, MacOS
> g++ -O3 -o omp main.cpp; time ./omp 4
 Reporting minimum time for 4 repetitions with 1 threads
blur: min/mean	573	576	ms
./omp 4  3.09s user 0.38s system 85% cpu 4.073 total
g++ -Xclang -fopenmp -O3 -o omp main.cpp -lomp; time ./omp 4
 Reporting minimum time for 4 repetitions with 8 threads
blur: min/mean	127	128	ms
./omp 4  5.40s user 0.74s system 515% cpu 1.191 total

 */
#ifdef __x86_64__    
	#include <immintrin.h>
#else  
	#include "sse2neon.h"
#endif 

#include <cmath> 
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <climits>
#include <cstring>
#if !defined (HAVE_POSIX_MEMALIGN) && !defined (_WIN32) && !defined (__APPLE__)
 #include <malloc.h>
#endif 
#if defined(_OPENMP) 
 #include <omp.h>
#endif 

#ifndef MAX
#define MAX(A,B) ((A) > (B) ? (A) : (B))
#endif

#ifndef MIN
#define MIN(A,B) ((A) > (B) ? (B) : (A))
#endif

#define kXmm 2.0 //e.g. voxels are 2mm apart 
#define kFWHMmm 4.0 //e.g. smooth with 4mm FWHM kernel
#define kX 104
#define kY 90
#define kZ 72
#define kXYZ kX * kY * kZ //3D spatial volume 104*90*72
#define kT 300 //time points: number of 3D volumes
#define kXYZT kXYZ * kT //= total number of voxels across all volumes

#if defined(__ICC) || defined(__INTEL_COMPILER)
	#define kCCsuf  " IntelCC"
#elif defined(_MSC_VER)
	#define kCCsuf  " MSC"
#elif defined(__clang__)
	#define kCCsuf  " Clang"
#elif defined(__GNUC__) || defined(__GNUG__)
    #define kCCsuf  " GCC"
#else
	#define kCCsuf " CompilerNA" //unknown compiler!
#endif


static void blurP(float * img, int64_t nx, int64_t ny, float xmm, float FWHMmm) {
//parallel blur
	//make kernels
	if ((xmm == 0) || (nx < 2) || (ny < 1) || (FWHMmm <= 0.0)) return;
	//float sigma = (FWHMmm/xmm)/sqrt(8*log(2)); 
	float sigma = (FWHMmm/xmm); //mm to vox
	int cutoffvox = round(6*sigma); //filter width to 6 sigma: faster but lower precision AFNI_BLUR_FIRFAC = 2.5
	cutoffvox = MAX(cutoffvox, 1);
	float * k = (float *)_mm_malloc((cutoffvox+1)*sizeof(float), 64); //FIR Gaussian
	float expd = 2*sigma*sigma;
	for (int64_t i = 0; i <= cutoffvox; i++ )
		k[i] = exp(-1.0f*(i*i)/expd);
	//calculate start, end for each voxel in 
	int * kStart = (int *)_mm_malloc(nx*sizeof(int64_t), 64); //-cutoff except left left columns, e.g. 0, -1, -2... cutoffvox
	int * kEnd = (int *)_mm_malloc(nx*sizeof(int), 64); //+cutoff except right columns
	float * kWeight = (float *)_mm_malloc(nx*sizeof(float), 64); //ensure sum of kernel = 1.0
	for (int64_t i = 0; i < nx; i++ ) {
		kStart[i] = MAX(-cutoffvox, -i);//do not read below 0
		kEnd[i] = MIN(cutoffvox, nx-i-1);//do not read beyond final columnn
		if ((i > 0) && (kStart[i] == (kStart[i-1])) && (kEnd[i] == (kEnd[i-1]))) { //reuse weight
				kWeight[i] = kWeight[i-1];
				continue;	
		 }
		 float wt = 0.0f;
		 for (int64_t j = kStart[i]; j <= kEnd[i]; j++ )
			wt += k[abs(j)];
		kWeight[i] = 1 / wt;
		//printf("%d %d->%d %g\n", i, kStart[i], kEnd[i], kWeight[i]);
	}
	//apply kernel to each row
	#pragma omp parallel for
	for (int64_t y = 0; y < ny; y++ ) {
		float * tmp = (float *)_mm_malloc(nx*sizeof(float), 64); //input values prior to blur
		float * imgx = img;
		imgx += (nx * y);
		memcpy(tmp, imgx, nx*sizeof(float));
		for (int64_t x = 0; x < nx; x++ ) {
			float sum = 0;
			for (int64_t i = kStart[x]; i <= kEnd[x]; i++ ) 
				sum += tmp[x+i] * k[abs(i)];
			imgx[x] = sum * kWeight[x];
		}
		_mm_free (tmp);	
	}
	//free kernel
	_mm_free (k);
	_mm_free (kStart);
	_mm_free (kEnd);
	_mm_free (kWeight);
} //blurP

long timediff(clock_t t1, clock_t t2) {
    long elapsed;
    elapsed = ((double)t2 - t1) / CLOCKS_PER_SEC * 1000;
    return elapsed;
}

void smoothImg(float *v) {
	blurP(v, kX, kY * kZ * kZ, kXmm, kFWHMmm);
}//smoothImg()


void sqrt_tst(int reps) {
	int64_t n = kXYZT;
	float* vin = (float *)_mm_malloc(n*sizeof(float), 64);
	for (int64_t i = 0; i < n; i++)
		vin[i] = i+1;
	float* v = (float *)_mm_malloc(n*sizeof(float), 64);
	memcpy(v, vin, n*sizeof(float));
	smoothImg(v); //for timing, ignore first run - get CPU in floating point mode
	long mn = INT_MAX;
	long sum = 0.0;
	for (int64_t i = 0; i < reps; i++) {
		memcpy(v, vin, n*sizeof(float));
		//report real time, not CPU time
		//https://stackoverflow.com/questions/10874214/measure-execution-time-in-c-openmp-code
		//clock_t startTime = clock();
		struct timeval start, end;
		gettimeofday(&start, NULL);
		smoothImg(v);
		//long repTime = timediff(startTime, clock());
		gettimeofday(&end, NULL);
		long repTime = (((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e3);
		sum += repTime;
		mn = MIN(mn, repTime);
		//printf("run %lld: %ld\n", i, repTime);
	}
	printf("blur: min/mean\t%ld\t%ld\tms\n", mn, sum/reps);
	_mm_free (v);
	_mm_free (vin);
}

int main(int argc, char * argv[]) {
	int reps = 1; //how many times to repeat each test
	int nThread = 1;
	#if defined(_OPENMP)
	nThread = omp_get_max_threads(); 
	#endif
	if ( argc > 1 ) {
		reps = atoi(argv[1]);
		if ( argc > 2 ) {
			#ifdef _OPENMP
			nThread = atoi(argv[2]);
			#else
			printf("Only using 1 thread (not compiled with OpenMP support)\n");
			#endif
		}
	}
	#if defined(_OPENMP)
	printf("omp (with OpenMP ) %s \n", kCCsuf);
	printf("usage: omp [reps] [threads]\n");
	printf("example: omp 4 4\n");
	#else
	printf("omp (without OpenMP ) %s \n", kCCsuf);
	printf("usage: omp [reps]\n");
	printf("example: omp 4\n");
	#endif
	printf(" Reporting minimum time for %d repetitions with %d threads\n", reps, nThread);
	sqrt_tst(reps);
	return 0;
}
