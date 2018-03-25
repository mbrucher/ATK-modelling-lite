#!/usr/bin/env python2
# -*- coding: utf-8 -*-

from numpy.testing import assert_almost_equal

from ATK.Modeling import *

def TransistorNPN_test():
    model = Modeler(5)

    model.add_component(Voltage(0), [0])
    Vcc = Voltage(5)
    model.add_component(Vcc, [1])
    model.add_component(Resistor(1470), [0, 2])
    model.add_component(Resistor(16670), [2, 1])

    model.add_component(Resistor(1000), [1, 3])
    model.add_component(Resistor(100), [0, 4])
    model.add_component(TransistorNPN(), [2, 3, 4])

    model.setup()
    
    model.dt = 1.e-3

    assert_almost_equal(model.state, [0, 5, .4051047, 4.9942842, .0005773])
