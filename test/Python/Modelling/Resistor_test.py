#!/usr/bin/env python2
# -*- coding: utf-8 -*-

from numpy.testing import assert_almost_equal

from ATK.Modelling import *

def SimpleBridgeResistor_test():
    model = Modeler(1, 2, 0)

    model.add_component(Resistor(100), [('S', 0), ('D', 0)])
    model.add_component(Resistor(200), [('D', 0), ('S', 1)])
    model.static_state[:] = (0, 5)

    model.dt = 1.e-3
    model.setup()

    assert_almost_equal(model.dynamic_state, [5./3])

def ParallelBridgeResistor_test():
    model = Modeler(1, 2, 0)

    model.add_component(Resistor(100), [('S', 0), ('D', 0)])
    model.add_component(Resistor(400), [('S', 1), ('D', 0)])
    model.add_component(Resistor(400), [('D', 0), ('S', 1)])
    model.static_state[:] = (0, 5)

    model.dt = 1.e-3
    model.setup()

    assert_almost_equal(model.dynamic_state, [5./3])

def SerialBridgeResistor_test():
    model = Modeler(2, 2, 0)

    model.add_component(Resistor(100), [('S', 0), ('D', 0)])
    model.add_component(Resistor(100), [('S', 1), ('D', 1)])
    model.add_component(Resistor(100), [('D', 1), ('D', 0)])
    model.static_state[:] = (0, 5)

    model.dt = 1.e-3
    model.setup()

    assert_almost_equal(model.dynamic_state, [5./3, 10./3])

def ImportBridgeResistor_test():
    model = Modeler(1, 2, 1)

    model.add_component(Resistor(100), [('S', 0), ('D', 0)])
    model.add_component(Resistor(100), [('S', 1), ('D', 0)])
    model.add_component(Resistor(100), [('I', 0), ('D', 0)])
    model.static_state[:] = (0, 5)
    
    model.dt = 1.e-3
    model.setup()

    # Input is considered to be grounded -> static state = 0
    assert_almost_equal(model.dynamic_state, [5./3])
    
    model.setup()
    # Changing input value should not change the inner variable state even after calling setup.
    # state[3] changes because itùs the new input voltage
    assert_almost_equal(model.dynamic_state, [5./3])
    
    assert_almost_equal(model([5./2])[0], 5./2)
    
