#!/usr/bin/env python2
# -*- coding: utf-8 -*-

from numpy.testing import assert_almost_equal
import math

from ATK.Modeling import *

dt = 1e-3
R = 1e3
C = 1e-3

def spice_1_test():
    f = """VS	1	0	AC 0
R1	1	2	1K
C1	2	0	1mF"""
    netlist = [line.strip().split() for line in f.splitlines()]
    
    spice_model = SpiceModel()
    spice_model.parse(netlist)
    model = spice_model.create()

    model.dt = dt
    model.setup()

    assert_almost_equal(model.dynamic_state, [0])
    model.input_state[:] = (1,)

    for i in range(1000):
        model(None)
        assert_almost_equal(model.dynamic_state[0], 1 - math.exp(-i * dt / (R * C)), 1e-4)
