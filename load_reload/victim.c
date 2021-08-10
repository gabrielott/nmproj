#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include "libarray.h"

int main(void) {
	int bytes[] = {12, 21, 34, 43};

	uint8_t const *array = get_array();

	for (int i = 0; i < 4; i++) {
		printf("%hhu\n", array[bytes[i]]);
	}

	sleep(3);
}
