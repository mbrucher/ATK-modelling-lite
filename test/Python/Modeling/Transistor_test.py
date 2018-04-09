#!/usr/bin/env python2
# -*- coding: utf-8 -*-

from numpy.testing import assert_almost_equal

from ATK.Modeling import *

def TransistorNPN_test():
    model = Modeler(3, 2, 0)

    model.add_component(Resistor(1470), [('S', 0), ('D', 0)])
    model.add_component(Resistor(16670), [('D', 0), ('S', 1)])
    model.add_component(Resistor(1000), [('S', 1), ('D', 1)])
    model.add_component(Resistor(100), [('S', 0), ('D', 2)])
    model.add_component(TransistorNPN(), [('D', 0), ('D', 1), ('D', 2)])
    model.static_state[:] = (0, 5)

    model.dt = 1.e-3
    model.setup()
    
    assert_almost_equal(model.dynamic_state, [.4051047, 4.9942842, .0005773])

def TransistorPNP_test():
    model = Modeler(3, 2, 0)

    model.add_component(Resistor(1470), [('S', 0), ('D', 0)])
    model.add_component(Resistor(16670), [('D', 0), ('S', 1)])
    model.add_component(Resistor(1000), [('S', 1), ('D', 1)])
    model.add_component(Resistor(100), [('S', 0), ('D', 2)])
    model.add_component(TransistorPNP(), [('D', 0), ('D', 1), ('D', 2)])
    model.static_state[:] = (0, -5)

    model.dt = 1.e-3
    model.setup()
    
    assert_almost_equal(model.dynamic_state, [-.4051047, -4.9942842, -.0005773])

def MatchedTransistor_test():
    model = Modeler(1, 3, 0)

    model.add_component(Resistor(200e3), [('S', 0), ('D', 0)])
    model.add_component(TransistorNPN(), [('S', 0), ('S', 1), ('D', 0)])
    model.add_component(TransistorPNP(), [('S', 0), ('S', 2), ('D', 0)])
    model.static_state[:] = (0, 2, -2)

    model.dt = 1.e-3
    model.setup()
    
    assert_almost_equal(model.dynamic_state, [0])

