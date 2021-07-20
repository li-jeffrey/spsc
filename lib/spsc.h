#pragma once

#include <stdint.h>
#include <stdio.h>

#define MSG_SIZE_T uint32_t

#define READ_MODE 1
#define WRITE_MODE 2


typedef struct spsc_ring_data
{
	char _header[2];
	int _version;
	size_t _size;
	char __pad0[64];

	size_t _rpos;
	char __pad1[64];

	size_t _wpos;
	char __pad2[64];

	char _buf[];
} spsc_ring_data;

typedef struct spsc_ring
{
	spsc_ring_data* _data;
	int mode;
} spsc_ring;

int spsc_create_sub(spsc_ring* ring, const char* pathname, const size_t size);
int spsc_create_pub(spsc_ring* ring, const char* pathname, const size_t size);

size_t spsc_read(spsc_ring* ring, void* dest, size_t bytes);
MSG_SIZE_T spsc_write(spsc_ring* ring, void* src, MSG_SIZE_T n);

void spsc_destroy(spsc_ring* ring);
