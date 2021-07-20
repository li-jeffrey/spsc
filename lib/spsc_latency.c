#include "spsc.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <x86intrin.h>

#define BUFFER_SIZE 4 * 1024 * 1024
#define NUM_MESSAGES 500000
#define SLEEP_TIME_NS 1000


static double get_tick_to_ns_factor()
{
	uint64_t tsc1, tsc2;
	struct timespec clk1, clk2, sleep_ts;
	sleep_ts.tv_sec = 2;
	sleep_ts.tv_nsec = 0;

	tsc1 = __rdtsc();
	clock_gettime(CLOCK_REALTIME, &clk1);
	nanosleep(&sleep_ts, NULL);
	clock_gettime(CLOCK_REALTIME, &clk2);
	tsc2 = __rdtsc();
	return (tsc2 - tsc1) / (clk2.tv_sec * 1e9 + clk2.tv_nsec - clk1.tv_sec * 1e9 - clk1.tv_nsec);
}

typedef struct payload
{
	size_t id;
	uint64_t ts;
} payload;

void pinger(char* send_ring_name, char* rcv_ring_name, int64_t* results)
{
	spsc_ring rcv_ring;
	spsc_ring send_ring;
	if (spsc_create_sub(&rcv_ring, rcv_ring_name, BUFFER_SIZE)) exit(1);
	if (spsc_create_pub(&send_ring, send_ring_name, BUFFER_SIZE)) exit(1);

	struct timespec sleep_ts;
	sleep_ts.tv_sec = 0;
	sleep_ts.tv_nsec = 1000;

	size_t count = 0;
	payload send = {0,0};
	payload rcv = {0,0};

	send.ts = __rdtsc();
	spsc_write(&send_ring, &send, sizeof(payload));
	while (count < NUM_MESSAGES)
	{
		if (spsc_read(&rcv_ring, &rcv, sizeof(payload)) > 0)
		{
			int64_t now = __rdtsc();
			results[rcv.id] = now - rcv.ts;

			send.id = count++;
			nanosleep(&sleep_ts, NULL);
			
			send.ts = __rdtsc();
			if (spsc_write(&send_ring, &send, sizeof(payload)) != sizeof(payload))
				puts("Pinger - Warning: buffered.");
		}
	}
}

void ponger(char* send_ring_name, char* rcv_ring_name)
{
	spsc_ring rcv_ring;
	spsc_ring send_ring;
	if (spsc_create_sub(&rcv_ring, rcv_ring_name, BUFFER_SIZE)) exit(1);
	if (spsc_create_pub(&send_ring, send_ring_name, BUFFER_SIZE)) exit(1);

	payload rcv;
	while (1)
	{
		if (spsc_read(&rcv_ring, &rcv, sizeof(payload)) > 0)
		{
			if (spsc_write(&send_ring, &rcv, sizeof(payload)) != sizeof(payload))
				puts("Ponger - Warning: buffered.");
			if (rcv.id == NUM_MESSAGES - 1) break;
		}
	}
}

int sort_cmp(const void* a, const void* b)
{
	return (*(int64_t*) a - *(int64_t*) b);
}

void print_percentile(long* values, float pct, double conv_factor)
{
	float p = pct / 100.0;
	size_t idx = (size_t) ceil(NUM_MESSAGES * p);
	printf("P%.0f: %.0f\n", pct, values[idx] / conv_factor);
}

int main(int argc, char** argv)
{
	char ring1[] = "/tmp/ringXXXXXX";
	char ring2[] = "/tmp/ringXXXXXX";
	if (mkstemp(ring1) == -1 || mkstemp(ring2) == -1)
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
		ponger(ring1, ring2);
		return 0;
	}

	// parent process
	double conv_factor = get_tick_to_ns_factor();
	printf("Ticks/ns: %.2f\n", conv_factor);

	int64_t* results = (int64_t*) malloc(sizeof(int64_t) * NUM_MESSAGES);
	pinger(ring2, ring1, results);
	while(wait(NULL) > 0);

	puts("Processing results...");
	qsort(results, NUM_MESSAGES, sizeof(int64_t), sort_cmp);

	print_percentile(results, 50.0, conv_factor);
	print_percentile(results, 75.0, conv_factor);
	print_percentile(results, 90.0, conv_factor);
	print_percentile(results, 95.0, conv_factor);
	print_percentile(results, 99.0, conv_factor);

	return 0;
}
