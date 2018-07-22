#!/usr/bin/env python2

from __future__ import division

import numpy as np
from matplotlib import pyplot as plt

import sys

sys.path.append("/Users/Matthieu/src/AudioTK-Modeling/Python/proto")

from ATK.Modelling import create_from_netlist

length = 10000
dt = 1 / 100000
t = np.arange(length) * dt

x = np.sin(2 * np.pi * 50 * t) + 2
y = []

model = create_from_netlist("DS1-input.cir")
model.dt = dt
model.input_state[:] = 2
model.setup()
print(model.dynamic_state)
for v in x:
    d = model((v))
    y.append(d)
    

plt.plot(t, x-2)
plt.plot(t, y)
plt.show()
