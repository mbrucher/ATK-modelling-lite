#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

import numpy as np
import math

from modeling import retrieve_voltage

class TransistorNPN(object):
    """
    Class that implements a NPN transistor between 3 pins, BCE
    """
    nb_pins = 3
    
    def __init__(self, Is=1e-12, Vt=26e-3, Br=1, Bf=100):
        self.Is = Is
        self.Vt = Vt
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

    def ib(self, state):
        return self.Is * ((self.expVbe - 1) / self.Bf + (self.expVbc - 1) / self.Br)

    def ib_vbe(self, state):
        return self.Is * self.expVbe / self.Vt / self.Bf

    def ib_vbc(self, state):
        return self.Is * self.expVbc / self.Vt / self.Br

    def ic(self, state):
        return self.Is * ((self.expVbe - self.expVbc) - (self.expVbc - 1) / self.Br)

    def ic_vbe(self, state):
        return self.Is * self.expVbe / self.Vt

    def ic_vbc(self, state):
        return self.Is * (-self.expVbc - self.expVbc / self.Br) / self.Vt

    def get_current(self, pin_index, state, steady_state):
        if pin_index == 0:
            return -self.ib(state)
        elif pin_index == 1:
            return -self.ic(state)
        return self.ib(state) + self.ic(state)

    def get_gradient(self, pin_index_ref, pin_index, state, steady_state):
        if pin_index_ref == 0 and pin_index == 0:
            return self.ib_vbc(state) + self.ib_vbe(state)
        elif pin_index_ref == 0 and pin_index == 1:
            return -self.ib_vbc(state)
        elif pin_index_ref == 0 and pin_index == 2:
            return -self.ib_vbe(state)
        elif pin_index_ref == 1 and pin_index == 0:
            return self.ic_vbc(state) + self.ic_vbe(state)
        elif pin_index_ref == 1 and pin_index == 1:
            return -self.ic_vbc(state)
        elif pin_index_ref == 1 and pin_index == 2:
            return -self.ic_vbe(state)
        elif pin_index_ref == 2 and pin_index == 0:
            return -(self.ib_vbe(state) + self.ib_vbc(state) + self.ic_vbe(state) + self.ic_vbc(state))
        elif pin_index_ref == 2 and pin_index == 1:
            return self.ib_vbc(state) + self.ic_vbc(state)
        elif pin_index_ref == 2 and pin_index == 2:
            return self.ib_vbe(state) + self.ic_vbe(state)

    def precompute(self, state, steady_state):
        Vbe = retrieve_voltage(state, self.pins[0]) - retrieve_voltage(state, self.pins[2])
        Vbc = retrieve_voltage(state, self.pins[0]) - retrieve_voltage(state, self.pins[1])
        self.expVbe = math.exp(Vbe / self.Vt)
        self.expVbc = math.exp(Vbc / self.Vt)


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

    def ib(self, state):
        return self.Is * ((self.expVbe - 1) / self.Bf + (self.expVbc - 1) / self.Br)

    def ib_vbe(self, state):
        return self.Is * self.expVbe / self.Vt / self.Bf

    def ib_vbc(self, state):
        return self.Is * self.expVbc / self.Vt / self.Br

    def ic(self, state):
        return self.Is * ((self.expVbe - self.expVbc) - (self.expVbc - 1) / self.Br)

    def ic_vbe(self, state):
        return self.Is * self.expVbe / self.Vt

    def ic_vbc(self, state):
        return self.Is * (-self.expVbc - self.expVbc / self.Br) / self.Vt

    def get_current(self, pin_index, state, steady_state):
        if pin_index == 0:
            return self.ib(state)
        elif pin_index == 1:
            return self.ic(state)
        return -self.ib(state) - self.ic(state)

    def get_gradient(self, pin_index_ref, pin_index, state, steady_state):
        if pin_index_ref == 0 and pin_index == 0:
            return self.ib_vbc(state) + self.ib_vbe(state)
        elif pin_index_ref == 0 and pin_index == 1:
            return -self.ib_vbc(state)
        elif pin_index_ref == 0 and pin_index == 2:
            return -self.ib_vbe(state)
        elif pin_index_ref == 1 and pin_index == 0:
            return self.ic_vbc(state) + self.ic_vbe(state)
        elif pin_index_ref == 1 and pin_index == 1:
            return -self.ic_vbc(state)
        elif pin_index_ref == 1 and pin_index == 2:
            return -self.ic_vbe(state)
        elif pin_index_ref == 2 and pin_index == 0:
            return -(self.ib_vbe(state) + self.ib_vbc(state) + self.ic_vbe(state) + self.ic_vbc(state))
        elif pin_index_ref == 2 and pin_index == 1:
            return self.ib_vbc(state) + self.ic_vbc(state)
        elif pin_index_ref == 2 and pin_index == 2:
            return self.ib_vbe(state) + self.ic_vbe(state)

    def precompute(self, state, steady_state):
        Vbe = retrieve_voltage(state, self.pins[0]) - retrieve_voltage(state, self.pins[2])
        Vbc = retrieve_voltage(state, self.pins[0]) - retrieve_voltage(state, self.pins[1])
        self.expVbe = math.exp(-Vbe / self.Vt)
        self.expVbc = math.exp(-Vbc / self.Vt)

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

    def precompute(self, state, steady_state):
        pass
