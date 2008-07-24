#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libpq-fe.h>

/* Sets up the local db and starts the PostgreSQL server */
void start_postgres(char* pgdata) {
	char* cmd = malloc(sizeof(char)*256);

	/* Setup the database */
	sprintf(cmd, "initdb -D %s", pgdata);
	system(cmd);

	/* Start running the database without providing IP connection, and socket in the data directory */
	sprintf(cmd, "pg_ctl -D %s -o \"-h '' --unix_socket_directory=%s\" start", pgdata, pgdata);
	system(cmd);

	free(cmd);
}

/* Stops the PostgreSQL server and deletes the local db */
void stop_postgres(char* pgdata) {
	char* cmd = malloc(sizeof(char)*256);

	/* Stop running the database */
	sprintf(cmd, "pg_ctl -D %s stop", pgdata);
	system(cmd);

	/* Destroy the database */
	sprintf(cmd, "rm -rf %s", pgdata);
	system(cmd);

	free(cmd);
}

int main(int argc, char* argv[]) {
	int rank; /* MPI rank */
	int size; /* MPI size */
	char* pgdata; /* Directory for local PostgreSQL data */

	/* Init MPI and data structures */
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	pgdata = malloc(sizeof(char)*256);

	sprintf(pgdata, "/tmp/pgis-%d-%d", getpid(), rank);
	start_postgres(pgdata);

	sleep(10);

	/* Finalize MPI and clean up */
	stop_postgres(pgdata);
	free(pgdata);
	MPI_Finalize();
	return 0;
}
