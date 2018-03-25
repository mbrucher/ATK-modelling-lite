#!/usr/bin/env python2
# -*- coding: utf-8 -*-

from numpy.testing import assert_almost_equal
import math

from ATK.Modeling import *

def RC_test():
    R = 1e3
    C = 1e-3
    dt = 1e-3
    
    model = Modeler(3)

    model.add_component(Voltage(0), [0])
    Vcc = Voltage(0)
    model.add_component(Vcc, [1])
    model.add_component(Resistor(R), [1, 2])

    model.dt = dt
    model.setup()
    
    assert_almost_equal(model.state, [0, 0, 0])

    model.add_component(Capacitor(C), [2, 0])
    Vcc.V = 1

    model.setup(False)

    for i in range(1000):
        print(i)
        model({})
        assert_almost_equal(model.state[2], 1 - math.exp(-i * dt / (R * C)), 1e-4)


def RC2_test():
    R = 1e3
    C = 1e-3
    dt = 1e-3
    
    model = Modeler(3)

    model.add_component(Voltage(0), [0])
    Vcc = Voltage(0)
    model.add_component(Vcc, [1])
    model.add_component(Resistor(R), [0, 2])

    model.dt = dt
    model.setup()
    
    assert_almost_equal(model.state, [0, 0, 0])

    model.add_component(Capacitor(C), [2, 1])
    Vcc.V = 1

    model.setup(False)

    for i in range(1000):
        print(i)
        model({})
        assert_almost_equal(model.state[2], math.exp(-i * dt / (R * C)), 1e-4)
