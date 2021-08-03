#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include "libbadhash.h"

int main(void) {
	uint8_t bytes[] = {'f', 'l', 'a', 'g'};

	for (;;) {
		sleep(1);
		int64_t hash = badhash(bytes, 4);
		printf("%ld\n", hash);
	}
}
