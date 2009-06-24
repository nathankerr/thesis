#include "clustergis.h"

int main(int argc, char** argv) {
	clusterGIS_dataset* dataset;
	clusterGIS_record* record;
	int count;
	int total_count;
	MPI_File output;

	/* Process local arguments */
	if (argc != 3) {
		fprintf(stderr, "Usage: %s input output\n", argv[0]);
		exit(1);
	}

	clusterGIS_Init(&argc, &argv);

	//clusterGIS_Load_data_replicated(argv[1], &dataset);
	clusterGIS_Load_data_distributed(argv[1], &dataset);

	MPI_File_open(MPI_COMM_WORLD, argv[2], MPI_MODE_WRONLY|MPI_MODE_CREATE, MPI_INFO_NULL, &output);
	printf("%d: file opened\n", clusterGIS_rank);
	clusterGIS_Write_record(&output, dataset->data);

	record = dataset->data;
	while(record != NULL) {
		count++;
		record = record->next;
	}


	MPI_Reduce(&count, &total_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	if(clusterGIS_rank == 0) printf("Count: %d\n", total_count);
	//printf("%d: Count: %d\n", clusterGIS_rank, count);
	
	/* Finalize */
	MPI_File_close(&output);
	clusterGIS_Finalize();
	return 0;
}
