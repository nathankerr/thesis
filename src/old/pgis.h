#ifndef PGIS
#define PGIS

#include <libpq-fe.h>

int rank;
int size;

void status(char*);
void error_abort(char*, char*, int);
#define error(m) error_abort(m, __FILE__, __LINE__)
#define check(A) if (A) error(#A " failed")

void postgis_start(char*);
void postgis_stop(void);
int postgis_status(void);
PGconn* postgis_connection(void);
char* postgis_data;
char* postgis_conninfo;
PGconn* postgis_conn;

#endif
