MAKE = make

default: build

build:
	$(MAKE) -C src build

clean:
	$(MAKE) -C src clean