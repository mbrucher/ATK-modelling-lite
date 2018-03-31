#!/usr/bin/env python2
# -*- coding: utf-8 -*-

from numpy.testing import assert_almost_equal

from ATK.Modeling import *

dt = 1e-3
R = 1e3

def FollowerOpAmp_test():
    model = Modeler(1, 2, 0)

    model.add_component(Voltage(0), [('S', 0)])
    model.add_component(Voltage(5), [('S', 1)])
    model.add_component(OpAmp(), [('D', 0), ('S', 1), ('D', 0)])

    model.dt = dt
    model.setup()

    assert_almost_equal(model.dynamic_state, [5])

def NonInvertingOpAmp_test():
    model = Modeler(2, 2, 0)

    model.add_component(Voltage(0), [('S', 0)])
    model.add_component(Voltage(5), [('S', 1)])
    model.add_component(OpAmp(), [('D', 0), ('S', 1), ('D', 1)])
    model.add_component(Resistor(R), [('S', 0), ('D', 0)])
    model.add_component(Resistor(R), [('D', 1), ('D', 0)])

    model.dt = dt
    model.setup()

    print(model)
    assert_almost_equal(model.dynamic_state, [5, 10])
