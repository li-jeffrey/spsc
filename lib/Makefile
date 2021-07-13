LIB_NAME=libspsc.a
LIB_OBJ=spsc.o

PERF_APP_NAME=spsc_perf
PERF_APP_OBJ=spsc_perf.o

TEST_APP_NAME=spsc_test
TEST_APP_OBJ=spsc_test.o

CFLAGS=-Wpedantic -Wall -O3

TEST_LDFLAGS=-L./ -lspsc -lm
PERF_LDFLAGS=-L./ -lpthread -lspsc -lm

all: $(LIB_NAME)

.PHONY: all

perf: $(PERF_APP_NAME)

.PHONY: perf

test: $(TEST_APP_NAME)

.PHONY: test

clean:
	rm -f *.o $(LIB_NAME) $(PERF_APP_NAME) $(TEST_APP_NAME)
	rm -f $(LIB_OBJ:%.o=%.d) $(PERF_APP_OBJ:%.o=%.d) $(TEST_APP_OBJ:%.o=%.d)

.PHONY: clean

$(LIB_NAME): $(LIB_OBJ)
	ar $(ARFLAGS) $@ $^

$(PERF_APP_NAME): $(PERF_APP_OBJ) $(LIB_NAME)
	$(CC) -o $@ $^ $(PERF_LDFLAGS)

$(TEST_APP_NAME): $(TEST_APP_OBJ) $(LIB_NAME)
	$(CC) -o $@ $^ $(TEST_LDFLAGS)

-include $(LIB_OBJ:%.o=%.d) $(TEST_APP_OBJ:%.o=%.d)
	
%.o: %.c 
	$(CC) $(CFLAGS) -MMD -o $@ -c $<
