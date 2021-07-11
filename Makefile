default: all

.DEFAULT:
	$(MAKE) -C lib/ $@
	$(MAKE) -C examples/subscriber $@
	$(MAKE) -C examples/publisher $@

test:
	$(MAKE) -C lib/ $@

.PHONY: test
