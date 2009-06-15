SHELL=/bin/sh

CC = mpicc
CFLAGS = -O3 -Wall -I${PGIS}/software/include
LDFLAGS = -lpq -L${PGIS}/software/lib

pgis_objects = pgis.o postgis.o
Source = ${pgis_objects:.o=.cc}

.PHONY: all
all: pgis

include $(Source:.cc=.d)

%.d: %.c
	@set -e; rm -f $@; \
	${CC} -MM ${CFLAGS} $< > $@.$$$$ 2>/dev/null; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

pgis: ${pgis_objects}
	${CC} -o $@ ${CFLAGS} ${pgis_objects} ${LDFLAGS}

clean:
	-rm *.o *.d pgis
