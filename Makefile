default: all

.DEFAULT:
	$(MAKE) -C lib/ $@

clean:
	$(MAKE) -C lib/ $@
	$(MAKE) -C examples/subscriber $@
	$(MAKE) -C examples/publisher $@

.PHONY: clean

examples:
	$(MAKE) -C examples/subscriber all 
	$(MAKE) -C examples/publisher all

.PHONY: examples 
