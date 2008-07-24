SHELL=/bin/sh

CC = mpicc
CFLAGS = -O3 -Wall -I${PGIS}/software/include
LDFLAGS = -lpq -L${PGIS}/software/lib

pgis_objects = pgis.o

.PHONY: all
all: pgis

pgis: ${pgis_objects}
	${CC} -o $@ ${CFLAGS} ${pgis_objects} ${LDFLAGS}

clean:
	-rm *.o pgis
