CFLAGS = -Wall -Werror

all: attacker.out victim.out data

asm: attacker.s victim.s

data:
	dd if=/dev/random of=data bs=1M count=1 status=progress

attacker.out: attacker.c
	$(CC) $(CFLAGS) -g -o attacker.out attacker.c -lpthread

attacker.s: attacker.c
	$(CC) $(CFLAGS) -g -S -o attacker.s attacker.c -lpthread

victim.out: victim.c
	$(CC) $(CFLAGS) -g -o victim.out victim.c -lpthread

victim.s: victim.c
	$(CC) $(CFLAGS) -g -S -o victim.s victim.c

.PHONY: run clean

run: attacker.out victim.out
	@echo '=ATTACKER='
	@taskset --cpu-list 0 ./attacker.out &
	@echo '=VICTIM='
	@taskset --cpu-list 8 ./victim.out

clean:
	rm -f attacker.out attacker.s victim.out victim.s data
