#include "pgis.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define RETRY_COUNT 15

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

	status("PostGIS Started");
}

void postgis_stop(void) {
	char cmd[256];

	status("Stopping PostGIS");

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

int postgis_status(void) {
	char cmd[256];
	sprintf(cmd, "psql -h %s postgres -c '' 2> /dev/null > /dev/null", postgis_data);
	return system(cmd);
}
