#!/usr/bin/env python2
# -*- coding: utf-8 -*-

from numpy.testing import assert_almost_equal

def SimpleBridgeResistor_test():
    from ATK.Modeling import *
    model = Modeler(3)

    model.add_component(Voltage(0), [0])
    model.add_component(Voltage(5), [1])
    model.add_component(Resistor(100), [0, 2])
    model.add_component(Resistor(200), [2, 1])

    assert_almost_equal(model([]), [0, 5, 5./3])

def ParallelBridgeResistor_test():
    from ATK.Modeling import *
    model = Modeler(3)

    model.add_component(Voltage(0), [0])
    model.add_component(Voltage(5), [1])
    model.add_component(Resistor(100), [0, 2])
    model.add_component(Resistor(400), [1, 2])
    model.add_component(Resistor(400), [2, 1])

    assert_almost_equal(model([]), [0, 5, 5./3])


def SeriqlBridgeResistor_test():
    from ATK.Modeling import *
    model = Modeler(4)

    model.add_component(Voltage(0), [0])
    model.add_component(Voltage(5), [1])
    model.add_component(Resistor(100), [0, 2])
    model.add_component(Resistor(100), [1, 3])
    model.add_component(Resistor(100), [3, 2])

    assert_almost_equal(model([]), [0, 5, 5./3, 10./3])
