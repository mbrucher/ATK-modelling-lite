#!/usr/bin/env python2

from __future__ import division

import numpy as np
from matplotlib import pyplot as plt

import sys

sys.path.append("/Users/Matthieu/src/AudioTK-Modeling/Python/proto")

from ATK.Modelling import create_from_netlist

length = 10000
init = 10
dt = 1 / 100000
t = np.arange(length) * dt

x = np.sin(2 * np.pi * 50 * t)
y = []

model = create_from_netlist("moog.cir")
model.dt = dt

static_state = np.array(model.static_state)
for i in range(init):
    model.static_state[:] = static_state * (i+1) / init
    model.setup()

for v in x:
    d = model((v))
    y.append(np.copy(d))
    

plt.plot(t, x)
plt.plot(t, y)
plt.show()
