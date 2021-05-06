#include <stdio.h>
#include <stdlib.h>

void deallocate2D(float ***table){
	free(*table[0]);
	free(*table);
}