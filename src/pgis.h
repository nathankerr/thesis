#ifndef PGIS
#define PGIS

int rank;
int size;

void status(char*);
void error_abort(char*, char*, int);
#define error(m) error_abort(m, __FILE__, __LINE__)
#define check(A) if (A) error(#A " failed")

void postgis_start(char*);
void postgis_stop(void);
int postgis_status(void);
char* postgis_data;

#endif
