CFLAGS = -L$(PWD) -Wall -Werror

all: attacker.out victim.out

attacker.out: attacker.c libbadhash.so
	$(CC) $(CFLAGS) -o attacker.out attacker.c -lbadhash -lpthread

victim.out: victim.c libbadhash.so
	$(CC) $(CFLAGS) -o victim.out victim.c -lbadhash

libbadhash.so: libbadhash.c libbadhash.h
	$(CC) $(CFLAGS) -c -fpic -o libbadhash.o libbadhash.c
	$(CC) $(CFLAGS) -shared -o libbadhash.so libbadhash.o

.PHONY: run clean

run: export LD_LIBRARY_PATH = $(PWD)
run: attacker.out victim.out
	@echo '=ATTACKER='
	@taskset --cpu-list 0 ./attacker.out &
	@echo '=VICTIM='
	@taskset --cpu-list 8 ./victim.out

clean:
	rm -f libbadhash.o libbadhash.so attacker.out victim.out
