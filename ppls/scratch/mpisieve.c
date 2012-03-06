#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
	MPI_Comm nextComm;
	int candidate = 2;
	int N = atoi(argv[1]);

	MPI_Init(&argc, &argv);

	MPI_Comm_spawn("sieve", argv, 1, MPI_INFO_NULL, 0,
			MPI_COMM_WORLD, &nextComm, MPI_ERRCODES_IGNORE);
	
	while(candidate < N) {
		MPI_Send(&candidate, 1, MPI_INT, 0, 0, nextComm);
		candidate++;
	}

	candidate = -1;
	MPI_Send(&candidate, 1, MPI_INT, 0, 0, nextComm);

	MPI_Finalize();
}
