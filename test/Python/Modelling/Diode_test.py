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

dt = 1. / 48000
R = 10000
C = 22e-9

def toto_test():
    import numpy as np
    
    model = Modeler(2, 1, 1)
    
    model.add_component(Resistor(R), [('D', 1), ('I', 0)])
    model.add_component(Capacitor(C), [('D', 0), ('D', 1)])
    model.add_component(AntiParallelDiode(Is=1e-12), [('D', 0), ('S', 0)])
    
    model.dt = dt
    model.setup()
    
    for i in range(10):
        model((2 * np.pi * i * dt * 1000,))
        
