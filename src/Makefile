SHELL=/bin/sh

CC = mpicc
CFLAGS = -O3 -Wall -g
LDFLAGS = 

pgis_objects = pgis.o clustergis.o
Source = ${pgis_objects:.o=.c}

.PHONY: all
all: pgis

include $(Source:.c=.d)

%.d: %.c
	@set -e; rm -f $@; \
	${CC} -MM ${CFLAGS} $< > $@.$$$$ 2>/dev/null; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

pgis: ${pgis_objects}
	${CC} -o $@ ${CFLAGS} ${pgis_objects} ${LDFLAGS}

clean:
	-rm *.o *.d* pgis
