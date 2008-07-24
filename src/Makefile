SHELL=/bin/sh

CC = mpicc
CFLAGS = -O3 -Wall
LDFLAGS = -lpq

pgis_objects = pgis.o

.PHONY: all
all: pgis

pgis: ${pgis_objects}
	${CC} -o $@ ${CFLAGS} ${pgis_objects} ${LDFLAGS}

clean:
	-rm *.o pgis
