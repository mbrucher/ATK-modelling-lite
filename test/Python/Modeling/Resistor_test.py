#!/usr/bin/env python2
# -*- coding: utf-8 -*-

from numpy.testing import assert_almost_equal

from ATK.Modeling import *

def SimpleBridgeResistor_test():
    model = Modeler(3)

    model.add_component(Voltage(0), [0])
    model.add_component(Voltage(5), [1])
    model.add_component(Resistor(100), [0, 2])
    model.add_component(Resistor(200), [2, 1])

    model.setup()
    model.dt = 1.e-3

    assert_almost_equal(model.state, [0, 5, 5./3])

def ParallelBridgeResistor_test():
    model = Modeler(3)

    model.add_component(Voltage(0), [0])
    model.add_component(Voltage(5), [1])
    model.add_component(Resistor(100), [0, 2])
    model.add_component(Resistor(400), [1, 2])
    model.add_component(Resistor(400), [2, 1])

    model.setup()
    model.dt = 1.e-3

    assert_almost_equal(model.state, [0, 5, 5./3])

def SerialBridgeResistor_test():
    model = Modeler(4)

    model.add_component(Voltage(0), [0])
    model.add_component(Voltage(5), [1])
    model.add_component(Resistor(100), [0, 2])
    model.add_component(Resistor(100), [1, 3])
    model.add_component(Resistor(100), [3, 2])

    model.setup()
    model.dt = 1.e-3

    assert_almost_equal(model.state, [0, 5, 5./3, 10./3])

def ImportBridgeResistor_test():
    model = Modeler(4)

    model.add_component(Voltage(0), [0])
    model.add_component(Voltage(5), [1])
    model.add_component(Resistor(100), [0, 2])
    model.add_component(Resistor(100), [1, 2])
    model.add_component(Resistor(100), [3, 2])
    
    model.setup()
    model.dt = 1.e-3

    assert_almost_equal(model.state, [0, 5, 5./2, 5./2])

    model.add_component(Input(0), [3])
    
    model.setup(False)
    # Changing input value should not change the inner variable state even after calling setup.
    # state[3] changes because itùs the new input voltage
    assert_almost_equal(model.state, [0, 5, 5./2, 0])
    
    assert_almost_equal(model({3:0})[2], 5./3)
    
