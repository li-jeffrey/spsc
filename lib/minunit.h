#pragma once

#include <string.h>

#define MU_ASSERT(test) do { if (!(test)) return "error: " #test " is false"; } while (0)
#define MU_ASSERT_STR(expected, actual) do { if (strcmp(expected, actual)) { \
		printf("expected:\n%s\n\n", expected); \
		printf("actual:\n%s\n\n", actual); \
		return "string mismatch"; }} while (0)
#define MU_RUN_TEST(test) do { char *message = test(); MU_TESTS_RUN++; \
				if (message) return message; } while (0)
extern int MU_TESTS_RUN;
