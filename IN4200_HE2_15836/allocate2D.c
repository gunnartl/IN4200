#include <stdio.h>
#include <stdlib.h>

void allocate2D(int M,int N,float ***table){
	*table    = (float**)malloc(M *sizeof(float*));
	(*table)[0] = (float*)calloc(M * N,sizeof(float)); //calloc for zeros

    for (int i = 0; i < M; i++) (*table)[i] = &(*table)[0][N*i];
}