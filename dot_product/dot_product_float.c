#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <string.h>

#define N 5
#define SEED 1234
#define MAX_NUMBER 10000


void print_float_array(float* array, int len) {
	printf("[");
	for (int i = 0; i < len-1; i++) {
		printf("%f, ", array[i]);
	}
	printf("%f]\n", array[len-1]);
}

void print_int_array(int* array, int len) {
	printf("[");
	for (int i = 0; i < len-1; i++) {
		printf("%d, ", array[i]);
	}
	printf("%d]\n", array[len-1]);
}


void main(int argc, char** argv) {

	int my_rank;
	int size;
	int tag = 42;
	float vector1[N];
	float vector2[N];
	float final_res;

	// Init MPI
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (my_rank == 0) {

		//fill vectors with random numbers
		srand(SEED);
		for (int i = 0; i < N; i++) {
			vector1[i] = rand()%MAX_NUMBER*1.0/(MAX_NUMBER/1000);
			vector2[i] = rand()%MAX_NUMBER*1.0/(MAX_NUMBER/1000);
		}
		printf("Vector1: ");
		print_float_array(vector1, N);
		printf("Vector2: ");
		print_float_array(vector2, N);

	}

	//create send count and displs
	int div = N/size;
	int r = N%size;
	int* sendcounts = (int*) malloc(size*sizeof(int));
	int* displs = (int*) malloc(size*sizeof(int));
	int curr = 0;

	for (int i = 0; i < r; i++) {
		sendcounts[i] = div+1;
		displs[i] = curr;
		curr += sendcounts[i];
	}
	sendcounts[r] = div;	
	displs[r] = curr;
	curr += sendcounts[r];
	for (int i = r+1; i < size; i++) {
		sendcounts[i] = div;	
		displs[i] = curr;
		curr += sendcounts[i];
	}
	if (my_rank == 0) {
		printf("Sendcounts: ");
		print_int_array(sendcounts, size);
		printf("Displs: ");
		print_int_array(displs, size);
	}

	//Set up recv buffers
	float* vector1_part = (float*) malloc(sendcounts[my_rank]*sizeof(float));
	float* vector2_part = (float*) malloc(sendcounts[my_rank]*sizeof(float));

	//Scatter to all
	MPI_Scatterv(vector1, sendcounts, displs, MPI_FLOAT, vector1_part, sendcounts[my_rank], MPI_INT, 0, MPI_COMM_WORLD);
	//MPI_Barrier(MPI_COMM_WORLD);
	MPI_Scatterv(vector2, sendcounts, displs, MPI_FLOAT, vector2_part, sendcounts[my_rank], MPI_INT, 0, MPI_COMM_WORLD);
	//MPI_Barrier(MPI_COMM_WORLD);
	
	printf("Process %d has Vector1: ", my_rank);
	print_float_array(vector1_part, sendcounts[my_rank]);
	printf("Process %d has Vector2: ", my_rank);
	print_float_array(vector2_part, sendcounts[my_rank]);
	fflush(stdout);

	//Calculate local
	float res = 0;
	for (int i = 0; i < sendcounts[my_rank]; i++){
		res += vector1_part[i]*vector2_part[i];
	}

	printf("io %f\n", res);
	//Reduce to calculate final product
	MPI_Reduce(&res, &final_res, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

	if (my_rank == 0)
		printf("Dot product is %f\n", final_res);

	free(vector1_part);
	free(vector2_part);
	free(sendcounts);
	free(displs);
	MPI_Finalize();
}