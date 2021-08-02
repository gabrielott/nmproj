#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#define MEGABYTE ((size_t) 1024 * 1024)

int main(void) {
	char ptr[MEGABYTE];
	uint64_t average = 0;

	for (uint64_t i = 0; i < MEGABYTE; i++) {
		uint64_t volatile time;
		void *addr = ptr + i;

		asm volatile (
				"mfence                \n\t"
				"lfence                \n\t"
				"rdtsc                 \n\t"
				"lfence                \n\t"
				"movl %%eax, %%ebx     \n\t"
				"movb (%[addr]), %%al  \n\t"
				"lfence                \n\t"
				"rdtsc                 \n\t"
				"subl %%ebx, %%eax     \n\t"
#ifdef FLUSH
				"clflush (%[addr])     \n\t"
#endif
				: "=a" (time)
				: [addr] "c" (addr)
				: "rdx", "ebx"
				);

		average += time;
	}

	average /= MEGABYTE;
	printf("Average: %ld\n", average);

	return EXIT_SUCCESS;
}
