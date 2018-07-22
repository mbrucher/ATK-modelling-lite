#!/usr/bin/env python

from ltspicereader import LTSpiceRawRead
from matplotlib import pyplot as plt
import numpy as np

state = {}

raw = LTSpiceRawRead("moog.op.raw")
for name in raw.get_trace_names():
    print(name)
    state[name] = raw.get_trace(str(name)).data[0]

print state
