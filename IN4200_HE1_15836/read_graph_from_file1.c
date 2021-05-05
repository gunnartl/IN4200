#include <stdio.h>
#include <stdlib.h>

void read_graph_from_file1 (char *filename, int *N, char ***table2D){
	/*
	Makes a boolean NxN 2d table showing the connectivity of the N nodes
	1 at table2D[m][n] shows a connection between node m and n
	-symmetric connections
	-exludes self-linkage
	*/
	int n_edge;



	FILE *infile = fopen(filename,"r");

	//skip first two lines
	fscanf(infile, "%*[^\n]\n");
	fscanf(infile, "%*[^\n]\n");
	//read third
	fscanf(infile, "%*s %*s %d %*s %d\n",N, &n_edge);
	//skip fourth
	fscanf(infile, "%*[^\n]\n");


	//Allocate 2d-table of zeroes
	*table2D    = (char**)malloc(*N *sizeof(char*));
	(*table2D)[0] = (char*)calloc(*N * *N,sizeof(char)); //calloc for zeros

	for (int i = 0; i < *N; i++) (*table2D)[i] = &(*table2D)[0][*N*i];

	// populate with legal values
	int x, y;
	for (int i = 0; i < n_edge; ++i)
	{
		fscanf(infile,"%d %d",&x,&y);
		if(0<=y && 0<=x && x<= *N && y<=*N) {  // if-test to keep x and y between 0 and N. specifically asked on padled if this was neccecary
			(*table2D)[x][y] = 1;
			(*table2D)[y][x] = 1;
		}
	}

	
	// sets potential self-lingage to zero faster than more if-tests in population-loop:
	for (int i = 0; i < *N; ++i) (*table2D)[i][i] = 0;



	fclose(infile);

}
