#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include "libbadhash.h"

uint64_t volatile count = 0;

void *counting_thread(void *args) {
	asm volatile (
		"xorq %%rax, %%rax   \n\t"
		"loop%=:             \n\t"
		"incq %%rax          \n\t"
		"movq %%rax, (%%rbx) \n\t"
		"jmp loop%=          \n\t"
		:
		: "b" (&count)
		: "rax"
	);

	pthread_exit(NULL);
}

int main(void) {
	pthread_t thread;
	pthread_create(&thread, NULL, counting_thread, NULL);

	printf("%lu\n", count);
	printf("%lu\n", count);
	printf("%lu\n", count);
	printf("%lu\n", count);
	printf("%lu\n", count);
	printf("%lu\n", count);
	printf("%lu\n", count);
	printf("%lu\n", count);
	printf("%lu\n", count);
	printf("%lu\n", count);
	printf("%lu\n", count);
	printf("%lu\n", count);
	printf("%lu\n", count);
	printf("%lu\n", count);
	printf("%lu\n", count);
	printf("%lu\n", count);
}
