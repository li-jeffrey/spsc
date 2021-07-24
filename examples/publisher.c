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
	if (spsc_create_pub(&ring, argv[1], BUFFER_SIZE) == -1) return 1;

	puts("Enter text: ");
	char buf[256];
	while (fgets(buf, 256, stdin))
	{
		if (spsc_write(&ring, buf, 255) == 0)
		{
			puts("Failed to write.");
		}
		puts("Enter text: ");
	}

	return 0;
}
