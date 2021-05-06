#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

void allocate(int M,int N,float ***table);
void deallocate(float ***table);

void MPI_double_layer_convolution (int M, int N, float **input, int K1, float **kernel1, int K2, float **kernel2,float **output);
void single_layer_convolution (int M, int N, float **input, int K, float **kernel,float **output);

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

	    // allocate 2D array ’input’ with M rows and N columns
	    allocate(M,N,&input);

	    printf("%d\n", my_rank); 

		// allocate 2D array ’output’ with M-K+1 rows and N-K1-K2+1 columns
		//allocate(M-K1-K2+2,N-K1-K2+2,&output);
		allocate(M-K1-K2+2,N-K1-K2+2,&output);

		// allocate the convolutional kernel with K rows and K columns
		allocate(K1,K1,&kernel1);
		allocate(K2,K2,&kernel2);

		// fill 2D array ’input’ with some values
		for (int i = 0; i < M; ++i)
		{
			for (int j = 0; j < N; ++j)
			{
				if(j< 4) input[i][j] = 10;
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
		//single_layer_convolution(M,N,input,K1,kernel1,output);


		printf("==================================================================================\n");
	}


	// process 0 broadcasts values of M, N, K to all the other processes
	// ...
	MPI_Bcast(&M,1, MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&N,1, MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&K1,1, MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&K2,1, MPI_INT,0,MPI_COMM_WORLD);
	if (my_rank>0) {
	allocate(K1,K1,&kernel1);
	allocate(K2,K2,&kernel2);
	}
	// process 0 broadcasts the content of kernel to all the other processes
	MPI_Bcast(&kernel1[0][0],K1*K1, MPI_FLOAT,0,MPI_COMM_WORLD);
	MPI_Bcast(&kernel2[0][0],K2*K2, MPI_FLOAT,0,MPI_COMM_WORLD);
	// parallel computation of a double-layer convolution
	MPI_double_layer_convolution (M, N, input, K1, kernel1,K2, kernel2, output);
	if (my_rank==0) {
	for (int i = 0; i < M-K1-K2+2; ++i)
		{
			for (int j = 0; j < N-K1-K2+2; ++j)
			{
				printf("%f ",output[i][j]);
			}
			printf("\n");
		}
	// For example, compare the content of array ’output’ with that is
	// produced by the sequential function single_layer_convolution
	
	float **compare_out, **intermediate=NULL;	
	allocate(M-K1+1,N-K1+1,&intermediate);
	allocate(M-K1-K2+2,N-K1-K2+2,&compare_out);

	single_layer_convolution(M,N,input,K1,kernel1,intermediate);
	single_layer_convolution(M-K1+1,N-K1+1,intermediate,K2,kernel2,compare_out);
	
	for(int i =0;i < M-K1-K2+2;i++){
		for(int j =0;j < N-K1-K2+2;j++){
			if(compare_out[i][j] !=	output[i][j]){
				printf("This is a terrible program");
			}
		}
	}	

	deallocate(&intermediate);
	deallocate(&compare_out);
	deallocate(&input);
	deallocate(&output);
	}
    
    deallocate(&kernel1);
	deallocate(&kernel2);
	
	MPI_Finalize();
	return 0;
}

void allocate(int M,int N,float ***table){
	*table    = (float**)malloc(M *sizeof(float*));
	(*table)[0] = (float*)calloc(M * N,sizeof(float)); //calloc for zeros

    for (int i = 0; i < M; i++) (*table)[i] = &(*table)[0][N*i];
}

void deallocate(float ***table){
	free(*table[0]);
	free(*table);
}


void MPI_double_layer_convolution (int M, int N, float **input, int K1, float **kernel1, int K2, float **kernel2,float **output){
	int my_rank,size;
	MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size (MPI_COMM_WORLD, &size);
	int my_M;
	int overlap_size1 = K1-1;    // number of overlapping rows
	int overlap_size2 = K2-1;
	float **my_input,**my_output;
    
    //sets the nummber of rows for all procs. integerdivision of rows, splits remainder of k between k first processes. and adds overlap
    my_M = (my_rank<(M-K1+1)%size)? (M-K1+1)/size + overlap_size1+1 :(M-K1+1)/size + overlap_size1;

    printf("%d\n", my_M);
    allocate(my_M,N,&my_input);
    allocate(my_M-overlap_size1,N-K1+1,&my_output);
	//first convolution
	if(my_rank == 0){

	int n_each = (M-K1+1)/size; // lines each
	int n_more = (M-K1+1)%size; //number of procs getting one more line

	int *counts = (int*)calloc(size,sizeof(int));
	int *displacements = (int*)calloc(size,sizeof(int));

	//calculates nummber of floats for each process 
	printf(" HER KOMMER COUNTS \n");
	for (int i = 0; i < size; ++i)
	{
		counts[i] = (i<n_more)? (n_each + overlap_size1+1)*N:(n_each + overlap_size1)*N; //could gather my_M but this is faster
		printf(" %d \n", counts[i]);
	}
	
	//sets displacements witch is lenght of counts[i-1] - overlappsize
	printf(" HER KOMMER DISPS \n");
	for (int i = 1; i < size; ++i)
	{
		displacements[i] = displacements[i-1]+counts[i-1]-overlap_size1*N;
		printf(" %d \n", displacements[i]);
	}

	//scatters input to procs with sizes and sisplacements already calculated 
    MPI_Scatterv(&input[0][0],counts,displacements,MPI_FLOAT,&my_input[0][0],my_M*N,MPI_FLOAT,0,MPI_COMM_WORLD);

	}

	if (my_rank>0)
	{	

		//recieving call from all other procs
		MPI_Scatterv(NULL,NULL,NULL,NULL,&my_input[0][0],my_M*N,MPI_FLOAT,0,MPI_COMM_WORLD);
		
	}

	for (int i = 0; i < my_M; ++i)
		{
			for (int j = 0; j < N; ++j)
			{
				printf("%.f - ", my_input[i][j]);
			}
			printf("  %d \n", my_rank);
		}
	// all procs
	MPI_Barrier(MPI_COMM_WORLD);
	//Im using the function given in the exam-text. The alternative is just to copypaste it, and this leaves a much cleaner code:
	single_layer_convolution(my_M,N,my_input,K1,kernel1,my_output);
	for (int i = 0; i < my_M-K1+1; ++i)
		{
			for (int j = 0; j < N-K1+1; ++j)
			{
				printf("%.f ",my_output[i][j]);
			}
			printf(" %d \n",my_rank);
		}


	//allocates room for the second input witch is the size of the last input - K1+1, pluss an overlapp for all other processes than the last
	my_M = (my_rank==size-1)?my_M - K1+1 : my_M - K1+1+overlap_size2;
	N = N - K1+1;
	deallocate(&my_input);
	allocate(my_M,N,&my_input);
	
	if(my_rank==size-1){
		for (int i = 0; i < my_M; ++i){
			for (int j = 0; j < N; ++j)
			{
				my_input[i][j] = my_output[i][j];
			}
	}
	}
	else{
		for (int i = 0; i < my_M-overlap_size2; ++i)
		{
			for (int j = 0; j < N; ++j)
			{
				my_input[i][j] = my_output[i][j];
			}
	}
	}
	deallocate(&my_output);
	allocate(my_M-overlap_size2, N-overlap_size2,&my_output);

	if(my_rank == 0){
		MPI_Recv(&my_input[my_M-overlap_size2][0], N*overlap_size2,MPI_FLOAT, 1, my_rank+1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	}
	else if (my_rank == size-1){
		MPI_Send(&my_input[0][0], N*overlap_size2,MPI_FLOAT,my_rank-1,my_rank,MPI_COMM_WORLD);
	}

	else {
	    MPI_Sendrecv(&my_input[0][0], N*overlap_size2,MPI_FLOAT,my_rank-1,my_rank,&my_input[my_M-overlap_size2][0],N*overlap_size2,MPI_FLOAT,my_rank+1,my_rank+1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    }


    
	MPI_Barrier(MPI_COMM_WORLD);
	if (my_rank==0)
	{
		printf("=====================================================\n");
	}
    MPI_Barrier(MPI_COMM_WORLD);
	single_layer_convolution(my_M,N,my_input,K2,kernel2,my_output); 
	for (int i = 0; i < my_M-K2+1; ++i)
		{
			for (int j = 0; j < N-K2+1; ++j)
			{
				printf("%.f ",my_output[i][j]);
			}
			printf(" %d \n",my_rank);
		}


	my_M = my_M-K2+1;
	N = N-K2+1;
	if (my_rank==0){

		//again calculating amount of lines for each process.

		//for some reason n_each and other variables from last calulation is not available anymore
		//and the compiler says they are undeclared?? if you have time pleace tell my why. 

		int n_each = (M-K1+1)/size; // lines each

		int n_more = (M-K1+1)%size; //number of procs getting one more line

		int *counts = (int*)calloc(size,sizeof(int));
		int *displacements = (int*)calloc(size,sizeof(int));
		
		for (int i = 0; i < size; ++i)
		{
			counts[i] = (i<n_more)? (n_each+1)*N:n_each*N;
		}
		counts[size-1] = counts[size-1]-(K2-1)*N; //last prosess is stuck with all the overlapp
		//i guess this could have been distributed, but as the kernels are much smaller than N and M
		// i guess it wont be a large issue. 

		

		for (int i = 1; i < size; ++i)
		{
			displacements[i] = displacements[i-1]+counts[i-1];
		}

		for (int i = 0; i < size; ++i)
		{
			printf("count - %d -", counts[i]);
			
		}printf("\n");
		printf("m:%d rank:%d\n", my_M,my_rank);
		MPI_Gatherv(&my_output[0][0],my_M*N,MPI_FLOAT,&output[0][0],counts,displacements,MPI_FLOAT,0,MPI_COMM_WORLD);

	}
	
	else{
		printf("m:%d rank:%d\n", my_M,my_rank);
		MPI_Gatherv(&my_output[0][0],my_M*N,MPI_FLOAT,NULL,NULL,NULL,MPI_FLOAT,0,MPI_COMM_WORLD);
	}
	
	
	
	deallocate(&my_input);
	deallocate(&my_output);
}


void single_layer_convolution (int M, int N, float **input, int K, float **kernel,float **output){
	int i,j,ii,jj;
	double temp;
	for (i=0; i<=M-K; i++)
		for (j=0; j<=N-K; j++) {
			temp = 0.0;

			for (ii=0; ii<K; ii++)
				for (jj=0; jj<K; jj++)
					temp += input[i+ii][j+jj]*kernel[ii][jj];
			output[i][j] = temp;
		}

	}
