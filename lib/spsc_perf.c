#include "spsc.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#define BUFFER_SIZE 4 * 1024 * 1024
#define NUM_MESSAGES 100000000

#define SLEEP_NANOS 200

void publish(char* ring_name)
{
	spsc_ring ring;
	if (spsc_create_pub(&ring, ring_name, BUFFER_SIZE)) return;

	char msg[135];
	if (spsc_write(&ring, "start", 5) == 0)
	{
		puts("Failed to write.");
		return;
	}

	long count = 0;
	long retry_count = 0;

	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = SLEEP_NANOS;
	clock_t t = clock();
	while (count < NUM_MESSAGES)
	{
		if (spsc_write(&ring, msg, 135) == 135) count++;
		else
		{
			retry_count++;
			nanosleep(&ts, 0);
		}
	}

	t = clock() - t;
	printf("Retry count: %ld\n", retry_count);
	return;
}

void subscribe(char* ring_name)
{
	spsc_ring ring;
	if (spsc_create_sub(&ring, ring_name, BUFFER_SIZE)) return;
	puts("Waiting for messages...");

	char buf[256];
	long count = 0;
	int started = 0;
	
	clock_t t;
	
	while (1)
	{
		size_t len = spsc_read(&ring, buf, 256);
		if (started)
		{
			if (len > 0 && ++count == NUM_MESSAGES) break;
		}
		else if (len != 0)
		{
			puts("Started clock.");
			started = 1;
			t = clock();
		}
	}
	t = clock() - t;
	float elapsed = ((float) t) / CLOCKS_PER_SEC;

	printf("Message count: %ld\n", count);
	printf("Elapsed: %.2fs\n", elapsed);
	printf("Messages per second: %.0f\n", count / elapsed);
	return;
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
	}

	return 0;
}
