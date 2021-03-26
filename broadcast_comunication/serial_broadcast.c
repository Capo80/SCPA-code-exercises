#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <string.h>

void main(int argc, char** argv) {

	int my_rank;
	int size;
	int tag = 42;
	char message[100];
	MPI_Status status;
	double start_time, end_time;

	// Init MPI
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (my_rank == 0) {

		// Read message
		printf("Insert message to send: ");
		fflush(stdout);
		scanf("%s", message);
		printf("\n");

		MPI_Barrier(MPI_COMM_WORLD);
		start_time = MPI_Wtime();

		// Send to all nodes
		for (int i = 1; i < size; i++) {
			MPI_Send(message, strlen(message)+1, MPI_CHAR, i, tag, MPI_COMM_WORLD);	
		}

	} else {

		MPI_Barrier(MPI_COMM_WORLD);
		start_time = MPI_Wtime();

		//Recv Message
		MPI_Recv(message, 100, MPI_CHAR, 0, tag, MPI_COMM_WORLD, &status);
		//print("Process %d has received the message \"%s\"", my_rank, message);

	}

	end_time = MPI_Wtime();
	printf("Process %d has has finished in %f\n", my_rank, end_time-start_time);

	MPI_Finalize();
}