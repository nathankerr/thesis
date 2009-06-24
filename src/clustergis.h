#ifndef CLUSTERGIS_H
#define CLUSTERGIS_H

#define CLUSTERGIS_BUFFERSIZE 1024*1024

#include "stdio.h"
#include "stdlib.h"
#include "mpi.h"

/* clusterGIS variables */
int clusterGIS_rank, clusterGIS_tasks;
int clusterGIS_started;

/* clusterGIS datatypes */
struct clusterGIS_record_el {
	char** data;
	int columns;
	struct clusterGIS_record_el * next;
};
typedef struct clusterGIS_record_el clusterGIS_record;
struct clusterGIS_dataset {
	clusterGIS_record* data;
};
typedef struct clusterGIS_dataset clusterGIS_dataset;

/* clusterGIS functions */
int clusterGIS_Init(int*, char***);
int clusterGIS_Finalize();
void clusterGIS_Load_data_distributed(char*, clusterGIS_dataset**);
void clusterGIS_Load_data_replicated(char*, clusterGIS_dataset**);
int clusterGIS_create_record(char* data, int start, clusterGIS_record**);
void clusterGIS_Free_dataset(clusterGIS_dataset**);
int clusterGIS_Write_record(MPI_File* file, clusterGIS_record* record);

#endif
