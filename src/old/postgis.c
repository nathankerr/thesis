#include "pgis.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define RETRY_COUNT 15

/* Setups and starts a local instance of PostgreSQL. Then creates a PostGIS database and connects to it */
void postgis_start(char* pgdata) {
	char cmd[256];
	int count = 0;

	status("Starting PostGIS");

	postgis_data = malloc(sizeof(char)*(strlen(pgdata)+1));
	strcpy(postgis_data, pgdata);

	status("Creating local database");
	sprintf(cmd, "initdb -A trust -D %s > /dev/null", postgis_data);
	check(system(cmd));

	status("Starting PostgreSQL");
	sprintf(cmd, "pg_ctl -D %s -o \"-h '' --unix_socket_directory=%s\" start > /dev/null", postgis_data, postgis_data);
	check(system(cmd));

	while(postgis_status()) {
		count++;
		check(count > RETRY_COUNT);
		sleep(1);
	}
	status("PostgreSQL Started");

	status("Creating PostGIS database");
	sprintf(cmd, "createdb -h %s pgis > /dev/null", postgis_data);
	check(system(cmd));
	sprintf(cmd, "createlang -h %s plpgsql pgis > /dev/null", postgis_data);
	check(system(cmd));
	sprintf(cmd, "psql -h %s -d pgis -f /home/alaster/pgis/software/share/lwpostgis.sql 2> /dev/null > /dev/null", postgis_data);
	check(system(cmd));

	status("Connecting to PostgreSQL");
	sprintf(cmd, "host = %s dbname = pgis", postgis_data);
	postgis_conninfo = malloc(sizeof(char)*(strlen(cmd)+1));
	strcpy(postgis_conninfo, cmd);
	check(PQstatus(postgis_connection()) != CONNECTION_OK);

	status("PostGIS Started");
}

/* Stops the PostgreSQL connection and server. Removes the local setup */
void postgis_stop(void) {
	char cmd[256];
	status("Stopping PostGIS");

	status("Disconnecting from PostgreSQL");
	PQfinish(postgis_connection());

	status("Stopping PostgreSQL");
	sprintf(cmd, "pg_ctl -D %s status > /dev/null", postgis_data);
	if (system(cmd))
		error("Unable to stop PostgreSQL");

	status("Removing local database");
	sprintf(cmd, "rm -rf %s > /dev/null", postgis_data);
	if (system(cmd))
		error("Unable to remove local database");

	free(postgis_data);
	status("PostGIS Stopped");
}

/* Checks to see if the PostgreSQL server is accepting connections */
int postgis_status(void) {
	char cmd[256];
	sprintf(cmd, "psql -h %s postgres -c '' 2> /dev/null > /dev/null", postgis_data);
	return system(cmd);
}

/* Returns an active connection. If the connection died, reconnects first */
PGconn* postgis_connection(void) {
	if (PQstatus(postgis_conn) != CONNECTION_OK) {
		PQfinish(postgis_conn);
		postgis_conn = PQconnectdb(postgis_conninfo);
	}
	return postgis_conn;
}
