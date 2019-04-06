#!/usr/bin/python


import sys
import random

PI=3.141592653589

# python inversek2j_dataGen.py <number of items> <output file>

outFile = open(sys.argv[2], "w")

l1 = random.uniform(0.2, 0.8)
outFile.write("%f\t%f\n" % (l1, 1.0 - l1))
outFile.write("%d\n" % (int(sys.argv[1])))

for i in range(int(sys.argv[1])):
	theta1 = random.uniform(0.0, 1.0) * PI / 2.0
	theta2 = random.uniform(0.0, 1.0) * PI / 2.0
	outFile.write("%f\t%f\n" % (theta1, theta2))
pass;

outFile.close()