MAKE = make

default: build

build:
	$(MAKE) -C src build

tests:
	$(MAKE) -C test run

clean:
	$(MAKE) -C src clean