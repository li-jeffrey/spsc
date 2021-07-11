APP_NAME=publisher
APP_OBJ=publisher.o

LDFLAGS=-L../../lib -lspsc -lm
CFLAGS=-Wpedantic -Wall -O3 -I../../lib 

all: $(APP_NAME)

.PHONY: all

clean:
	rm -f *.o $(APP_NAME)
	rm -f $(APP_OBJ:%.o=%.d)

.PHONY: clean

$(APP_NAME): $(APP_OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

-include $(APP_OBJ:%.o=%.d)
	
%.o: %.c
	$(CC) $(CFLAGS) -MMD -o $@ -c $<
