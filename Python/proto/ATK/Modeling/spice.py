#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

def scale(suffix):
    return 1

def create(filename):
    f = open(filename)
    
    netlist = []
    
    print("Analyzing netlist: %s" % f.readline().strip())
    
    for line in f:
        line = line.strip()
        if len(line) == 0 or line[0] == '*':
            continue
        if line[0] == '+':
            netlist[-1].extend(line.split())
        else:
            netlist.append(line.split())

    print(netlist)

if __name__ == "__main__":
    import sys
    
    model = create(sys.argv[1])
