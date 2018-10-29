#!/usr/bin/env python

import numpy as np
from matplotlib import pyplot as plt

plt.style.use("dark_background")

from ATK.Modelling import create_from_netlist

length = 10000
init = 10
dt = 1 / 100000
t = np.arange(length) * dt

x = np.sin(2 * np.pi * 50 * t)

model = create_from_netlist("moog.cir")
model.dt = dt

static_state = np.array(model.static_state)
for i in range(init):
    model.static_state[:] = static_state * (i+1) / init
    model.setup()

def generate(x):
    y = []
    for v in x:
        d = model((v))
        y.append(np.copy(d))
    return y
y = generate(x)

plt.plot(t, x)
plt.plot(t, y)
plt.title("Moog filter in pure Python")
plt.show()

"""
  SPICE model with (7,11,1) pins:
  gnd mapped to ('S', 0)
  0 mapped to ('S', 0)
  v+ mapped to ('S', 1)
  ref mapped to ('S', 2)
  s1b mapped to ('S', 3)
  s2b mapped to ('S', 4)
  s3b mapped to ('S', 5)
  s4b mapped to ('S', 6)
  in+ mapped to ('I', 0)
  s2+ mapped to ('D', 0)
  s4- mapped to ('D', 1)
  lt mapped to ('D', 2)
  s1- mapped to ('D', 3)
  in- mapped to ('D', 4)
  out mapped to ('D', 5)
  s2- mapped to ('D', 6)
  s4+ mapped to ('D', 7)
  s3- mapped to ('D', 8)
  s1+ mapped to ('D', 9)
  s3+ mapped to ('D', 10)
  Components:
  Transistor NPN (0.000000,0.032734,0.737400,416.400000) between pins (('I', 0),('D', 3),('D', 2))
  Transistor NPN (0.000000,0.032734,0.737400,416.400000) between pins (('D', 4),('D', 9),('D', 2))
  4.70e-08F between pins (('D', 3),('D', 9))
  Transistor NPN (0.000000,0.032734,0.737400,416.400000) between pins (('S', 3),('D', 6),('D', 3))
  Transistor NPN (0.000000,0.032734,0.737400,416.400000) between pins (('S', 3),('D', 0),('D', 9))
  4.70e-08F between pins (('D', 6),('D', 0))
  Transistor NPN (0.000000,0.032734,0.737400,416.400000) between pins (('S', 4),('D', 8),('D', 6))
  Transistor NPN (0.000000,0.032734,0.737400,416.400000) between pins (('S', 4),('D', 10),('D', 0))
  4.70e-08F between pins (('D', 8),('D', 10))
  Transistor NPN (0.000000,0.032734,0.737400,416.400000) between pins (('S', 5),('D', 1),('D', 8))
  Transistor NPN (0.000000,0.032734,0.737400,416.400000) between pins (('S', 5),('D', 7),('D', 10))
  4.70e-08F between pins (('D', 1),('D', 7))
  Transistor NPN (0.000000,0.032734,0.737400,416.400000) between pins (('S', 6),('S', 1),('D', 1))
  Transistor NPN (0.000000,0.032734,0.737400,416.400000) between pins (('S', 6),('S', 1),('D', 7))
  Voltage gain between pins (('D', 7),('D', 1),('D', 5),('S', 0)) overriding equation at pin ('D', 5)
  1.00e+00F between pins (('D', 5),('D', 4))
  3.30e+03ohms between pins (('D', 4),('S', 0))
  1.00e-05A between pins (('D', 2),('S', 0))
  Models:
  ('Q2N3904', ('npn', {'Vt': 0.026, 'Is': 6.73e-15, 'Bf': 416.4, 'Br': 0.7374, 'Ne': 1.259}))
  """
