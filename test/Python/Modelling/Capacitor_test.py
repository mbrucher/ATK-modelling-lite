#!/usr/bin/env python2
# -*- coding: utf-8 -*-

from numpy.testing import assert_almost_equal
import math

from ATK.Modelling import *

R = 1e3
C = 1e-3
dt = 1e-3

def RC_test():
    model = Modeler(1, 1, 1)

    model.add_component(Resistor(R), [('I', 0), ('D', 0)])
    model.add_component(Capacitor(C), [('D', 0), ('S', 0)])
    model.static_state[:] = (0,)

    model.dt = dt
    model.setup()

    assert_almost_equal(model.dynamic_state, [0])

    for i in range(1000):
        model((1,))
        assert_almost_equal(model.dynamic_state[0], 1 - math.exp(-(i+1) * dt / (R * C)), 4)
