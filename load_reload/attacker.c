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
#include <signal.h>
#include "util.h"

/* These have to be different logical threads on the same physical core for some reason */
#define MAIN_CORE 0
#define COUNTING_CORE 8

/* #define RDTSC */

#define RAND_INDEX (rand() % INDEX_NUM)

void set_affinity(uint32_t core);
uint64_t load_count(uint64_t *addr);

uint64_t volatile count = 0;
uint64_t *total_times, *times_accessed, *avgs;

void sigint_handler(int sig) {
	uint64_t avg_all = 0;
	uint64_t accessed_all = 0;

	for (uint64_t i = 0; i < INDEX_NUM; i++) {
		if (times_accessed[i] == 0)
			avgs[i] = 0;
		else
			avgs[i] = total_times[i] / times_accessed[i];
		avg_all += total_times[i];
		accessed_all += times_accessed[i];
	}

	avg_all /= accessed_all;
	printf("Average of all indices: %lu\n", avg_all);

	printf("=== NOTABLE AVERAGES ===\n");
	for (uint64_t i = 0; i < INDEX_NUM; i++)
		if (avgs[i] > avg_all + 20)
			printf("%06lu (%03lu)- %lu\n", i, times_accessed[i], avgs[i]);

	printf("\n=== ALL AVERAGES ===\n");
	for (uint64_t i = 0; i < INDEX_NUM; i++)
		printf("%06lu (%03lu)- %lu\n", i, times_accessed[i], avgs[i]);
	exit(0);
}

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
	signal(SIGINT, sigint_handler);
	srand(time(NULL));

	int fd = open(DATA, O_RDONLY);
	uint64_t *data = mmap(NULL, FILE_SIZE, PROT_READ, MAP_PRIVATE, fd, 0);
	if (data == MAP_FAILED) {
		fprintf(stderr, "mmap error.\n");
		exit(1);
	}

	total_times = calloc(sizeof(uint64_t), INDEX_NUM);
	times_accessed = calloc(sizeof(uint64_t), INDEX_NUM);
	avgs = calloc(sizeof(uint64_t), INDEX_NUM);

#ifndef RDTSC
	pthread_t thread;
	pthread_create(&thread, NULL, counting_thread, NULL);
	while (!count)
		;
#endif

	for (;;) {
		fprintf(stderr, "Tick\n");
		for (uint64_t i = 0; i < INDEX_NUM; i++) {
			uint64_t index = RAND_INDEX;
			uint64_t time = load_count(data + index);

			total_times[index] += time;
			times_accessed[index]++;
		}
		sleep(1);
	}
}
