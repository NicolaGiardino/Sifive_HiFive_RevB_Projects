# Copyright 2019 SiFive, Inc #
# SPDX-License-Identifier: Apache-2.0 #

PROGRAM ?= main

$(PROGRAM): $(wildcard *.c) $(wildcard *.h) $(wildcard *.S) $(wildcard/include *.h) $(wildcard/src *.c) $(wildcard/include/devices *.h) $(wildcard/src/devices *.c)

clean:
	rm -f $(PROGRAM) $(PROGRAM).hex

