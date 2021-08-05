CFLAGS = -L$(PWD) -Wall -Werror

all: attacker.out victim.out

attacker.out: attacker.c libarray.so
	$(CC) $(CFLAGS) -o attacker.out attacker.c -larray -lpthread

victim.out: victim.c libarray.so
	$(CC) $(CFLAGS) -o victim.out victim.c -larray

libarray.so: libarray.c libarray.h
	$(CC) $(CFLAGS) -c -fpic -o libarray.o libarray.c
	$(CC) $(CFLAGS) -shared -o libarray.so libarray.o

.PHONY: run clean

run: export LD_LIBRARY_PATH = $(PWD)
run: attacker.out victim.out
	@echo '=ATTACKER='
	@taskset --cpu-list 0 ./attacker.out &
	@echo '=VICTIM='
	@taskset --cpu-list 8 ./victim.out

clean:
	rm -f libarray.o libarray.so attacker.out victim.out
