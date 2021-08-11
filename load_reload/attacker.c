#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <sched.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "util.h"

/* These have to be different logical threads on the same physical core for some reason */
#define MAIN_CORE 0
#define COUNTING_CORE 8

/* #define RDTSC */

#define RAND_INDEX (rand() % SIZE)

void set_affinity(uint32_t core);
uint64_t load_count(void *addr);

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

void set_affinity(uint32_t core) {
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(core, &cpuset);

	pthread_t current = pthread_self();
	pthread_setaffinity_np(current, sizeof(cpu_set_t), &cpuset);
}

uint64_t get_average(uint8_t *data) {
	uint64_t avg = 0;
	for (int i = 0; i < 100; i++) {
		uint64_t time = load_count((void *) (data + RAND_INDEX));
		avg += time;
	}

	return avg / 100;
}

#ifndef RDTSC
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
#else
uint64_t load_count(void *addr) {
	uint64_t volatile time;
	asm volatile (
		"mfence             \n\t"
		"lfence             \n\t"
		"rdtsc              \n\t"
		"lfence             \n\t"
		"movq %%rax, %%rbx  \n\t"
		"movb (%%rcx), %%cl \n\t"
		"lfence             \n\t"
		"rdtsc              \n\t"
		"subq %%rbx, %%rax  \n\t"
		: "=a" (time)
		: "c" (addr)
		: "rbx"
	);
	return time;
}
#endif

int main(void) {
	set_affinity(MAIN_CORE);
	srand(time(NULL));

#ifndef RDTSC
	pthread_t thread;
	pthread_create(&thread, NULL, counting_thread, NULL);
	while (!count)
		;
#endif

	int fd = open(DATA, O_RDONLY);
	uint8_t *data = mmap(NULL, SIZE, PROT_READ, MAP_PRIVATE, fd, 0);

	if (data == MAP_FAILED) {
		printf("Erro mmap.\n");
		exit(1);
	}

	uint64_t avg;
	printf("Average 1: %lu\n", get_average(data));
	sleep(1);
	printf("Average 2: %lu\n", get_average(data));
	sleep(1);
	printf("Average 3: %lu\n", (avg = get_average(data)));

	sleep(3);

	for (;;) {
		for (uint64_t i = 0; i < SIZE; i++) {
			uint64_t index = RAND_INDEX;
			uint64_t time = load_count((void *) (data + index));
			if (time > avg + 100)
				printf("%lu -- %lu\n", index, time);
		}
		sleep(1);
		printf("=====\n");
	}
}
