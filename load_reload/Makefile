CFLAGS = -L$(PWD) -Wall -Werror

all: attacker.out victim.out

asm: attacker.s victim.s

attacker.out: attacker.c libarray.so
	$(CC) $(CFLAGS) -o attacker.out attacker.c -larray -lpthread

attacker.s: attacker.c libarray.so
	$(CC) $(CFLAGS) -S -o attacker.s attacker.c -larray -lpthread

victim.out: victim.c libarray.so
	$(CC) $(CFLAGS) -o victim.out victim.c -larray

victim.s: victim.c libarray.so
	$(CC) $(CFLAGS) -S -o victim.s victim.c -larray

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
	rm -f libarray.o libarray.so attacker.out attacker.s victim.out victim.s
