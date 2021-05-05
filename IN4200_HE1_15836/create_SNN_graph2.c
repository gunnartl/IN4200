#include <stdio.h>
#include <stdlib.h>

void create_SNN_graph2 (int N, int *row_ptr, int *col_idx, int **SNN_val){
	
	int len = row_ptr[N];

	*SNN_val = (int*)calloc(len,sizeof(int));


	clock_t start,stop;


	for (int i = 1; i < N+1; ++i)
	{
		for (int j = row_ptr[i-1]; j < row_ptr[i]; ++j) // makes indexes for iterating a row of col_idx
		{
			for (int k = row_ptr[col_idx[j]]; k < row_ptr[col_idx[j]+1]; ++k)
			{
			    for (int l = row_ptr[i-1]; l < row_ptr[i]; ++l){
			    	if (col_idx[l] == col_idx[k]) (*SNN_val)[j]+=1;
			    }
			}
		}
	}

}