#include "stdio.h"
#include "stdlib.h"
#include "mpi.h"

int main(int argc, char** argv) {
	int rank, size, err;
	MPI_File primary_file;
	char *primary_filename, *primary_data;
	MPI_Offset primary_offset, primary_size;
	MPI_Status status;

	if(argc != 2) {
		fprintf(stderr, "usage: %s <primary filename>\n", argv[0]);
		exit(1);
	}

	/* Init */
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	primary_filename = argv[1];

	/* Open the file */
	err = MPI_File_open(MPI_COMM_WORLD, primary_filename, MPI_MODE_RDONLY, MPI_INFO_NULL, &primary_file);
	if (err != MPI_SUCCESS) {
		MPI_Abort(MPI_COMM_WORLD, err);
	}
	MPI_File_get_size(primary_file, &primary_size);

	/* split the file by byte */
	primary_offset = rank * primary_size / size;
	if (rank == size - 1) {
		/* last task will pick up the slack */
		primary_size = primary_size / size + primary_size % size;
	} else { 
		primary_size = primary_size / size;
	}
	primary_data = (char*) malloc(primary_size);
	MPI_File_read_at_all(primary_file, primary_offset, primary_data, primary_size, MPI_CHAR, &status);

	/* user function */
	int i=0, j=0;
	char *partial_record;
	int record_start, record_size;

	partial_record = (char*) malloc(1000000);

/*	while(i < primary_size) {
		while(primary_data[i] != '\n' && i < primary_size) {
			i++;
		}
		i++;
	}*/

	char first_id[20], last_id[20];

	/* find to first full record */
	i = 0;
	if (rank != 0) {
		while(primary_data[i] != '\n') {
			i++;
		}
		i+=2;
	} else {
		i++;
	}

	while(primary_data[i] != '"') {
		first_id[j]=primary_data[i];
		j++;
		i++;
	}
	first_id[j] = '\0';

	/* find last full record */
	i = primary_size-1;
	while(primary_data[i] != '\n') {
		i--;
	}
	i--;
	while(primary_data[i] != '\n') {
		i--;
	}
	i+=2;

	j=0;
	while(primary_data[i] != '"') {
		last_id[j]=primary_data[i];
		j++;
		i++;
	}
	last_id[j] = '\0';

	printf("%d: %s - %s\n", rank, first_id, last_id);


	/* Finalize */
	MPI_File_close(&primary_file);
	MPI_Finalize();
	return 0;
}
