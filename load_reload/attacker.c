#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include "libarray.h"

/* These have to be different logical threads on the same physical core for some reason */
#define MAIN_CORE 0
#define COUNTING_CORE 8

uint64_t volatile count = 0;

void set_affinity(uint32_t core) {
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(core, &cpuset);

	pthread_t current = pthread_self();
	pthread_setaffinity_np(current, sizeof(cpu_set_t), &cpuset);
}

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

void load(void *addr) {
	asm volatile (
		"movq (%%rax), %%rax \n\t"
		:
		: "a" (addr)
		:
	);
}

uint64_t load_count(void *addr) {
	uint64_t volatile time;

	asm volatile (
		"mfence              \n\t"
		"lfence              \n\t"
		"movq (%%rbx), %%rcx \n\t"
		"lfence              \n\t"
		"movb (%%rax), %%al  \n\t"
		"lfence              \n\t"
		"movq (%%rbx), %%rax \n\t"
		"subq %%rcx, %%rax   \n\t"
		: "=a" (time)
		: "a" (addr), "b" (&count)
		: "rcx"
	);

	return time;
}

int main(void) {
	set_affinity(MAIN_CORE);

	pthread_t thread;
	pthread_create(&thread, NULL, counting_thread, NULL);
	while (!count)
		;

	uint8_t const *array = get_array();

	uint64_t avg = 0;
	for (int i = 0; i < SIZE; i++) {
		uint64_t time = load_count((void *) (array + i));
		avg += time;
		printf("%d=%lu\n", i, time);
	}

	avg /= SIZE;
	printf("Average: %lu\n", avg);
	sleep(3);

	for (int i = 0; i < SIZE; i++) {
		uint64_t time = load_count((void *) (array + i));
		if (time > avg + 5)
			printf("%d -- %lu\n", i, time);
	}
}
