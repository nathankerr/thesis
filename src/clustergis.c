#include "clustergis.h"
#include "string.h"

int clusterGIS_Init(int* argc, char*** argv) {
	/* Init */
	int ret;
	ret = MPI_Init(argc, argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &clusterGIS_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &clusterGIS_tasks);

	clusterGIS_started = 1;
	return ret;
}

int clusterGIS_Finalize() {
	int ret = MPI_Finalize();
	/* Free all malloc'ed memory */
	return ret;
}

void clusterGIS_Load_data_distributed(char* filename, clusterGIS_dataset** dataset) {
	MPI_File file;
	int err;
	int i;
	int j;
	MPI_Offset chunksize, offset;
	char* rawdata;
	char* record_buffer;
	int record_buffer_size = 1024*1024;
	MPI_Status status;
	int first_full_record_start, last_full_record_end;
	clusterGIS_record** record;

	/* Load the file, distributed between tasks bytewise */
	err = MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_RDONLY, MPI_INFO_NULL, &file);
	if(err != MPI_SUCCESS) {
		fprintf(stderr, "Error opening file %s\n", filename);
		MPI_Abort(MPI_COMM_WORLD, err);
	}
	MPI_File_get_size(file, &chunksize);
	offset = clusterGIS_rank * chunksize / clusterGIS_tasks;
	if (clusterGIS_rank == clusterGIS_tasks - 1) {
		/* last task will pick up the slack */
		chunksize = chunksize / clusterGIS_tasks + chunksize % clusterGIS_tasks;
	} else { 
		chunksize = chunksize / clusterGIS_tasks;
	}
	rawdata = (char*) malloc(chunksize);
	MPI_File_read_at_all(file, offset, rawdata, chunksize, MPI_CHAR, &status);
	MPI_File_close(&file);
	
	/* find where the first full record begins */
	first_full_record_start = 0;
	if (clusterGIS_rank != 0) {
		while(rawdata[first_full_record_start++] != '\n');
	}

	/* find where the last full record ends */
	last_full_record_end = chunksize-1;
	while(rawdata[last_full_record_end--] != '\n');
	last_full_record_end++;
	
	/* Put the records into the dataset */
	i = first_full_record_start;
	(*dataset) = (clusterGIS_dataset*) malloc(sizeof(clusterGIS_dataset));
	record = &(*dataset)->data;
	while (i < last_full_record_end) {
		i = clusterGIS_create_record(rawdata, i, record);
		(*record)->next = NULL;
		record = &(*record)->next;
		i++;
	}

	/* Construct full record from the partials */
	/* Shifts the begining of rawdata (end of a partial record) to the left, appending it to the partial record at the end of rawdata */
	record_buffer = (char*) malloc(record_buffer_size);
	i = last_full_record_end+1;
	j = 0;
	while(i < chunksize) {
		record_buffer[j] = rawdata[i];
		i++;
		j++;
	}
	if (clusterGIS_rank == 0) {
		/* leftmost, does not need to send */
		MPI_Recv(&(record_buffer[j]), record_buffer_size-j, MPI_CHAR, clusterGIS_rank + 1, 99, MPI_COMM_WORLD, &status);
	} else if (clusterGIS_rank == clusterGIS_tasks - 1) {
		/* rightmost, does not need to receive */
		MPI_Send(rawdata, first_full_record_start-1, MPI_CHAR, clusterGIS_rank - 1, 99, MPI_COMM_WORLD);
	} else {
		/* inner, needs to send and receive */
		MPI_Sendrecv(rawdata, first_full_record_start-1, MPI_CHAR, clusterGIS_rank - 1, 99, &(record_buffer[j]), record_buffer_size-j, MPI_CHAR, clusterGIS_rank + 1, 99, MPI_COMM_WORLD, &status);
	}

	/* If there is a recomposed record, add it to the dataset */
	if(clusterGIS_rank != clusterGIS_tasks - 1) {
		MPI_Get_count(&status, MPI_CHAR, &i);
		record_buffer[j+i] = '\n';
	
		clusterGIS_create_record(record_buffer, 0, record);
		(*record)->next = NULL;
	}

	free(record_buffer);
	free(rawdata);
}

void clusterGIS_Load_data_replicated(char* filename, clusterGIS_dataset** dataset) {
	MPI_File file;
	int err;
	int i;
	MPI_Offset filesize;
	char* rawdata;
	MPI_Status status;
	clusterGIS_record** record;

	/* Load the file, with a full copy on each task */
	err = MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_RDONLY, MPI_INFO_NULL, &file);
	if(err != MPI_SUCCESS) {
		fprintf(stderr, "Error opening file %s\n", filename);
		MPI_Abort(MPI_COMM_WORLD, err);
	}
	MPI_File_get_size(file, &filesize);
	rawdata = (char*) malloc(filesize);
	MPI_File_read_all(file, rawdata, filesize, MPI_CHAR, &status);
	MPI_File_close(&file);
	
	/* Put the records into the dataset */
	i = 0;
	(*dataset) = (clusterGIS_dataset*) malloc(sizeof(clusterGIS_dataset));
	record = &(*dataset)->data;
	while (i < filesize) {
		i = clusterGIS_create_record(rawdata, i, record);
		(*record)->next = NULL;
		record = &(*record)->next;
		i++;
	}

	free(rawdata);
}

/* Returns index where record ends (\n)*/
int clusterGIS_create_record(char* data, int start, clusterGIS_record** record) {
	int end = start;
	int field_end;
	int field_count;
	int field_start;
	int i;
	int j;
	clusterGIS_record* temp_record;

	struct item {
		char* data;
		int len;
		struct item* next;
	};
	struct item* head;
	struct item* current;

	/* find end of record */
	while(data[end] != '\n') {
		end++;
	}

	/* Pull the fields out of the record. Assumes fields are comma delimited. Quotes surround fields with commas or quotes (escaped in the field) */
	i = start;
	field_count = 0;
	head = (struct item*) malloc(sizeof(struct item));
	current = head;
	current->next = NULL;
	while(i < end) {
		/* get the start and stop indexes of the field */
		if(data[i] == '"') {
			/* escaped field */
			i++;
			field_start = i;
			while (data[i] != '"' && data[i-1] != '\\') {
				i++;
			}
			field_end = i;
			i++; /* moves to the , or \n that follows the " */
		} else {
			/* non escaped field */
			field_start = i;
			while(data[i] != ',' && data[i] != '\n') {
				i++;
			}
			field_end = i;
		}

		/* add field to the linked list and move to the next field */
		current->data = (char*) malloc(field_end - field_start + 1);
		for(j = 0; j < field_end - field_start; j++) {
			current->data[j] = data[field_start + j];
		}
		current->data[j] = '\0';

		current->next = (struct item*) malloc(sizeof(struct item));
		current = current->next;
		current->next = NULL;
		field_count++;
		i++;
	}

	/* Convert the linked list to an array of strings */
	current = head;
	i = 0;
	temp_record = (clusterGIS_record*) malloc(sizeof(clusterGIS_record));
	temp_record->data = malloc(field_count * sizeof(char*));
	temp_record->columns = field_count;
	for(i = 0; i < field_count; i++) {
		temp_record->data[i] = current->data;
		head = current->next;
		free(current);
		current = head;
	}

	*record = temp_record;
	
	return end;
}
