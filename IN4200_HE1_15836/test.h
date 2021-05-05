#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <omp.h>
#include "read_graph_from_file1.c"
#include "read_graph_from_file2.c"
#include "create_SNN_graph1.c"
#include "create_SNN_graph1_parallel.c"
#include "create_SNN_graph2.c"
#include "create_SNN_graph2_parallel.c"
#include "check_node.c"



void read_graph_from_file1 (char *filename, int *N, char ***table2D);
void read_graph_from_file2 (char *filename, int *N, int **row_ptr, int **col_idx);

void create_SNN_graph1 (int N, char **table2D, int ***SNN_table);
void create_SNN_graph1_parallel (int N, char **table2D, int ***SNN_table,int threads);

void create_SNN_graph2 (int N, int *row_ptr, int *col_idx, int **SNN_val);
void create_SNN_graph2_prallel (int N, int *row_ptr, int *col_idx, int **SNN_val, int threads);

void check_node (int node_id, int tau, int N, int *row_ptr, int *col_idx, int *SNN_val);