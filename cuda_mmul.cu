#undef DT32
//#define DT32 //<- This should be the ONLY difference between core32 and core64!

#ifdef DT32
 #define flt float
#else
 #define flt double
#endif
/*
https://github.com/sol-prog/cuda_cublas_curand_thrust
https://solarianprogrammer.com/2012/05/31/matrix-multiplication-cuda-cublas-curand-thrust/

nvcc cuda_mmul.cu -lcublas -lcurand -o cuda_mmul; ./cuda_mmul
matrix multiplication 10 repetitions 32-bit
mmul: min/mean	2367	2375	ms
mmulCUDA: min/mean	22	32	ms
7772416 values, differences 63.0876%, max difference 8.38861e+06
mmul>nvcc cuda_mmul.cu -lcublas -lcurand -o cuda_mmul; ./cuda_mmul
matrix multiplication 10 repetitions 64-bit
mmul: min/mean	2370	2375	ms
mmulCUDA: min/mean	60	70	ms
7772416 values, differences 0%, max difference 0
*/
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cublas_v2.h>
#include <curand.h>

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

// Fill the array A(nr_rows_A, nr_cols_A) with random numbers on GPU
void GPU_fill_rand(flt *A, int nr_rows_A, int nr_cols_A) {
	// Create a pseudo-random number generator
	curandGenerator_t prng;
	curandCreateGenerator(&prng, CURAND_RNG_PSEUDO_DEFAULT);

	// Set the seed for the random number generator using the system clock
	curandSetPseudoRandomGeneratorSeed(prng, (unsigned long long) clock());

	// Fill the array with random numbers on the device
	#ifdef DT32
	curandGenerateUniform(prng, A, nr_rows_A * nr_cols_A);
	#else
	curandGenerateUniformDouble(prng, A, nr_rows_A * nr_cols_A);
	#endif
}

//naive matrix multiplication, for optimization see http://apfel.mathematik.uni-ulm.de/~lehn/sghpc/gemm/
void mmul(const flt * A, size_t  IA, const flt * B, size_t  IB, flt * C, size_t  IC, size_t M, size_t N, size_t P) {
 /*
    A is regarded as a two-dimensional matrix with dimemnsions [M][P]
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
        C[m][n] = sum(A[m][p] * B[p][n], 0 <= p < P);
*/

	    for (size_t m = 0; m < M; ++m) {
			size_t mP = m * P;
	    	for (size_t n = 0; n < N; ++n) {
				flt ret = 0.0;
				for (size_t p = 0; p < P; ++p)
	        		ret += A[mP + p] * B[p*N + n];
				C[m*N + n] = ret;
			} //for n
		} //for m
}


// Multiply the arrays A and B on GPU and save the result in C
void gpu_blas_mmul(const flt *A, const flt *B, flt *C, const int m, const int n, const int p) {
	int lda=m,ldb=p,ldc=m;
	const flt alf = 1;
	const flt bet = 0;
	const flt *alpha = &alf;
	const flt *beta = &bet;

	// Create a handle for CUBLAS
	cublasHandle_t handle;
	cublasCreate(&handle);

	// Do the actual multiplication
	#ifdef DT32
	cublasSgemm(handle, CUBLAS_OP_N, CUBLAS_OP_N, m, n, p, alpha, A, lda, B, ldb, beta, C, ldc);
	#else
	cublasDgemm(handle, CUBLAS_OP_N, CUBLAS_OP_N, m, n, p, alpha, A, lda, B, ldb, beta, C, ldc);
	
	//cublasDgemm(handle, CUBLAS_OP_T, CUBLAS_OP_T, m, n, p, alpha, A, lda, B, ldb, beta, C, ldc);
	#endif
	// Destroy the handle
	cublasDestroy(handle);
}

//Print matrix A(nr_rows_A, nr_cols_A) storage in column-major format
void print_matrix(const flt *A, int nr_rows_A, int nr_cols_A) {

    for(int i = 0; i < nr_rows_A; ++i){
        for(int j = 0; j < nr_cols_A; ++j){
            std::cout << A[j * nr_rows_A + i] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

int main() {
	size_t m = 485776; //<- voxels
	size_t n = 16; //statistical contrast, e.g "1 0 0" 
	size_t p = 120; //<- shared: participants
	size_t reps = 10;
	printf("matrix multiplication %zu repetitions %llu-bit\n",  reps, (unsigned long long) sizeof(flt)*8);
	
	// Allocate 3 arrays on CPU
	flt *a = (flt *)malloc(m * p * sizeof(flt));
	flt *b = (flt *)malloc(p * n * sizeof(flt));
	flt *cGPU = (flt *)malloc(m * n * sizeof(flt));
	flt *c = (flt *)malloc(m * n * sizeof(flt));
	//
	for (size_t i = 0; i < (m * p); i++)
    		a[i] = (flt)i;//(flt) rand()/RAND_MAX;
	for (size_t i = 0; i < (p* n); i++)
    		b[i] = (flt)i;//(flt) rand()/RAND_MAX;	
 
    //CPU solution:
    long mn = INT_MAX;
	long sum = 0.0;
	for (int64_t i = 0; i < reps; i++) {
		double startTime = clockMsec();
    		mmul(a, 1, b, 1, c, 1, m, n, p);
    		mn = MIN(mn, timediff(startTime, clockMsec()));	
		sum += timediff(startTime, clockMsec());
    }
    printf("mmul: min/mean\t%ld\t%ld\tms\n", mn, sum/reps);
	// Allocate 3 arrays on GPU
	flt *d_A, *d_B, *d_C;
	

	cudaMalloc(&d_A, m * p * sizeof(flt));
	cudaMalloc(&d_B, p * n * sizeof(flt));
	cudaMalloc(&d_C, m * n * sizeof(flt));
    
    mn = INT_MAX;
	sum = 0.0;
	for (int64_t i = 0; i < reps; i++) {
		double startTime = clockMsec();
	// Transfer data to GPU
	cudaMemcpy(d_A, a, m * p * sizeof(flt),cudaMemcpyHostToDevice);
	cudaMemcpy(d_B, b, p * n * sizeof(flt),cudaMemcpyHostToDevice);
	
	// Multiply A and B on GPU
	//https://docs.nvidia.com/cuda/cublas/index.html
	// since matrices stored in column-major format
	// we compute "C = B * A" instead of "C = A * B"
	gpu_blas_mmul(d_B, d_A, d_C, n, m, p);
	//gpu_blas_mmul(d_A, d_B, d_C, m, n, p);

	// Copy (and print) the result on host memory
	cudaMemcpy(cGPU,d_C,m * n * sizeof(flt),cudaMemcpyDeviceToHost);
    		mn = MIN(mn, timediff(startTime, clockMsec()));	
		sum += timediff(startTime, clockMsec());
    }
    printf("mmulCUDA: min/mean\t%ld\t%ld\tms\n", mn, sum/reps);
 
	//Free GPU memory
	cudaFree(d_A);
	cudaFree(d_B);
	cudaFree(d_C);	

	//#define dbug
	#ifdef dbug
	std::cout << "A =" << std::endl;
	print_matrix(a, p, m);
	std::cout << "B =" << std::endl;
	print_matrix(b, n, p);
	std::cout << "C(cpu) =" << std::endl;
	print_matrix(c, n, m);
	std::cout << "C(gpu) =" << std::endl;
	print_matrix(cGPU, n, m);

	#endif
	//check results
	size_t nDiff = 0;
    	flt mxDiff = (flt) 0.0;
	for (size_t i = 0; i < (m * n); i++) {
		if (c[i] != cGPU[i]) {
			nDiff ++;
			mxDiff = MAX(mxDiff, fabs(c[i] - cGPU[i]) );
		}
	}
	printf("%zu values, differences %g%%, max difference %g\n", (m * n), ((double) nDiff) / ((double) (m*n)) * 100.0, mxDiff);

	// Free CPU memory
	free(a);
	free(b);
	free(c);
	free(cGPU);
	return 0;
}

