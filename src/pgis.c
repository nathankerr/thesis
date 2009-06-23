#include "clustergis.h"

int main(int argc, char** argv) {
	clusterGIS_dataset* dataset;
	clusterGIS_record* record;
	int count;

	/* Process local arguments */
	if (argc != 2) {
		fprintf(stderr, "Usage: %s filename\n", argv[0]);
		exit(1);
	}

	clusterGIS_Init(&argc, &argv);

	clusterGIS_Load_data_replicated(argv[1], &dataset);
	//clusterGIS_Load_data_distributed(argv[1], &dataset);

	/* User operations */
	record = dataset->data;
	count = 0;

	while(record != NULL) {
		count++;
		record = record->next;
	}


	//MPI_Reduce(&count, &total_count, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	//if(clusterGIS_rank == 0) printf("Count: %d\n", total_count);
	printf("%d: Count: %d\n", clusterGIS_rank, count);
	
	/* Finalize */
	clusterGIS_Finalize();
	return 0;
}
