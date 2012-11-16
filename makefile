# Bootstraping eclipse makefile 

all: 
	$(MAKE) -C release all

clean: 
	$(MAKE) -C release clean

.PHONY: release