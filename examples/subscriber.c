#include "spsc.h"

#include <stdio.h>

#define BUFFER_SIZE 1024 * 1024


int main(int argc, char** argv)
{
	if (argc < 2)
	{
		printf("Usage: %s [FILE]\n", argv[0]);
		return 1;
	}

	spsc_ring ring;
	if (spsc_create_sub(&ring, argv[1], BUFFER_SIZE) == -1) return 1;

	char buf[256];
	while (1)
	{
		size_t n_read = spsc_read(&ring, buf, 255); 
		if (n_read > 0)
		{
			buf[n_read] = '\0';
			printf("Received message: %s\n", buf);
		}
	}

	return 0;
}
