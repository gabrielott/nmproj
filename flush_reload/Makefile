CFLAGS = -Wall

all: flush no_flush

flush: flush_reload.c
	$(CC) -DFLUSH -o flush.out flush_reload.c

no_flush: flush_reload.c
	$(CC) -o no_flush.out flush_reload.c

.PHONY: run clean
run: flush no_flush
	@echo '= NO FLUSH ='
	@./no_flush.out
	@echo '= FLUSH ='
	@./flush.out

clean:
	rm -f flush.out no_flush.out
