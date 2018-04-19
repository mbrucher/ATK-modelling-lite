#!/usr/bin/env python2
# -*- coding: utf-8 -*-

from numpy.testing import assert_almost_equal

from ATK.Modelling import *

def SimpleBridgeResistorDiode_test():
    model = Modeler(1, 2, 0)

    model.add_component(Resistor(1000), [('D', 0), ('S', 1)])
    model.add_component(Diode(), [('D', 0), ('S', 0)])
    model.static_state[:] = (0, 5)

    model.dt = 1.e-3
    model.setup()

    assert_almost_equal(model.dynamic_state, [0.8623735])
