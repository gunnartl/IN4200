#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#include "allocate2D.c"
#include "deallocate2D.c"
#include "single_layer_convolution.c"
#include "MPI_double_layer_convolution.c"

void allocate2D(int M,int N,float ***table);
void deallocate2D(float ***table);

void MPI_double_layer_convolution (int M, int N, float **input, int K1, float **kernel1, int K2, float **kernel2,float **output);
void single_layer_convolution (int M, int N, float **input, int K, float **kernel,float **output);