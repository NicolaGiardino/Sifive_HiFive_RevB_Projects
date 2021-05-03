# Copyright 2019 SiFive, Inc #
# SPDX-License-Identifier: Apache-2.0 #

PROGRAM ?= main

$(PROGRAM): $(wildcard *.c) $(wildcard *.h) $(wildcard *.S) $(wildcard/include *.h) $(wildcard/src *.c) $(wildcard/include/devices *.h) $(wildcard/src/devices *.c)

#$(PROGRAM): $(shell find . -name "*.c") $(shell find . -name "*.h") $(shell find . -name "*.S")

clean:
	rm -f $(PROGRAM) $(PROGRAM).hex

