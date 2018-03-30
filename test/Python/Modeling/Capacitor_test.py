#!/usr/bin/env python2
# -*- coding: utf-8 -*-

from numpy.testing import assert_almost_equal
import math

from ATK.Modeling import *

R = 1e3
C = 1e-3
dt = 1e-3

def RC_test():
    model = Modeler(1, 2, 0)

    model.add_component(Voltage(0), [('S', 0)])
    Vcc = Voltage(0)
    model.add_component(Vcc, [('S', 1)])
    model.add_component(Resistor(R), [('S', 1), ('D', 0)])
    model.add_component(Capacitor(C), [('D', 0), ('S', 0)])

    model.dt = dt
    model.setup()

    assert_almost_equal(model.dynamic_state, [0])

    model.setup()
    Vcc.V = 1

    for i in range(1000):
        model(None)
        assert_almost_equal(model.dynamic_state[0], 1 - math.exp(-i * dt / (R * C)), 1e-4)


def RC2_test():
    model = Modeler(1, 2, 0)

    model.add_component(Voltage(0), [('S', 0)])
    model.add_component(Voltage(1), [('S', 1)])
    model.add_component(Resistor(R), [('S', 0), ('D', 0)])
    model.add_component(Capacitor(C), [('D', 0), ('S', 1)])

    model.dt = dt
    model.setup()
    
    assert_almost_equal(model.dynamic_state, [0])

    model.setup()

    for i in range(1000):
        model(None)
        assert_almost_equal(model.dynamic_state[0], math.exp(-i * dt / (R * C)), 1e-4)
