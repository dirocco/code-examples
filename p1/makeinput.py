#!/usr/bin/env python

import random

random.seed()

filename="input.txt"

with open(filename, "w") as file:
	for i in xrange(0,10000000):
		file.write(str(random.randint(-2000000000, 2000000000)) + '\n')


