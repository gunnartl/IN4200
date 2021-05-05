#include <stdio.h>
#include <stdlib.h>
//#include <mpi.h>

void allocate(int M,int N,float ***table);

int main (int nargs, char** args)
{
	/*int size, my_rank;

	MPI_Init (&nargs, &args);
	MPI_Comm_size (MPI_COMM_WORLD, &size);
	MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
	
	printf("Hello world, I’ve rank %d out of %d procs.\n",
	my_rank,size);
	MPI_Finalize ();*/

	float **test;

	allocate(3,4,&test);
	test[4][3]=5;

	free(test[0]);
	free(test);
}

void allocate(int M,int N,float ***table){
	*table    = (float**)malloc(M *sizeof(float*));
	(*table)[0] = (float*)calloc(M * N,sizeof(float)); //calloc for zeros

    for (int i = 0; i < M; i++) (*table)[i] = &(*table)[0][N*i];
}