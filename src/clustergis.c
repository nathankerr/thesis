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
	char* buffer;
	int buffersize = 2*1024*1024;
	MPI_Status status;
	clusterGIS_record** record;
	MPI_Offset offset;
	int count;
	int last_full_record_end;
	MPI_Offset filesize;
	int i;

	err = MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_RDONLY, MPI_INFO_NULL, &file);
	if(err != MPI_SUCCESS) {
		fprintf(stderr, "%d: Error opening file %s\n", clusterGIS_rank, filename);
		MPI_Abort(MPI_COMM_WORLD, err);
	}

	buffer = (char*) malloc(buffersize);
	MPI_File_get_size(file, &filesize);
	offset = 0;
	(*dataset) = (clusterGIS_dataset*) malloc(sizeof(clusterGIS_dataset));
	record = &(*dataset)->data;

	while(offset < filesize) {
		MPI_File_read_at_all(file, offset, buffer, buffersize, MPI_CHAR, &status);
		MPI_Get_count(&status, MPI_CHAR, &count);
	
		/* find where the last full record ends */
		last_full_record_end = count - 1;
		while(buffer[last_full_record_end] != '\n' && last_full_record_end >= 0) {
			last_full_record_end--;
		}
		if(last_full_record_end < 0) {
			fprintf(stderr, "%d: Error in clusterGIS_Load_data_replicated: buffersize is too small, %d\n", clusterGIS_rank, count);
			MPI_Abort(MPI_COMM_WORLD, 1);
		}

		/* Put the records into the dataset */
		i = 0;
		while (i < last_full_record_end) {
			i = clusterGIS_create_record(buffer, i, record);
			(*record)->next = NULL;
			record = &(*record)->next;
			i++;
		}

		offset = offset + last_full_record_end + 1;
	}

	free(buffer);
	MPI_File_close(&file);
}

/* Returns index where record ends (\n)*/
int clusterGIS_create_record(char* data, int start, clusterGIS_record** record) {
	int end = start;
	int field_end;
	int field_count;
	int field_start;
	int i;
	int j;

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
	*record = (clusterGIS_record*) malloc(sizeof(clusterGIS_record));
	(*record)->data = malloc(field_count * sizeof(char*));
	(*record)->columns = field_count;
	for(i = 0; i < field_count; i++) {
		(*record)->data[i] = current->data;
		head = current->next;
		free(current);
		current = head;
	}

	return end;
}

void clusterGIS_Free_dataset(clusterGIS_dataset** dataset) {
	clusterGIS_record* head;
	clusterGIS_record* current;

	head = (*dataset)->data;
	current = head;
	while(current != NULL) {
		head = current->next;
		free(current);
		current = head;
	}

	free(*dataset);
}

int clusterGIS_Write_record(MPI_File* file, clusterGIS_record* record) {
	char* buffer;
	int buffersize = 2*1024*1024;
	int count = 0;
	int i;
	int j;
	int err;
	MPI_Status status;

	buffer = (char*) malloc(buffersize);

	/* convert record to csv */
	for(i=0; i < record->columns; i++) {
		buffer[count] = '"';
		count++;
		j = 0;
		while(record->data[i][j] != '\0') {
			buffer[count] = record->data[i][j];
			count++;
			j++;
		}
		buffer[count] = '"';
		count++;
		buffer[count]= ',';
		count++;
	}
	buffer[count-1] = '\n';

	/* Write to a shared pointer file */
	err = MPI_File_write_shared(*file, buffer, count, MPI_CHAR, &status);

	free(buffer);
	return err;
}
