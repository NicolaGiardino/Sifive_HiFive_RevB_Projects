# Copyright 2019 SiFive, Inc #
# SPDX-License-Identifier: Apache-2.0 #

PROGRAM ?= main

$(PROGRAM): $(wildcard *.c) $(wildcard *.h) $(wildcard *.S) $(wildcard/bsp/env *.h) $(wildcard/bsp/env/freedom-e300-hifive1 *.h) $(wildcard/bsp/include/sifive *.h) $(wildcard/bsp/include/sifive/devices *.h)

clean:
	rm -f $(PROGRAM) $(PROGRAM).hex

