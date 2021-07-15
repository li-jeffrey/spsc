#include "spsc.h"

#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>
#include <math.h>

#define HEADER_BYTE_1 0x52
#define HEADER_BYTE_2 0x42
#define VERSION 1


static inline size_t get_closest_power_of_two(const size_t size)
{
	return 1 << (size_t) ceil(log2((double) size));
}

static int spsc_create(spsc_ring* ring, const char* pathname, const size_t size)
{
	int permissions = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
	int fd = open(pathname, O_RDWR | O_CREAT, permissions);
	if (fd == -1)
	{
		perror("open");
		return -1;
	}

	int protection = PROT_READ | PROT_WRITE;
	int visibility = MAP_SHARED;
	size_t rounded_size = get_closest_power_of_two(size);
	size_t mmap_size = sizeof(spsc_ring_data) + rounded_size;
	if (posix_fallocate(fd, 0, mmap_size + 1) != 0)
	{
		perror("fallocate");
		close(fd);
		return -1;
	}

	void* addr = mmap(NULL, mmap_size, protection, visibility, fd, 0);
	if (addr == MAP_FAILED)
	{
		perror("mmap");
		return -1;
	}
	close(fd);

	spsc_ring_data* spsc_data = (spsc_ring_data*) addr;
	char* header = spsc_data->_header;
	if (header[0] != HEADER_BYTE_1 || header[1] != HEADER_BYTE_2)
	{
		header[0] = HEADER_BYTE_1;
		header[1] = HEADER_BYTE_2;
		spsc_data->_rpos = 0;
		spsc_data->_wpos = 0;
		spsc_data->_version = VERSION;
		spsc_data->_size = rounded_size;
		printf("Created shared memory file at %s\n", pathname);
	}
	else if (spsc_data->_size != rounded_size)
	{
		puts("Error: existing buffer does not match specified size");
		munmap(spsc_data, sizeof(spsc_ring_data));
		return -1;
	}

	ring->_data = spsc_data;
	return 0;
}

int spsc_create_sub(spsc_ring* ring, const char* pathname, const size_t size)
{
	if (spsc_create(ring, pathname, size) == -1) return -1;

	ring->mode = READ_MODE;
	return 0;
}

int spsc_create_pub(spsc_ring* ring, const char* pathname, const size_t size)
{
	if (spsc_create(ring, pathname, size) == -1) return -1;

	ring->mode = WRITE_MODE;
	return 0;
}

static inline size_t _read(spsc_ring_data* data, size_t offset, char* dest, size_t n)
{
	if (offset + n > data->_size)
	{
		size_t overflow = offset + n - data->_size;
		memcpy(dest, data->_buf + offset, n - overflow);
		memcpy(dest + n - overflow, data->_buf, overflow);
	}
	else
	{
		memcpy(dest, data->_buf + offset, n);
	}

	return n;
}

size_t spsc_read(spsc_ring* ring, char* buf, size_t n)
{
	if (ring->mode != READ_MODE) return 0;

	spsc_ring_data* data = ring->_data;
	size_t rpos;
	size_t wpos;

	__atomic_load(&data->_rpos, &rpos, __ATOMIC_RELAXED);
	__atomic_load(&data->_wpos, &wpos, __ATOMIC_ACQUIRE);
	if (rpos == wpos)
	{
		return 0;
	}

	size_t size = data->_size;
	size_t data_offset = sizeof(MSG_SIZE_T);
	char msg_size_bytes[data_offset];
	rpos += _read(data, rpos & (size - 1), msg_size_bytes, data_offset);
	
	MSG_SIZE_T msg_size = *(MSG_SIZE_T*) msg_size_bytes;

	size_t read = msg_size > n ? n : msg_size;
	_read(data, rpos & (size - 1), buf, read);

	rpos += msg_size;
	__atomic_store(&data->_rpos, &rpos, __ATOMIC_RELEASE);
	return read;
}

static inline size_t _write(spsc_ring_data* data, size_t offset, char* src, size_t n)
{
	if (offset + n > data->_size)
	{
		size_t overflow = offset + n - data->_size;
		memcpy(data->_buf + offset, src, n - overflow);
		memcpy(data->_buf, src + n - overflow, overflow);
	}
	else
	{
		memcpy(data->_buf + offset, src, n);
	}

	return n;
}

MSG_SIZE_T spsc_write(spsc_ring* ring, char* buf, MSG_SIZE_T n)
{
	if (ring->mode != WRITE_MODE) return 0;

	spsc_ring_data* data = ring->_data;
	size_t wpos;
	__atomic_load(&data->_wpos, &wpos, __ATOMIC_RELAXED);

	size_t rpos;
	__atomic_load(&data->_rpos, &rpos, __ATOMIC_ACQUIRE);

	size_t size = data->_size;
	size_t data_offset = sizeof(MSG_SIZE_T);
	if (wpos + data_offset + n - rpos > size)
	{
		return 0;
	}

	char* msg_size_bytes = (char*) &n;
	wpos += _write(data, wpos & (size - 1), msg_size_bytes, data_offset);
	wpos += _write(data, wpos & (size - 1), buf, n);

	__atomic_store(&data->_wpos, &wpos, __ATOMIC_RELEASE);
	return n;
}

void spsc_destroy(spsc_ring* ring)
{
	munmap(ring->_data, sizeof(spsc_ring_data));
}
