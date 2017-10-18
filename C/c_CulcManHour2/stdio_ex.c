#include "stdio_ex.h"

void flush() {
	int c;
	while(1) {
		c = getchar();
		if (c == 10 || c == EOF) break;
	}
}