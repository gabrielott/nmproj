#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "util.h"

/* These have to be different logical threads on the same physical core for some reason */
#define MAIN_CORE 0
#define COUNTING_CORE 8

/* #define RDTSC */

uint64_t load_count(uint64_t *addr);

uint64_t volatile count = 0;

#ifndef RDTSC
void *counting_thread(void *args) {
	set_affinity(COUNTING_CORE);
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
#endif

#ifndef RDTSC
uint64_t load_count(uint64_t *addr) {
	uint64_t volatile time;
	asm volatile (
		"mfence               \n\t"
		"lfence               \n\t"
		"movq (%%rbx), %%rcx  \n\t"
		"lfence               \n\t"
		"movq (%%rax), %%rax  \n\t"
		"lfence               \n\t"
		"movq (%%rbx), %%rax  \n\t"
		"subq %%rcx, %%rax    \n\t"
		: "=a" (time)
		: "a" (addr), "b" (&count)
		: "rcx"
	);
	return time;
}
#else
uint64_t load_count(uint64_t *addr) {
	uint64_t volatile time;
	asm volatile (
		"mfence              \n\t"
		"lfence              \n\t"
		"rdtsc               \n\t"
		"lfence              \n\t"
		"movq %%rax, %%rbx   \n\t"
		"movq (%%rcx), %%rcx \n\t"
		"lfence              \n\t"
		"rdtsc               \n\t"
		"subq %%rbx, %%rax   \n\t"
		: "=a" (time)
		: "c" (addr)
		: "rbx"
	);
	return time;
}
#endif

int main(void) {
	set_affinity(MAIN_CORE);

	uint64_t *data = map_file((void *) 0x50000);

#ifndef RDTSC
	pthread_t thread;
	pthread_create(&thread, NULL, counting_thread, NULL);
	while (!count)
		;
#endif
	
	double sum = 0;
	uint64_t samples;

	for (samples = 0; samples < 1e7; samples++) {
		uint64_t time = load_count(data + 1234);
		sum += time;
	}

	printf("%lf\n", sum / samples);
}
