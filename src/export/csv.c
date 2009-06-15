#include "libpq-fe.h"
#include <stdlib.h>

int main(int argc, char **argv) {
	PGconn *conn;
	PGresult *res;

	conn = PQconnectdb("host = 10.0.30.11 user = alaster password = mwd312 dbname = DigitalPhoenix");
	if (PQstatus(conn) != CONNECTION_OK) {
		fprintf(stderr, "Connection failed: %s", PQerrorMessage(conn));
		PQfinish(conn);
		exit(1);
	}

	// jobs
	//res = PQexec(conn, "SELECT id, emp01_, emp01_id, empid, name1, name2, address, suite, city, citycode, state, zipcode, zip5, siccode, naicscode, employment, sic, sic_rev, sictext, econ_base, ind_cluste, maj_ind, mag_lucode, mag_landus, mpa, job_center, x, y, oid_, field2, astext(the_geom), devtype FROM nathan.jobs_coded");

	// parcels
	res = PQexec(conn, "SELECT id, area, perimeter, pars300_, pars300_id, apn, filename, source, puc, lot, bloc, trac, site_addre, suite, site_city, site_zip, owner_name, land_fcv, impr_fcv, lot_size, lvg_ft, \"year\", pool, p_id, astext(the_geom), sector_type_secondary, devtype FROM nathan.parcels_coded order by id");

	int ntuples = PQntuples(res);
	int nfields = PQnfields(res);
	int i;
	int j;

	for (i=0; i < ntuples; i++) {
		for (j=0; j < nfields; j++) {
			printf("\"%s\"", PQgetvalue(res, i, j));
			if(j != nfields - 1) {
				printf(",");
			}
		}
		printf("\n");
	}
	PQclear(res);

//	res = PQexec(conn, "END");
//	PQclear(res);

	PQfinish(conn);
	return 0;
}
