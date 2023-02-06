MAKE = make

default: build

build:
	$(MAKE) -C src

.PHONY: test
test:
	$(MAKE) -C src
	$(MAKE) -C test

clean:
	$(MAKE) -C src clean