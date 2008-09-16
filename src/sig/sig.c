#include <mpi.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

int rank, size;

void sighandler(int signum);

int main(int argc, char* argv[]) {
	MPI_Init(&argc, &argv);

	if (signal(SIGTERM, &sighandler) == SIG_ERR) {
		printf("couldn't register signal handler.\n");
	}

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (rank == 0) {
		sleep(5);
		MPI_Abort(MPI_COMM_WORLD, 1);
	} else {
		while(1);
	}

	MPI_Finalize();
	return 0;
}

void sighandler(int signum) {
	printf("Caught SIGTERM with %d\n", signum);
	exit(signum);
}
