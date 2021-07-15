#include <stdlib.h>
#include <unistd.h>

#include "minunit.h"
#include "spsc.h"

int MU_TESTS_RUN = 0;


char* test_pub_sub()
{
	spsc_ring pub;
	spsc_ring sub;

	char ring_name[] = "/tmp/ringXXXXXX";
	if (mkstemp(ring_name) == -1)
	{
		perror("mkstemp");
		return "Failed to create ring";
	}

	if (spsc_create_pub(&pub, ring_name, 100)) return "Failed to create publisher";
	if (spsc_create_sub(&sub, ring_name, 100)) return "Failed to create subscriber";
	MU_ASSERT(pub._data->_size == 128);

	// write buffer until full
	for (int i = 0; i < 14; i++)
	{
		MU_ASSERT(spsc_write(&pub, "abcde", 5) == 5);
	}
	MU_ASSERT(spsc_write(&pub, "abcde", 5) == 0);

	// unmap publisher
	spsc_destroy(&pub);

	// read from buffer
	for (int i = 0; i < 14; i++)
	{
		char buf[6];
		MU_ASSERT(spsc_read(&sub, buf, 5) == 5);
		buf[5] = '\0';
		MU_ASSERT_STR("abcde", buf);
	}

	char buf[1];
	MU_ASSERT(spsc_read(&sub, buf, 1) == 0);

	// map publisher again
	if (spsc_create_pub(&pub, ring_name, 100)) return "Failed to create publisher";

	// write again, expect success
	MU_ASSERT(spsc_write(&pub, "abcde", 5) == 5);

	// read again
	char buf2[6];
	MU_ASSERT(spsc_read(&sub, buf2, 5) == 5);
	buf2[5] = '\0';
	MU_ASSERT_STR("abcde", buf2);
	return 0;
}

char* test_sub_empty()
{
	spsc_ring sub;
	char ring_name[] = "/tmp/ringXXXXXX";
	if (mkstemp(ring_name) == -1)
	{
		perror("mkstemp");
		return "Failed to create ring";
	}

	if (spsc_create_sub(&sub, ring_name, 100)) return "Failed to create subscriber";
	char buf[1];
	MU_ASSERT(spsc_read(&sub, buf, 1) == 0);
	return 0;
}

char* test_ring_permissions()
{
	spsc_ring pub;
	spsc_ring sub;

	char ring_name[] = "/tmp/ringXXXXXX";
	if (mkstemp(ring_name) == -1)
	{
		perror("mkstemp");
		return "Failed to create ring";
	}

	if (spsc_create_pub(&pub, ring_name, 100)) return "Failed to create publisher";
	if (spsc_create_sub(&sub, ring_name, 100)) return "Failed to create subscriber";

	MU_ASSERT(spsc_write(&sub, "abcd", 4) == 0);

	char buf[1];
	MU_ASSERT(spsc_read(&pub, buf, 1) == 0);
	return 0;
}

typedef struct test_msg
{
	int id;
	char text[4];
} test_msg;

#define NUM_MESSAGES 10000
#define BUFFER_SIZE 1024 * 1024

void do_publish(const char* ring_name)
{
	spsc_ring ring;
	if (spsc_create_pub(&ring, ring_name, BUFFER_SIZE))
	{
		puts("Failed to create publisher");
		return;
	}

	test_msg msg = { 0, "abc" };
	
	while (msg.id < NUM_MESSAGES)
	{
		if (spsc_write(&ring, (char*) &msg, sizeof(msg)) > 0)
		{
			msg.id++;
		}
		else
		{
			usleep(1000);
		}
	}
}

char* do_subscribe(const char* ring_name)
{
	spsc_ring ring;
	if (spsc_create_sub(&ring, ring_name, BUFFER_SIZE))
	{
		return "Failed to create subscriber";
	}

	int expected_id = 0;
	char buf[sizeof(test_msg)];
	while (1)
	{
		size_t len = spsc_read(&ring, buf, sizeof(test_msg));
		if (len > 0)
		{
			test_msg* msg = (test_msg*) buf;
			MU_ASSERT(msg->id == expected_id);
			MU_ASSERT_STR("abc", msg->text);
			if (++expected_id == NUM_MESSAGES) break;
		}
	}

	return 0;
}


char* test_pub_sub_multiprocess()
{
	char ring_name[] = "/tmp/ringXXXXXX";
	if (mkstemp(ring_name) == -1)
	{
		perror("mkstemp");
		return "Failed to create ring";
	}

	int pid = fork();
	if (pid == -1)
	{
		perror("fork");
		return "Failed to fork process";
	}
	else if (pid == 0)
	{
		// child process
		sleep(1);
		do_publish(ring_name);
		exit(0);
	}
	else
	{
		return do_subscribe(ring_name);
	}
	return 0;
}

char* test_all()
{
	MU_RUN_TEST(test_pub_sub);
	MU_RUN_TEST(test_sub_empty);
	MU_RUN_TEST(test_ring_permissions);
	MU_RUN_TEST(test_pub_sub_multiprocess);
	return 0;
}

int main(int argc, char** argv)
{
	char *result = test_all();
	if (result != 0) printf("%s\n", result);
	else printf("ALL TESTS PASSED\n");
	printf("Tests run: %d\n", MU_TESTS_RUN);
	return result != 0;
}
