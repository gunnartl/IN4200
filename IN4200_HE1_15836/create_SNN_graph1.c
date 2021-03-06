#include <stdio.h>
#include <stdlib.h>

void create_SNN_graph1 (int N, char **table2D, int ***SNN_table){
	

	//allocate SNN-table
	*SNN_table    = (int**)malloc(N *sizeof(int*));
	(*SNN_table)[0] = (int*)calloc(N * N,sizeof(int)); //calloc for zeros

	for (int i = 0; i < N; i++) (*SNN_table)[i] = &(*SNN_table)[0][N*i];



	for (int i = 0; i < N; ++i)
	{
		for (int j = i; j < N; ++j)
		{
			if(table2D[i][j] != 0){

				for (int k = 0; k < N; ++k) {
					(*SNN_table)[i][j]+= table2D[i][k]*table2D[j][k];
					(*SNN_table)[j][i] = (*SNN_table)[i][j];
				}
			}
		}
	}

}