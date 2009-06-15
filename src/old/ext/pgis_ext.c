#include "postgres.h"
#include "fmgr.h"

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC;
#endif

PG_FUNCTION_INFO_V1(test);
Datum test(PG_FUNCTION_ARGS) {
	PG_RETURN_INT32(42);
}
