#include <stdlib.h>

#include "minunit.h"
#include "spsc.h"

int MU_TESTS_RUN = 0;


char* test_pub_sub()
{
	spsc_ring pub;
	spsc_ring sub;

	char test_ring[] = "/tmp/ringXXXXXX";
	if (mkstemp(test_ring) == -1)
	{
		perror("mkstemp");
		return "Failed to create ring";
	}

	if (spsc_create_pub(&pub, test_ring, 100)) return "Failed to create publisher";
	if (spsc_create_sub(&sub, test_ring, 100)) return "Failed to create subscriber";
	MU_ASSERT(pub._data->_size == 128);

	// write buffer until full
	for (int i = 0; i < 16; i++)
	{
		MU_ASSERT(spsc_write(&pub, "abcd", 4) == 4);
	}
	MU_ASSERT(spsc_write(&pub, "abcd", 4) == 0);

	// unmap publisher
	spsc_destroy(&pub);

	// read from buffer
	for (int i = 0; i < 16; i++)
	{
		char buf[5];
		MU_ASSERT(spsc_read(&sub, buf, 4) == 4);
		buf[4] = '\0';
		MU_ASSERT_STR("abcd", buf);
	}

	char buf[1];
	MU_ASSERT(spsc_read(&sub, buf, 1) == 0);

	// map publisher again
	if (spsc_create_pub(&pub, test_ring, 100)) return "Failed to create publisher";

	// write again, expect success
	MU_ASSERT(spsc_write(&pub, "abcd", 4) == 4);
	return 0;
}

char* test_sub_empty()
{
	spsc_ring sub;
	char test_ring[] = "/tmp/ringXXXXXX";
	if (mkstemp(test_ring) == -1)
	{
		perror("mkstemp");
		return "Failed to create ring";
	}

	if (spsc_create_sub(&sub, test_ring, 100)) return "Failed to create subscriber";
	char buf[1];
	MU_ASSERT(spsc_read(&sub, buf, 1) == 0);
	return 0;
}

char* test_ring_permissions()
{
	spsc_ring pub;
	spsc_ring sub;

	char test_ring[] = "/tmp/ringXXXXXX";
	if (mkstemp(test_ring) == -1)
	{
		perror("mkstemp");
		return "Failed to create ring";
	}

	if (spsc_create_pub(&pub, test_ring, 100)) return "Failed to create publisher";
	if (spsc_create_sub(&sub, test_ring, 100)) return "Failed to create subscriber";

	MU_ASSERT(spsc_write(&sub, "abcd", 4) == 0);

	char buf[1];
	MU_ASSERT(spsc_read(&pub, buf, 1) == 0);
	return 0;
}

char* test_all()
{
	MU_RUN_TEST(test_pub_sub);
	MU_RUN_TEST(test_sub_empty);
	MU_RUN_TEST(test_ring_permissions);
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
