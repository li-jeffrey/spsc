#include "spsc.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFFER_SIZE 4 * 1024 * 1024
#define NUM_MESSAGES 500000000


void publish(char* ring_name)
{
	spsc_ring ring;
	if (spsc_create_pub(&ring, ring_name, BUFFER_SIZE)) return;

	uint64_t count = 0;
	uint64_t retry_count = 0;

	char msg[135];
	while (count <= NUM_MESSAGES)
	{
		if (spsc_write(&ring, msg, 135) == 135) count++;
		else
		{
			retry_count++;
			usleep(1);
		}
	}

	printf("Retry count: %ld\n", retry_count);
}

void subscribe(char* ring_name)
{
	spsc_ring ring;
	if (spsc_create_sub(&ring, ring_name, BUFFER_SIZE)) return;
	puts("Waiting for messages...");

	char buf[256];
	uint64_t count = 0;
	
	clock_t t;
	while(1)
	{
		if (spsc_read(&ring, buf, 256) > 0) break;
	}
	
	puts("Started clock.");
	t = clock();
	while (1)
	{
		if (spsc_read(&ring, buf, 256) > 0 && ++count == NUM_MESSAGES) break;
	}
	t = clock() - t;
	float elapsed = ((float) t) / CLOCKS_PER_SEC;

	printf("Message count: %ld\n", count);
	printf("Elapsed: %.2fs\n", elapsed);
	printf("Messages per second: %.0f\n", count / elapsed);
}

int main(int argc, char** argv)
{
	char ring_name[] = "/tmp/ringXXXXXX";
	if (mkstemp(ring_name) == -1)
	{
		perror("mkstemp");
		return 1;
	}

	int pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return 1;
	} 
	else if (pid == 0)
	{
		// child process
		sleep(1);
		publish(ring_name);
	} 
	else 
	{
		subscribe(ring_name);
		while(wait(NULL) > 0);
	}

	return 0;
}
