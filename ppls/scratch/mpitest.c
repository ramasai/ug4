#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
	int rank, p;

	MPI_Init(&argc, &argv);

	// Explore the world.
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	// Say hello
	printf("Hello world from process %d of %d\n", rank, p);

	MPI_Finalize();
}
