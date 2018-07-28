#!/usr/bin/env python

from ltspicereader import LTSpiceRawRead
from matplotlib import pyplot as plt
import numpy as np

raw = LTSpiceRawRead("moog.raw")
print(raw.get_trace_names())

x = raw.get_trace('time')
x.data = np.abs(x.data)

y = raw.get_trace('V(in+)')
plt.plot(x.data, y.data, label="Vin")

y = raw.get_trace('V(out)')
plt.plot(x.data, y.data, label="Vout")

plt.legend()
plt.show()
