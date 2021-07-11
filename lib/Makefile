LIB_NAME=libspsc.a
LIB_OBJ=spsc.o

TEST_APP_NAME=spsc_test
TEST_APP_OBJ=spsc_test.o

CFLAGS=-Wpedantic -Wall -O3

TEST_LDFLAGS=-L./ -lspsc -lm

all: $(LIB_NAME)

.PHONY: all

test: $(TEST_APP_NAME)

.PHONY: test

clean:
	rm -f *.o $(LIB_NAME) $(TEST_APP_NAME)
	rm -f $(LIB_OBJ:%.o=%.d) $(TEST_APP_OBJ:%.o=%.d)

.PHONY: clean

$(LIB_NAME): $(LIB_OBJ)
	ar $(ARFLAGS) $@ $^

$(TEST_APP_NAME): $(TEST_APP_OBJ) $(LIB_NAME)
	$(CC) -o $@ $^ $(TEST_LDFLAGS)

-include $(LIB_OBJ:%.o=%.d) $(TEST_APP_OBJ:%.o=%.d)
	
%.o: %.c 
	$(CC) $(CFLAGS) -MMD -o $@ -c $<
