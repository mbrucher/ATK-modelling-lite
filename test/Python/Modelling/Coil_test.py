#!/usr/bin/env python2
# -*- coding: utf-8 -*-

from numpy.testing import assert_almost_equal
import math

from ATK.Modelling import *

R = 1e3
L = 1e-3
dt = 1e-3

def RHR_test():    
    model = Modeler(2, 2, 0)

    model.add_component(Resistor(R), [('S', 1), ('D', 0)])
    model.add_component(Coil(L), [('D', 0), ('D', 1)])
    model.add_component(Resistor(R), [('S', 0), ('D', 1)])
    model.static_state[:] = (0, 5)

    model.dt = dt
    model.setup()

    assert_almost_equal(model.dynamic_state, [2.5, 2.5])


def RH_test():    
    model = Modeler(1, 2, 0)

    model.add_component(Resistor(R), [('S', 1), ('D', 0)])
    model.add_component(Coil(L), [('D', 0), ('S', 0)])
    model.static_state[:] = (0, 1)

    model.dt = dt
    model.setup()

    assert_almost_equal(model.dynamic_state, [0])

    model.setup()

    for i in range(1000):
        model(None)
        assert_almost_equal(model.dynamic_state[0], math.exp(-i * dt / (R * L)), 1e-4)


def RH2_test():
    model = Modeler(1, 1, 1)

    model.add_component(Resistor(R), [('S', 0), ('D', 0)])
    model.add_component(Coil(L), [('D', 0), ('I', 0)])
    model.static_state[:] = (0,)

    model.dt = dt
    model.setup()
    
    assert_almost_equal(model.dynamic_state, [0])

    for i in range(1000):
        model((1,))
        assert_almost_equal(model.dynamic_state[0], 1 - math.exp(-i * dt / (R * L)), 1e-4)
