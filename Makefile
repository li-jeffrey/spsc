default: all

.DEFAULT:
	$(MAKE) -C lib/ $@

clean:
	$(MAKE) -C lib/ $@
	$(MAKE) -C examples/ $@

.PHONY: clean

examples:
	$(MAKE) -C examples/ $@

.PHONY: examples 
