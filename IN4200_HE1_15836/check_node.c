#include <stdio.h>
#include <stdlib.h>


void check_node (int node_id, int tau, int N, int *row_ptr, int *col_idx, int *SNN_val){
	int *cluster = (int*)calloc(N,sizeof(int));
	for (int i = 0; i < N; ++i)
	{
		cluster[i] = -1;
	}
	int counter=0	;
	for (int i = row_ptr[node_id]; i < row_ptr[node_id+1]; ++i)
	{
		
		if (SNN_val[i]>=tau)
		{
			cluster[counter] = col_idx[i];
			counter++;

		}

		//denne gir meg alle shared nearest neighbour verdiene til alle denne er kobla til. 
		//if-test for å sjekke hvilke som er større enn= tau
		//col index for å finne hvilke noder det er. 
		// så må de nye nodene søkes gjennom
	}
	printf("%d %d %d %d \n", cluster[0],cluster[1],cluster[2],cluster[3]);

	if(cluster[0]!=-1){
		int old_count = 0;
		int node = 0;
		int new_nodes;
		while(old_count != counter){
			new_nodes = counter-old_count;
			old_count = counter;
			int placeholder = 0;
			for (int i = 0; i < new_nodes; ++i)
			{
				for (int j = row_ptr[cluster[placeholder+i]]; j < row_ptr[cluster[placeholder+i]+1]; ++j)
				{

					for (int k = 0; k < counter; ++k)
					{
						if(SNN_val[j]>=tau){
							if(col_idx[j] == cluster[k] || col_idx[j] == node_id)
							{
								break;
							}
							if (k == counter-1)
							{
								cluster[counter]= col_idx[j];
								counter++;
							}
					    }
					}
				}
			}
		}

		printf("There are %d other nodes in the cluster around node %d with tau = %d: ",counter, node_id , tau);
		for (int i = 0; i < N; ++i)
		{	if (cluster[i]>=0) printf("%d ", cluster[i]);
			else break;
		}
		printf("\n");
			
	}
	else{
		printf("Node %d does not form a base for a cluster with tau %d \n", node_id, tau);
	}

}