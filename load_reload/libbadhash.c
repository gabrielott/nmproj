#include <stdio.h>
#include <stdint.h>

__attribute__ ((noinline))
void handle_zero(void) {
	printf("0");
}

__attribute__ ((noinline))
void handle_one(void) {
	printf("1");
}

int64_t badhash(uint8_t *bytes, uint32_t size) {
	int64_t sum = 0;

	for (uint32_t i = 0; i < size; i++) {
		uint8_t byte = bytes[i];
		while (byte) {
			if (byte & 1) {
				sum++;
				handle_zero();
			} else {
				sum--;
				handle_one();
			}
			byte >>= 1;
		}
	}

	return sum;
}
