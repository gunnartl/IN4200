#include "test.h"


int main(int argc, char const *argv[])
{


    if( argc != 4 ) {
    	printf("Wrong amount of arguments \ninput format: N_threads node_to_check tau \n");
    	exit(1);
    }
	int N;
	char **table;
	char *filename = "test_test.txt";

	int threads = atoi(argv[1]);
	int node_to_check = atoi(argv[2]);
	int tau = atoi(argv[3]);

	double start,stop;
	start= omp_get_wtime();
	read_graph_from_file1(filename,&N,&table);
    stop = omp_get_wtime()-start;
	printf("read time table:  %f s\n", stop);

	printf("----------------------------------------------------\n");
	//serial 2d snn
	int **snn2d;


	start= omp_get_wtime();
	create_SNN_graph1(N,table,&snn2d);
    stop = omp_get_wtime()-start;
	printf("2d-SNN-serial time:  %f s\n", stop);
	double serial_stop = stop;
	free(snn2d[0]);
	free(snn2d);
    printf("----------------------------------------------------\n");
    //parallel 2d snn


	start= omp_get_wtime();
	create_SNN_graph1_parallel(N,table,&snn2d,threads);
    stop = omp_get_wtime()-start;
	printf("2d-SNN-parallel time:  %f s\n", stop);
	printf("Speedup : %f \n", serial_stop/stop);
	free(snn2d[0]);
	free(snn2d);



	free(table[0]);
	free(table);


	printf("----------------------------------------------------\n");

	int *row_ptr, *col_idx;
	read_graph_from_file2(filename,&N,&row_ptr,&col_idx);

	int *SNN_val;
	start = omp_get_wtime();
	create_SNN_graph2(N,row_ptr,col_idx,&SNN_val);
    stop = omp_get_wtime()-start;
	printf("crs-SNN-serial time:  %f s\n", stop);
	serial_stop = stop;
	printf("----------------------------------------------------\n");


	free(SNN_val);
    start = omp_get_wtime();
	create_SNN_graph2_prallel(N,row_ptr,col_idx,&SNN_val,threads);
    stop = omp_get_wtime()-start;
	printf("crs-SNN-paralell time:  %f \n", stop);
	printf("Speedup : %f \n", serial_stop/stop);
    printf("----------------------------------------------------\n");

	start = omp_get_wtime();
	check_node(node_to_check,tau,N,row_ptr,col_idx,SNN_val);
	stop = omp_get_wtime()-start;
	printf("cluster check time :  %f s\n", stop);

	free(row_ptr);
	free(col_idx);
	free(SNN_val);

	return 0;
}
