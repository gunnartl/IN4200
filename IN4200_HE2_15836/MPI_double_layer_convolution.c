#include <stdio.h>
#include <stdlib.h>

void MPI_double_layer_convolution (int M, int N, float **input, int K1, float **kernel1, int K2, float **kernel2,float **output){
	/*
	function to perform a parallelized double convolution on a MxN matrix input with two kernels of size K1 and K2
	*/
	int my_rank,size;
	MPI_Comm_rank (MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size (MPI_COMM_WORLD, &size);
	int my_M;
	int overlap_size1 = K1-1;    // number of overlapping rows
	int overlap_size2 = K2-1;
	float **my_input,**my_output;
    
    //sets the nummber of rows for all procs. integerdivision of rows, splits remainder of k between k first processes. and adds overlap
    // using the size of M after one convolution to ensure rather equal distribution of workload
    my_M = (my_rank<(M-K1+1)%size)? (M-K1+1)/size + overlap_size1+1 :(M-K1+1)/size + overlap_size1;

    allocate2D(my_M,N,&my_input);
    allocate2D(my_M-overlap_size1,N-K1+1,&my_output);
	//first convolution
	if(my_rank == 0){

	int n_each = (M-K1+1)/size; // lines each
	int n_more = (M-K1+1)%size; //number of procs getting one more line

	int *counts = (int*)calloc(size,sizeof(int));
	int *displacements = (int*)calloc(size,sizeof(int));

	//calculates nummber of floats for each process 
	for (int i = 0; i < size; ++i)
	{
		counts[i] = (i<n_more)? (n_each + overlap_size1+1)*N:(n_each + overlap_size1)*N; //could gather my_M but this is faster
	}
	
	//sets displacements witch is lenght of counts[i-1] - overlappsize
	for (int i = 1; i < size; ++i)
	{
		displacements[i] = displacements[i-1]+counts[i-1]-overlap_size1*N;
	}

	//scatters input to procs with sizes and sisplacements already calculated 
    MPI_Scatterv(&input[0][0],counts,displacements,MPI_FLOAT,&my_input[0][0],my_M*N,MPI_FLOAT,0,MPI_COMM_WORLD);

	}

	if (my_rank>0)
	{	
		//recieving call from all other procs
		MPI_Scatterv(NULL,NULL,NULL,NULL,&my_input[0][0],my_M*N,MPI_FLOAT,0,MPI_COMM_WORLD);
	}

	// all procs
	//Im using the function given in the exam-text. The alternative is just to copypaste it, and this leaves a much cleaner code:
	single_layer_convolution(my_M,N,my_input,K1,kernel1,my_output);

	//allocates room for the second input witch is the size of the last input - K1+1, pluss an overlapp for all other processes than the last
	my_M = (my_rank==size-1)?my_M - K1+1 : my_M - K1+1+overlap_size2;
	N = N - K1+1;
	deallocate2D(&my_input);
	allocate2D(my_M,N,&my_input);
	

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
	deallocate2D(&my_output);
	allocate2D(my_M-overlap_size2, N-overlap_size2,&my_output);


	// sending and recieving overlaps, only sending upwards leaving process size-1 a bit less work
	// but seeing as N and M >> K1 and K2 this is not a big problem

	if(my_rank == 0){
		// proc 0 only recieving
		MPI_Recv(&my_input[my_M-overlap_size2][0], N*overlap_size2,MPI_FLOAT, 1, my_rank+1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
	}
	else if (my_rank == size-1){
		//last proc only sending
		MPI_Send(&my_input[0][0], N*overlap_size2,MPI_FLOAT,my_rank-1,my_rank,MPI_COMM_WORLD);
	}

	else {
		// all others sending and recieving
	    MPI_Sendrecv(&my_input[0][0], N*overlap_size2,MPI_FLOAT,my_rank-1,my_rank,&my_input[my_M-overlap_size2][0],N*overlap_size2,MPI_FLOAT,my_rank+1,my_rank+1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    }


    //second convolution
	single_layer_convolution(my_M,N,my_input,K2,kernel2,my_output); 


	//update M and N for gathering
	my_M = my_M-K2+1;
	N = N-K2+1;
	if (my_rank==0){

		//again calculating amount of lines for each process.

		//for some reason n_each and other variables from last calulation is not available anymore
		//and the compiler says they are undeclared?? if you have time for some feedback pleace tell my why. 

		int n_each = (M-K1+1)/size; // lines each

		int n_more = (M-K1+1)%size; //number of procs getting one more line

		int *counts = (int*)calloc(size,sizeof(int));
		int *displacements = (int*)calloc(size,sizeof(int));
		
		for (int i = 0; i < size; ++i)
		{
			counts[i] = (i<n_more)? (n_each+1)*N:n_each*N;
		}
		counts[size-1] = counts[size-1]-(K2-1)*N; //last prosess is stuck with all the missing rows from second convolution
		//i guess this could have been distributed, but as the kernels are much smaller than N and M
		// i guess it wont be a large issue. 

		

		for (int i = 1; i < size; ++i)
		{
			displacements[i] = displacements[i-1]+counts[i-1];
		}

		MPI_Gatherv(&my_output[0][0],my_M*N,MPI_FLOAT,&output[0][0],counts,displacements,MPI_FLOAT,0,MPI_COMM_WORLD);

	}
	
	else{
		MPI_Gatherv(&my_output[0][0],my_M*N,MPI_FLOAT,NULL,NULL,NULL,MPI_FLOAT,0,MPI_COMM_WORLD);
	}
	
	
	
	deallocate2D(&my_input);
	deallocate2D(&my_output);
}