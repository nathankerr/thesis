#include "clustergis.h"

int main(int argc, char** argv) {
	clusterGIS* head;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s filename\n", argv[0]);
		exit(1);
	}

	clusterGIS_Init(&argc, &argv);

	clusterGIS_Load_data_distributed(argv[1], head);

	clusterGIS_Finalize();
	return 0;
}
