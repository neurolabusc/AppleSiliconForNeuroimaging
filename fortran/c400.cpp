/* ----------------------------------------------------------------------
g++ -O3 -o c400 c400.cpp; time ./c400 4


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

#define kXYZT 104*90*72*400 //= total number of voxels across all volumes

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



long timediff(clock_t t1, clock_t t2) {
    long elapsed;
    elapsed = ((double)t2 - t1) / CLOCKS_PER_SEC * 1000;
    return elapsed;
}

void sqrtV(float *v, int64_t n) {
//vectorize square-root, fslmaths "-sqrt"
	#pragma omp parallel for
	for (int64_t i = 0; i < n; i++ )
      		v[i] = sqrt(v[i]);
}//sqrtV()


void sqrt_tst(int reps) {
	int64_t n = kXYZT;
	float* vin = (float *)_mm_malloc(n*sizeof(float), 64);
	for (int64_t i = 0; i < n; i++)
		vin[i] = i+1;
	float* v = (float *)_mm_malloc(n*sizeof(float), 64);
	memcpy(v, vin, n*sizeof(float));
	sqrtV(v, n); //for timing, ignore first run - get CPU in floating point mode
	long mn = INT_MAX;
	long sum = 0.0;
	for (int64_t i = 0; i < reps; i++) {
		memcpy(v, vin, n*sizeof(float));
		//report real time, not CPU time
		//https://stackoverflow.com/questions/10874214/measure-execution-time-in-c-openmp-code
		//clock_t startTime = clock();
		struct timeval start, end;
		gettimeofday(&start, NULL);
		sqrtV(v, n);
		//long repTime = timediff(startTime, clock());
		gettimeofday(&end, NULL);
		long repTime = (((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e3);
		sum += repTime;
		mn = MIN(mn, repTime);
		//printf("run %lld: %ld\n", i, repTime);
	}
	printf("sqrt32: min/mean\t%ld\t%ld\tms\n", mn, sum/reps);
	_mm_free (v);
	_mm_free (vin);
}

void sqrt64V(double *v, int64_t n) {
//vectorize square-root, fslmaths "-sqrt"
	#pragma omp parallel for
	for (int64_t i = 0; i < n; i++ )
      		v[i] = sqrt(v[i]);
}//sqrtV()


void sqrt64_tst(int reps) {
	int64_t n = kXYZT;
	double* vin = (double *)_mm_malloc(n*sizeof(double), 64);
	for (int64_t i = 0; i < n; i++)
		vin[i] = i+1;
	double* v = (double *)_mm_malloc(n*sizeof(double), 64);
	memcpy(v, vin, n*sizeof(double));
	sqrt64V(v, n); //for timing, ignore first run - get CPU in doubleing point mode
	long mn = INT_MAX;
	long sum = 0.0;
	for (int64_t i = 0; i < reps; i++) {
		memcpy(v, vin, n*sizeof(double));
		//report real time, not CPU time
		//https://stackoverflow.com/questions/10874214/measure-execution-time-in-c-openmp-code
		//clock_t startTime = clock();
		struct timeval start, end;
		gettimeofday(&start, NULL);
		sqrt64V(v, n);
		//long repTime = timediff(startTime, clock());
		gettimeofday(&end, NULL);
		long repTime = (((end.tv_sec  - start.tv_sec) * 1000000u + end.tv_usec - start.tv_usec) / 1.e3);
		sum += repTime;
		mn = MIN(mn, repTime);
		//printf("run %lld: %ld\n", i, repTime);
	}
	printf("sqrt64: min/mean\t%ld\t%ld\tms\n", mn, sum/reps);
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
	printf("c400 (with OpenMP ) %s \n", kCCsuf);
	printf("usage: omp [reps] [threads]\n");
	printf("example: omp 4 4\n");
	#else
	printf("c400 (without OpenMP ) %s \n", kCCsuf);
	printf("usage: omp [reps]\n");
	printf("example: omp 4\n");
	#endif
	printf(" Reporting minimum time for %d repetitions with %d threads\n", reps, nThread);
	sqrt_tst(reps);
	sqrt64_tst(reps);
	return 0;
}
