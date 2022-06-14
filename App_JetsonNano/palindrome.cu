#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>
#include <time.h>

#define N 4 
#define M (10000)
#define THREADS_PER_BLOCK 1024

bool checkPalindromoHost(char* s){
    for(int i = 0; i < N; i++){
        if(s[i] != s[N-i]){
            return false;
        }
    }
    return true;
}

__global__ void checkPalindromoDevice(char* s){
    bool b;
    for(int i = 0; i < N; i++){
        if(s[i] != s[N-i]){
            b= false;
        }
    }
    b= true;
}

int main()
{
	clock_t start,end;
    char *a;
    int size = N * sizeof( char );
    a = (char *)malloc( size );

    for (int  i = 0;i<N;i++){
        a[i] = (random() % 26);
    }

    /*a[0] = 'a';
    a[1] = 'v';
    a[2] = 'v';
    a[3] = 'a';*/

    start = clock();
	bool b = checkPalindromoHost(a);
    end = clock();

    float time1 = ((float)(end-start))/CLOCKS_PER_SEC;
    printf("CPU: %f seconds\n",time1);

    start = clock();

    char *p;

    cudaMalloc( (void **) &p, size );
  
    cudaMemcpy( p, a, size, cudaMemcpyHostToDevice );
   
    bool pe;
    checkPalindromoDevice<<< (N + (THREADS_PER_BLOCK-1)) / THREADS_PER_BLOCK, THREADS_PER_BLOCK >>>( p);

    free(a);
   
    cudaFree( p );  

    end = clock();
    float time2 = ((float)(end-start))/CLOCKS_PER_SEC;
    printf("CUDA: %f seconds, Speedup: %f\n",time2, time1/time2);

    return 0;
}
