#include "pgis.h"

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void status(char* message) {
	fprintf(stdout, "[%d] %s\n", rank, message);
}

void error_abort(char* message, char* filename, int linenum) {
	fprintf(stderr, "[%d] ERROR: \"%s\" (%s:%d)\n", rank, message, filename, linenum);
	MPI_Abort(MPI_COMM_WORLD, 1);
}

void init(int argc, char* argv[], char* pgdata) {
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	sprintf(pgdata, "/tmp/pgis-%d-%d", getpid(), rank);
	postgis_start(pgdata);
}

void finalize() {
	postgis_stop();
	MPI_Finalize();
}

int main(int argc, char* argv[]) {
	char pgdata[256];

	init(argc, argv, pgdata);


	/* Decompose and Load Data */
	/* Perform Calculation */
	/* Save data */

	finalize();
	return 0;
}
