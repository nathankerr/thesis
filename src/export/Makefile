SHELL=/bin/sh

CC = gcc
CFLAGS = -O3 -Wall -I${PGIS}/software/include
LDFLAGS = -lpq -L${PGIS}/software/lib

csv_objects = csv.o
Source = ${csv_objects:.o=.cc}

.PHONY: all
all: csv

#include $(Source:.cc=.d)

%.d: %.c
	@set -e; rm -f $@; \
	${CC} -MM ${CFLAGS} $< > $@.$$$$ 2>/dev/null; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

csv: ${csv_objects}
	${CC} -o $@ ${CFLAGS} ${csv_objects} ${LDFLAGS}

clean:
	-rm *.o *.d pgis
