#include <stdio.h>
#include <stdlib.h>

void read_graph_from_file2 (char *filename, int *N, int **row_ptr, int **col_idx){

	int n_edge;



	FILE *infile = fopen(filename,"r");

	//skip first two lines
	fscanf(infile, "%*[^\n]\n");
	fscanf(infile, "%*[^\n]\n");
	//read third
	fscanf(infile, "%*s %*s %d %*s %d\n",N, &n_edge);
	//skip fourth
	fscanf(infile, "%*[^\n]\n");


	//llocate row_ptr and col_idx
	*row_ptr = (int*)calloc(*N+1,sizeof(int));
	*col_idx = (int*)calloc(n_edge*2,sizeof(int));

	int *from = (int*)malloc(n_edge*sizeof(int));
	int *to = (int*)malloc(n_edge*sizeof(int));

	int x, y;
	for (int i = 0; i < n_edge; ++i)
	{
		fscanf(infile,"%d %d",&x,&y);
		if(0<=y && 0<=x && x<= *N && y<=*N && x!=y) {  // if-test to keep x and y between 0 and N nd not equal
			(*row_ptr)[x+1]++;
			(*row_ptr)[y+1]++;
			from[i] = x;
			to[i] = y;
		}
	}
	fclose(infile);

	//cumsum

	for (int i = 1; i < *N+1; ++i) (*row_ptr)[i] += (*row_ptr)[i-1];
	

	

	int counter = 0; // counnter for col_idx

	for (int i = 0; i <= *N; ++i)
	{	

		for (int j = 0; j < n_edge; ++j)
		{
			if (from[j] == i ){
				(*col_idx)[counter] = to[j];
				counter++;
			}
			if (to[j] == i ){
				(*col_idx)[counter] = from[j];
				counter++;
			}
		}
	}

	// Could sort for each row here

	
	free(from);
	free(to);

}