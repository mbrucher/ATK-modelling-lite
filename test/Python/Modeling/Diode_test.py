#!/usr/bin/env python2
# -*- coding: utf-8 -*-

from numpy.testing import assert_almost_equal

from ATK.Modeling import *

def SimpleBridgeResistorDiode_test():
    model = Modeler(1, 2, 0)

    model.add_component(Voltage(0), [('S', 0)])
    model.add_component(Voltage(5), [('S', 1)])
    model.add_component(Resistor(100), [('S', 0), ('D', 0)])
    model.add_component(Diode(), [('D', 0), ('S', 1)])

    model.dt = 1.e-3
    model.setup()

    print(model.state)
    assert_almost_equal(model.dynamic_state, [5./3])
