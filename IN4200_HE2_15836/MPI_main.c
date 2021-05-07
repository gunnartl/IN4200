#include "MPI_main.h"

int main (int nargs, char **args)
{
	int M=0, N=0, K1=0, K2=0, my_rank;
	float **input=NULL, **output=NULL, **kernel1=NULL,**kernel2=NULL;

	MPI_Init (&nargs, &args);
	MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
	if (my_rank==0) {
		// read from command line the values of M, N, and K
		M  = atoi(args[1]);
		N  = atoi(args[2]);
		K1 = atoi(args[3]);
		K2 = atoi(args[4]);

	    // allocate2D 2D array ’input’ with M rows and N columns
	    allocate2D(M,N,&input); 

		// allocate2D 2D array ’output’ with M-K+1 rows and N-K1-K2+1 columns
		//allocate2D(M-K1-K2+2,N-K1-K2+2,&output);
		allocate2D(M-K1-K2+2,N-K1-K2+2,&output);

		// allocate2D the convolutional kernel with K rows and K columns
		allocate2D(K1,K1,&kernel1);
		allocate2D(K2,K2,&kernel2);

		// fill 2D array ’input’ with some values
		for (int i = 0; i < M; ++i)
		{
			for (int j = 0; j < N; ++j)
			{
				if(j<N/2) input[i][j] = i*j/.3;
				//printf("%f ", input[i][j]);
			}
			//printf("\n");
		}
		// fill kernel with some values
		for (int i = 0; i < K1; ++i){
			kernel1[i][0] = 1;
			kernel1[i][2] = -1;

		}
		for (int i = 0; i < K2; ++i)
		{
			kernel2[i][0] = 1;
			kernel2[i][K2-1] = -1;
		}
	}


	// process 0 broadcasts values of M, N, K to all the other processes
	// ...
	MPI_Bcast(&M,1, MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&N,1, MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&K1,1, MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&K2,1, MPI_INT,0,MPI_COMM_WORLD);


	if (my_rank>0) {
	allocate2D(K1,K1,&kernel1);
	allocate2D(K2,K2,&kernel2);
	}


	// process 0 broadcasts the content of kernel to all the other processes
	MPI_Bcast(&kernel1[0][0],K1*K1, MPI_FLOAT,0,MPI_COMM_WORLD);
	MPI_Bcast(&kernel2[0][0],K2*K2, MPI_FLOAT,0,MPI_COMM_WORLD);


	// parallel computation of a double-layer convolution

	double start,pstop,sstop;
	start= MPI_Wtime();
	
	
	MPI_double_layer_convolution(M, N, input, K1, kernel1,K2, kernel2, output);
	pstop = MPI_Wtime()-start;
    if (my_rank==0){
	printf("parallel:  %f s\n", pstop);

	// For example, compare the content of array ’output’ with that is
	// produced by the sequential function single_layer_convolution
	
	float **compare_out, **intermediate=NULL;	
	allocate2D(M-K1+1,N-K1+1,&intermediate);
	allocate2D(M-K1-K2+2,N-K1-K2+2,&compare_out);


	start= MPI_Wtime();
	single_layer_convolution(M,N,input,K1,kernel1,intermediate);
	single_layer_convolution(M-K1+1,N-K1+1,intermediate,K2,kernel2,compare_out);
	sstop = MPI_Wtime()-start;
	printf("serial:  %f s\n", sstop);
	printf("speedup:  %f\n",sstop/pstop);

	int testvariable = 0;
	for(int i =0;i < M-K1-K2+2;i++){
		for(int j =0;j < N-K1-K2+2;j++){
			if(compare_out[i][j] !=	output[i][j]){
				testvariable ++;
			}
		}
	}

	if (testvariable != 0)
	{
		printf("Error in double convolution \n");
	}
	else{
		printf("Double convolution executed successfully \n");
	}

	deallocate2D(&intermediate);
	deallocate2D(&compare_out);
	deallocate2D(&input);
	deallocate2D(&output);
	}
    
    deallocate2D(&kernel1);
	deallocate2D(&kernel2);
	
	MPI_Finalize();
	return 0;
}
