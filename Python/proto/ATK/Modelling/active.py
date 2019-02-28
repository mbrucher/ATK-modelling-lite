#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

import numpy as np
import math

from .modeling import retrieve_voltage

class TransistorNPN(object):
    """
    Class that implements a NPN transistor between 3 pins, BCE
    """
    nb_pins = 3
    
    def __init__(self, Is=1e-12, Vt=26e-3, Br=1, Bf=100, Ne=1):
        self.Is = Is
        self.Vt = Vt * Ne
        self.Br = Br
        self.Bf = Bf

    def __repr__(self):
        return "Transistor NPN (%f,%f,%f,%f) between pins (%s,%s,%s)" % (self.Is, self.Vt, self.Br, self.Bf, self.pins[0], self.pins[1], self.pins[2])
    
    def update_model(self, model):
        pass

    def update_steady_state(self, state, dt):
        pass

    def update_state(self, state):
        pass

    def ib(self, expVbe, expVbc):
        return self.Is * ((expVbe - 1) / self.Bf + (expVbc - 1) / self.Br)

    def ib_vbe(self, expVbe, expVbc):
        return self.Is * expVbe / self.Vt / self.Bf

    def ib_vbc(self, expVbe, expVbc):
        return self.Is * expVbc / self.Vt / self.Br

    def ic(self, expVbe, expVbc):
        return self.Is * ((expVbe - expVbc) - (expVbc - 1) / self.Br)

    def ic_vbe(self, expVbe, expVbc):
        return self.Is * expVbe / self.Vt

    def ic_vbc(self, expVbe, expVbc):
        return self.Is * (-expVbc - expVbc / self.Br) / self.Vt

    def get_current(self, pin_index, state, steady_state):
        Vbe = retrieve_voltage(state, self.pins[0]) - retrieve_voltage(state, self.pins[2])
        Vbc = retrieve_voltage(state, self.pins[0]) - retrieve_voltage(state, self.pins[1])
        expVbe = math.exp(Vbe / self.Vt)
        expVbc = math.exp(Vbc / self.Vt)
        if pin_index == 0:
            return -self.ib(expVbe, expVbc)
        elif pin_index == 1:
            return -self.ic(expVbe, expVbc)
        return self.ib(expVbe, expVbc) + self.ic(expVbe, expVbc)

    def get_gradient(self, pin_index_ref, pin_index, state, steady_state):
        Vbe = retrieve_voltage(state, self.pins[0]) - retrieve_voltage(state, self.pins[2])
        Vbc = retrieve_voltage(state, self.pins[0]) - retrieve_voltage(state, self.pins[1])
        expVbe = math.exp(Vbe / self.Vt)
        expVbc = math.exp(Vbc / self.Vt)
        if pin_index_ref == 0 and pin_index == 0:
            return -(self.ib_vbc(expVbe, expVbc) + self.ib_vbe(expVbe, expVbc))
        elif pin_index_ref == 0 and pin_index == 1:
            return self.ib_vbc(expVbe, expVbc)
        elif pin_index_ref == 0 and pin_index == 2:
            return self.ib_vbe(expVbe, expVbc)
        elif pin_index_ref == 1 and pin_index == 0:
            return -(self.ic_vbc(expVbe, expVbc) + self.ic_vbe(expVbe, expVbc))
        elif pin_index_ref == 1 and pin_index == 1:
            return self.ic_vbc(expVbe, expVbc)
        elif pin_index_ref == 1 and pin_index == 2:
            return self.ic_vbe(expVbe, expVbc)
        elif pin_index_ref == 2 and pin_index == 0:
            return (self.ib_vbe(expVbe, expVbc) + self.ib_vbc(expVbe, expVbc) + self.ic_vbe(expVbe, expVbc) + self.ic_vbc(expVbe, expVbc))
        elif pin_index_ref == 2 and pin_index == 1:
            return -(self.ib_vbc(expVbe, expVbc) + self.ic_vbc(expVbe, expVbc))
        elif pin_index_ref == 2 and pin_index == 2:
            return -(self.ib_vbe(expVbe, expVbc) + self.ic_vbe(expVbe, expVbc))


class TransistorPNP(object):
    """
    Class that implements a PNP transistor between 3 pins, BCE
    """
    nb_pins = 3
    
    def __init__(self, Is=1e-12, Vt=26e-3, Br=1, Bf=100):
        self.Is = Is
        self.Vt = Vt
        self.Br = Br
        self.Bf = Bf

    def __repr__(self):
        return "Transistor PNP (%f,%f,%f,%f) between pins (%s,%s,%s)" % (self.Is, self.Vt, self.Br, self.Bf, self.pins[0], self.pins[1], self.pins[2])
    
    def update_model(self, model):
        pass

    def update_steady_state(self, state, dt):
        pass

    def update_state(self, state):
        pass

    def ib(self, expVbe, expVbc):
        return self.Is * ((expVbe - 1) / self.Bf + (expVbc - 1) / self.Br)

    def ib_vbe(self, expVbe, expVbc):
        return self.Is * expVbe / self.Vt / self.Bf

    def ib_vbc(self, expVbe, expVbc):
        return self.Is * expVbc / self.Vt / self.Br

    def ic(self, expVbe, expVbc):
        return self.Is * ((expVbe - expVbc) - (expVbc - 1) / self.Br)

    def ic_vbe(self, expVbe, expVbc):
        return self.Is * expVbe / self.Vt

    def ic_vbc(self, expVbe, expVbc):
        return self.Is * (-expVbc - expVbc / self.Br) / self.Vt

    def get_current(self, pin_index, state, steady_state):
        Vbe = retrieve_voltage(state, self.pins[0]) - retrieve_voltage(state, self.pins[2])
        Vbc = retrieve_voltage(state, self.pins[0]) - retrieve_voltage(state, self.pins[1])
        expVbe = math.exp(-Vbe / self.Vt)
        expVbc = math.exp(-Vbc / self.Vt)
        if pin_index == 0:
            return self.ib(expVbe, expVbc)
        elif pin_index == 1:
            return self.ic(expVbe, expVbc)
        return -self.ib(expVbe, expVbc) - self.ic(expVbe, expVbc)

    def get_gradient(self, pin_index_ref, pin_index, state, steady_state):
        Vbe = retrieve_voltage(state, self.pins[0]) - retrieve_voltage(state, self.pins[2])
        Vbc = retrieve_voltage(state, self.pins[0]) - retrieve_voltage(state, self.pins[1])
        expVbe = math.exp(-Vbe / self.Vt)
        expVbc = math.exp(-Vbc / self.Vt)
        if pin_index_ref == 0 and pin_index == 0:
            return -(self.ib_vbc(expVbe, expVbc) + self.ib_vbe(expVbe, expVbc))
        elif pin_index_ref == 0 and pin_index == 1:
            return self.ib_vbc(expVbe, expVbc)
        elif pin_index_ref == 0 and pin_index == 2:
            return self.ib_vbe(expVbe, expVbc)
        elif pin_index_ref == 1 and pin_index == 0:
            return -(self.ic_vbc(expVbe, expVbc) + self.ic_vbe(expVbe, expVbc))
        elif pin_index_ref == 1 and pin_index == 1:
            return self.ic_vbc(expVbe, expVbc)
        elif pin_index_ref == 1 and pin_index == 2:
            return self.ic_vbe(expVbe, expVbc)
        elif pin_index_ref == 2 and pin_index == 0:
            return (self.ib_vbe(expVbe, expVbc) + self.ib_vbc(expVbe, expVbc) + self.ic_vbe(expVbe, expVbc) + self.ic_vbc(expVbe, expVbc))
        elif pin_index_ref == 2 and pin_index == 1:
            return -(self.ib_vbc(expVbe, expVbc) + self.ic_vbc(expVbe, expVbc))
        elif pin_index_ref == 2 and pin_index == 2:
            return -(self.ib_vbe(expVbe, expVbc) + self.ic_vbe(expVbe, expVbc))

class OpAmp(object):
    """
    Class that implements a OpAmp between 3 pins, V-, V+, Vo
    """
    nb_pins = 3
    
    def __init__(self):
        pass

    def __repr__(self):
        return "OpAmp between pins (%s,%s,%s) overriding equation at pin %s" % (self.pins[0], self.pins[1], self.pins[2], self.pins[2])
    
    def update_model(self, model):
        assert self.pins[2][0] == "D"
        model.dynamic_pins_equation[self.pins[2][1]] = (self, 0)

    def update_steady_state(self, state, dt):
        pass

    def update_state(self, state):
        pass

    def get_current(self, pin_index, state, steady_state):
        return 0

    def get_gradient(self, pin_index_ref, pin_index, state, steady_state):
        return 0

    def add_equation(self, state, steady_state, eq_number):
        eq = retrieve_voltage(state, self.pins[0]) - retrieve_voltage(state, self.pins[1])
        jac = np.zeros(len(state["D"]))
        if self.pins[0][0] == "D":
            jac[self.pins[0][1]] = 1
        if self.pins[1][0] == "D":
            jac[self.pins[1][1]] = -1
        return eq, jac

class VoltageGain(object):
    """
    Class that implements a voltage gain between 4 pins, Vi+, Vi-, Vo+, Vo-
    """
    nb_pins = 4
    
    def __init__(self, gain):
        self.gain = gain

    def __repr__(self):
        return "Voltage gain between pins (%s,%s,%s,%s) overriding equation at pin %s" % (self.pins[0], self.pins[1], self.pins[2], self.pins[3], self.pins[2])
    
    def update_model(self, model):
        assert self.pins[2][0] == "D"
        model.dynamic_pins_equation[self.pins[2][1]] = (self, 0)

    def update_steady_state(self, state, dt):
        pass

    def update_state(self, state):
        pass

    def get_current(self, pin_index, state, steady_state):
        return 0

    def get_gradient(self, pin_index_ref, pin_index, state, steady_state):
        return 0

    def add_equation(self, state, steady_state, eq_number):
        eq = self.gain * (retrieve_voltage(state, self.pins[0]) - retrieve_voltage(state, self.pins[1])) - (retrieve_voltage(state, self.pins[2]) - retrieve_voltage(state, self.pins[3]))
        jac = np.zeros(len(state["D"]))
        if self.pins[0][0] == "D":
            jac[self.pins[0][1]] = self.gain
        if self.pins[1][0] == "D":
            jac[self.pins[1][1]] = -self.gain
        if self.pins[2][0] == "D":
            jac[self.pins[2][1]] = -1
        if self.pins[3][0] == "D":
            jac[self.pins[3][1]] = 1
        return eq, jac

class Current(object):
    """
    Class that implements a perfect current generator between two pins
    """
    nb_pins = 2
    
    def __init__(self, current):
        self.current = current

    def __repr__(self):
        return "%02.2eA between pins (%s,%s)" % (self.current, self.pins[0], self.pins[1])

    def update_model(self, model):
        pass

    def update_steady_state(self, state, dt):
        pass

    def update_state(self, state):
        pass

    def get_current(self, pin_index, state, steady_state):
        return self.current * (-1 if 0 == pin_index else 1)

    def get_gradient(self, pin_index_ref, pin_index, state, steady_state):
        return 0
