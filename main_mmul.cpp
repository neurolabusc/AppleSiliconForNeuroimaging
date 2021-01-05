#undef DT32
//#define DT32 //<- This should be the ONLY difference between core32 and core64!

#ifdef DT32
 #define flt float
#else
 #define flt double
#endif

/* 
This program tests matrix multiplication for a naive C implementation versus a blas optimized for the architecture.

The test simulates the inner loop for General Linear Model t-tests, which are computed thousands of times for permutation threshod testing
 m = 485776; //<- voxels
 n = 16; //statistical contrast, e.g "1 0 0" 
 p = 120; //<- shared: participants
    A[m][p]         A[(m*P+p)*IA]
    B[p][n]         B[(p*N+n)*IB]
    C[m][n] 	    C[(m*N+n)*IC]
 c = a * b

To compile on MacOS: M1 MacBook Air

g++ -O3 -o tst main_mmul.cpp  -framework Accelerate -target arm64-apple-macos11 -mmacosx-version-min=11.0 -I/Library/Developer/CommandLineTools/SDKs/MacOSX11.1.sdk/System/Library/Frameworks/Accelerate.framework/Versions/A/Frameworks/vecLib.framework/Versions/A/Headers

export VECLIB_MAXIMUM_THREADS=1

time ./tst
matrix multiplication 10 repetitions 64-bit
mmul: min/mean	530	531	ms
mmulBLAS: min/mean	16	18	ms
mmulDSP: min/mean	16	20	ms
differences 36.8881%, max difference 2.13163e-14
./tst  6.06s user 0.10s system 99% cpu 6.158 tot

To compile on Ubuntu: Ryzen 3900X

sudo apt-get install libblas-dev
c++ -O3 main_mmul.cpp -lblas -o tst; ./tst
mmul: min/mean	279	281	ms
mmulBLAS: min/mean	86	86	ms
differences 36.9339%, max difference 2.13163e-14

This test can be modified to evaluate gsl_blas_dgemm by setting
 #include <gsl/gsl_blas.h>
and using
 gsl_blas_dgemm
gsl_blas seems to underperform the naive implementation

c++ -O3 main_mmul.cpp -lgslcblas -o tst; ./tst
matrix multiplication 10 repetitions 64-bit
mmul: min/mean	282	284	ms
mmulBLAS: min/mean	820	821	ms
differences 0%, max difference 0

This test can be modified to evaluate Intel MKL by setting
  #include "mkl.h"
https://software.intel.com/content/www/us/en/develop/documentation/mkl-tutorial-c/top/multiplying-matrices-using-dgemm.html


 */
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#ifdef __APPLE__    
	#include <vDSP.h>
#endif
#include <cblas.h>
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

#ifdef __x86_64__    
	#include <immintrin.h>
#else 
	void *_mm_malloc(size_t size, size_t align)
	{
	    void *ptr;
	    if (align == 1)
	        return malloc(size);
	    if (align == 2 || (sizeof(void *) == 8 && align == 4))
	        align = sizeof(void *);
	    if (!posix_memalign(&ptr, align, size))
	        return ptr;
	    return NULL;
	}

	void _mm_free(void *addr)
	{
	    free(addr);
	}
#endif

#ifndef MAX
#define MAX(A,B) ((A) > (B) ? (A) : (B))
#endif

#ifndef MIN
#define MIN(A,B) ((A) > (B) ? (B) : (A))
#endif

double clockMsec() { //return milliseconds since midnight
	struct timespec _t;
	clock_gettime(CLOCK_MONOTONIC, &_t);
	return _t.tv_sec*1000.0 + (_t.tv_nsec/1.0e6);
}

long timediff(double startTimeMsec, double endTimeMsec) {
	return round(endTimeMsec - startTimeMsec);
}


//naive matrix multiplication, for optimization see http://apfel.mathematik.uni-ulm.de/~lehn/sghpc/gemm/
//void mmul(const flt * __restrict__ A, size_t  IA, const flt * __restrict__ B, size_t  IB, flt * __restrict__ C, size_t  IC, size_t M, size_t N, size_t P) {
void mmul(const flt * A, size_t  IA, const flt * B, size_t  IB, flt * C, size_t  IC, size_t M, size_t N, size_t P) {
 /*   A is regarded as a two-dimensional matrix with dimemnsions [M][P]
    and stride IA.  B is regarded as a two-dimensional matrix with
    dimemnsions [P][N] and stride IB.  C is regarded as a
    two-dimensional matrix with dimemnsions [M][N] and stride IC.

    Pseudocode:     Memory:
    A[m][p]         A[(m*P+p)*IA]
    B[p][n]         B[(p*N+n)*IB]
    C[m][n]         C[(m*N+n)*IC]
These compute:
    for (m = 0; m < M; ++m)
    for (n = 0; n < N; ++n)
        C[m][n] = sum(A[m][p] * B[p][n], 0 <= p < P);*/
	if ((IA != 1) || (IB != 1) || (IC != 1)) { //deal with stride
	    for (int m = 0; m < M; ++m) {
			size_t mP = m * P;
	    	for (size_t n = 0; n < N; ++n) {
				flt ret = 0.0;
				for (size_t p = 0; p < P; ++p)
	        		ret += A[(mP + p) * IA] * B[(p*N + n)*IB];
				C[(m*N + n) * IC] = ret;
			} //for n
		} //for m
		return;	
	} 
	//#define cptr //optional C pointer trick does not seem to help with modern compilers at high optimization levels
	#ifdef cptr
	flt * Cp = C;
    for (size_t m = 0; m < M; ++m) {
		size_t mP = m * P;
    	for (size_t n = 0; n < N; ++n) {
			flt * Ap = (flt *) A;
			Ap += mP;
			flt * Bp = (flt *) B;
			Bp += n;
			flt ret = 0.0;
			for (size_t p = 0; p < P; ++p) {
        		//ret += A[mP + p] * B[p*N + n];
				ret += Ap[0] * Bp[0];
				Ap ++;
				Bp += N;
			}
			//C[m*N + n] = ret;
			Cp[0] = ret;
			Cp ++;
		} //for n
	} //for m	
	#else
	    for (size_t m = 0; m < M; ++m) {
			size_t mP = m * P;
	    	for (size_t n = 0; n < N; ++n) {
				flt ret = 0.0;
				for (size_t p = 0; p < P; ++p)
	        		ret += A[mP + p] * B[p*N + n];
				C[m*N + n] = ret;
			} //for n
		} //for m
	#endif
}

void svesq(const flt *A, size_t IA, flt *C, size_t N) {
// Sum of vector elements' squares.
	flt sumSq = 0.0;
	for (size_t n = 0; n < N; ++n)
		sumSq += A[n] * A[n]; 
	C[0] = sumSq;
}

/*
//Matlab equivalence
maxNumCompThreads(1)
m = 485776; % voxels
n = 16; %statistical contrast, e.g "1 0 0" 
p = 120; % shared: participants
reps = 10;
a = rand(m, p);
b = rand(p, n);
r = zeros(m,n);
sm = 0.0; %sum time for all iterations
mn = inf; %minimum time for fastest iteration
for i = 1:reps 
	startTime = tic;
	r = a * b;
	s = sum(r(:) .* r(:)); % sum squared error
	tm = toc(startTime) * 1000.0; %elapsed in in ms
	sm = sm + tm;
	mn = min(tm, mn);
end
fprintf('mmul: min/mean\t%g\t%g\tms\n', mn, sm/reps);
*/

void tst_mmul(int reps) {
	/*
    A[m][p]         A[(m*P+p)*IA]
    B[p][n]         B[(p*N+n)*IB]
    C[m][n] 	    C[(m*N+n)*IC]
	*/
	printf("matrix multiplication %d repetitions %llu-bit\n",  reps, (unsigned long long) sizeof(flt)*8);
	size_t m = 485776; //<- voxels
	size_t n = 16; //statistical contrast, e.g "1 0 0" 
	size_t p = 120; //<- shared: participants
	//#define dbug
	#ifdef dbug
	m = 3;
	n = 3;
	p = 3;
	#endif
	flt *a = (flt *) _mm_malloc(m * p * sizeof(flt),64);
	flt *b = (flt *) _mm_malloc(p * n * sizeof(flt),64);
    flt *c = (flt *) _mm_malloc(m * n * sizeof(flt),64);
    flt *cBLAS = (flt *) _mm_malloc(m * n * sizeof(flt),64);
	flt *cDSP = (flt *) _mm_malloc(m * n * sizeof(flt),64);	
	//flt ss, ssDSP; 
	for (size_t i = 0; i < (m * p); i++)
    	a[i] = (flt) rand()/RAND_MAX;
	for (size_t i = 0; i < (p * n); i++)
    	b[i] = (flt) rand()/RAND_MAX;
	#ifdef dbug
	for (size_t i = 0; i < (m * p); i++)
    	a[i] = i;
	for (size_t i = 0; i < (p * n); i++)
    	b[i] = i;	
	#endif
	long mn = INT_MAX;
	long sum = 0.0;
	long mnDSP = INT_MAX;
	long sumDSP = 0.0;
	long mnBLAS = INT_MAX;
	long sumBLAS = 0.0;
	for (int64_t i = 0; i < reps; i++) {
		double startTime = clockMsec();
		#ifdef __APPLE__ 
		    #ifdef DT32
				vDSP_mmul(a, 1, b, 1, cDSP, 1, m, n, p);
				//vDSP_svesq(a, 1, &ssDSP, m * n);
			#else
				vDSP_mmulD(a, 1, b, 1, cDSP, 1, m, n, p);
				//vDSP_svesqD(a, 1, &ssDSP, m * n);
			#endif
			mnDSP = MIN(mnDSP, timediff(startTime, clockMsec()));	
			sumDSP += timediff(startTime, clockMsec());
			startTime = clockMsec();
		#endif
		//blas
		flt alpha = 1.0;
		flt beta = 0.0;
		#ifdef DT32
		cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, m, n, p, alpha, a, p, b, n, beta, cBLAS, n);
		#else 
		cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, m, n, p, alpha, a, p, b, n, beta, cBLAS, n);
		#endif
		mnBLAS = MIN(mnBLAS, timediff(startTime, clockMsec()));	
		sumBLAS += timediff(startTime, clockMsec());
        //naive C
		startTime = clockMsec();
		mmul(a, 1, b, 1, c, 1, m, n, p);
		//svesq(a, 1, &ss, m * n);
		mn = MIN(mn, timediff(startTime, clockMsec()));	
		sum += timediff(startTime, clockMsec());
	}
	printf("mmul: min/mean\t%ld\t%ld\tms\n", mn, sum/reps);
	printf("mmulBLAS: min/mean\t%ld\t%ld\tms\n", mnBLAS, sumBLAS/reps);
	#ifdef __APPLE__ 
	printf("mmulDSP: min/mean\t%ld\t%ld\tms\n", mnDSP, sumDSP/reps);	
	#endif
    //check results
	size_t nDiff = 0;
    flt mxDiff = 0.0;
	for (size_t i = 0; i < (m * n); i++) {
		#ifdef dbug
			#ifdef __APPLE__ 
			printf("%lu %g %g %g\n", i,c[i], cBLAS[i], cDSP[i]);
			#else
			printf("%lu %g %g\n", i,c[i], cBLAS[i]);
			#endif
		#endif
		if (c[i] != cBLAS[i]) {
			nDiff ++;
			mxDiff = MAX(mxDiff, fabs(c[i] - cBLAS[i]) );
		}
	}
    //printf("differences %g%%, max difference %g sum-square difference %g\n", ((double) nDiff) / ((double) (m*n)) * 100.0, mxDiff, fabs(ssDSP - ss));
	printf("differences %g%%, max difference %g\n", ((double) nDiff) / ((double) (m*n)) * 100.0, mxDiff);
	_mm_free (a);
	_mm_free (b);
	_mm_free (c);
    _mm_free (cBLAS);
	_mm_free (cDSP);										
} //tst_mmul()

int main(int argc, char * argv[]) {
	tst_mmul(10);
	return 0;
}
