#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "util.h"

int main(void) {
	uint64_t indices[] = {50000, 16000, 80000, 90000};

	int fd = open(DATA, O_RDONLY);
	uint64_t *data = mmap(NULL, FILE_SIZE, PROT_READ, MAP_PRIVATE, fd, 0);

	for (;;) {
		for (uint64_t i = 0; i < 4; i++) {
			asm volatile (
				"movq (%%rcx), %%rcx \n\t"
				:
				: "c" (data + indices[i])
				:
			);
		}
	}
}
