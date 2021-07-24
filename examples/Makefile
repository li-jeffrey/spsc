APP_OBJ=publisher.o subscriber.o

LDFLAGS=-L../lib -lspsc
CFLAGS=-Wpedantic -Wall -O3 -I../lib

examples: publisher subscriber

.PHONY: examples

clean:
	rm -f *.o publisher subscriber
	rm -f $(APP_OBJ:%.o=%.d)

.PHONY: clean

publisher: publisher.o
	$(CC) -o $@ $^ $(LDFLAGS)

subscriber: subscriber.o
	$(CC) -o $@ $^ $(LDFLAGS)

-include $(APP_OBJ:%.o=%.d)
	
%.o: %.c
	$(CC) $(CFLAGS) -MMD -o $@ -c $<
