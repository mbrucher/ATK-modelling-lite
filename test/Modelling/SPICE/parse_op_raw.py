#!/usr/bin/env python

from ltspicereader import LTSpiceRawRead

state = {}

raw = LTSpiceRawRead("moog.op.raw")
for name in raw.get_trace_names():
    state[name] = raw.get_trace(str(name)).data[0]

print state
