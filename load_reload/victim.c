#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

#include "util.h"

int main(void) {
	uint64_t indices[] = {50000, 16000, 80000, 90000};

	int fd = open(DATA, O_RDONLY);
	uint8_t *data = mmap(NULL, SIZE, PROT_READ, MAP_PRIVATE, fd, 0);

	for (;;) {
		for (uint64_t i = 0; i < 4; i++) {
			printf("%hhu\n", data[indices[i]]);
		}
		sleep(1);
	}
}
