#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>

#include "util.h"

int main(void) {
	set_affinity(8);

	uint64_t *data = map_file((void *) 0x80000);

	for (;;) {
		read_byte(data + 1234);
	}
}
