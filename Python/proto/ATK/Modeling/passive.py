#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

import math

from modeling import retrieve_voltage

class Resistor(object):
    """
    Class that implements a resistor between two pins
    """
    nb_pins = 2
    
    def __init__(self, R):
        self.R = R
        self.G = 1 / R

    def __repr__(self):
        return "%02.2eohms between pins (%s,%s)" % (self.R, self.pins[0], self.pins[1])

    def update_model(self, model):
        pass

    def update_steady_state(self, state, dt):
        pass

    def update_state(self, state):
        pass

    def get_current(self, pin_index, state, steady_state):
        return (retrieve_voltage(state, self.pins[1]) - retrieve_voltage(state, self.pins[0])) * self.G * (1 if 0 == pin_index else -1)

    def get_gradient(self, pin_index_ref, pin_index, state, steady_state):
        return (1 if 1 == pin_index else -1) * (1 if 0 == pin_index_ref else -1) * self.G

    def precompute(self, state, steady_state):
        pass


class Capacitor(object):
    """
    Class that implements a capacitor between two pins
    Inspired by http://qucs.sourceforge.net/tech/node26.html
    """
    nb_pins = 2
    
    def __init__(self, C):
        self.C = C

    def __repr__(self):
        return "%02.2eF between pins (%s,%s)" % (self.C, self.pins[0], self.pins[1])
    
    def update_model(self, model):
        pass

    def update_steady_state(self, state, dt):
        self.dt = dt
        self.c2t = (2 * self.C) / dt
        
        self.iceq = self.c2t * (retrieve_voltage(state, self.pins[1]) - retrieve_voltage(state, self.pins[0]))

    def update_state(self, state):
        self.iceq = 2 * self.c2t * (retrieve_voltage(state, self.pins[1]) - retrieve_voltage(state, self.pins[0])) - self.iceq

    def get_current(self, pin_index, state, steady_state):
        if steady_state:
            return 0
        return ((retrieve_voltage(state, self.pins[1]) - retrieve_voltage(state, self.pins[0])) * self.c2t - self.iceq) * (1 if 0 == pin_index else -1)

    def get_gradient(self, pin_index_ref, pin_index, state, steady_state):
        if steady_state:
            return 0
        return (1 if 1 == pin_index else -1) * (1 if 0 == pin_index_ref else -1) * self.c2t

    def precompute(self, state, steady_state):
        pass


class Coil(object):
    """
    Class that implements a coilbetween two pins
    Inspired by http://qucs.sourceforge.net/tech/node26.html
    """
    nb_pins = 2
    
    def __init__(self, L):
        self.L = L
        self.i = 0

    def __repr__(self):
        return "%2.2eH between pins (%s,%s)" % (self.L, self.pins[0], self.pins[1])

    def update_model(self, model):
        pass

    def update_steady_state(self, state, dt):
        self.dt = dt
        self.l2t = (2 * self.L) / dt
        self.invl2t = 1 / self.l2t
        
        self.veq = self.l2t * self.i

    def update_state(self, state):
        self.veq = 2 * self.l2t * self.i - self.veq

    def get_current(self, pin_index, state, steady_state):
        return self.i  * (1 if 0 == pin_index else -1)

    def get_gradient(self, pin_index_ref, pin_index, state, steady_state):
        if steady_state:
            return (1 if 1 == pin_index else -1) * (1 if 0 == pin_index_ref else -1) * 1000000
        return (1 if 1 == pin_index else -1) * (1 if 0 == pin_index_ref else -1) * self.invl2t

    def precompute(self, state, steady_state):
        if steady_state:
            self.i = (retrieve_voltage(state, self.pins[1]) - retrieve_voltage(state, self.pins[0])) * 1000000
        else:
            self.i = (retrieve_voltage(state, self.pins[1]) - retrieve_voltage(state, self.pins[0]) + self.veq) * self.invl2t


class Diode(object):
    """
    Class that implements a diode between two pins
    """
    nb_pins = 2
    
    def __init__(self, Is=1e-14, N=1.24, Vt = 26e-3):
        self.Is = Is
        self.n = N
        self.Vt = Vt

    def __repr__(self):
        return "Diode between pins (%s,%s)" % (self.pins[0], self.pins[1])

    def update_model(self, model):
        pass

    def update_steady_state(self, state, dt):
        pass

    def update_state(self, state):
        pass

    def get_current(self, pin_index, state, steady_state):
        return self.Is * (self.one_diode - 1) * (1 if 1 == pin_index else -1)

    def get_gradient(self, pin_index_ref, pin_index, state, steady_state):
        return self.Is / (self.n * self.Vt) * self.one_diode * (1 if 0 == pin_index else -1) * (1 if 1 == pin_index_ref else -1)

    def precompute(self, state, steady_state):
        self.one_diode = math.exp((retrieve_voltage(state, self.pins[0]) - retrieve_voltage(state, self.pins[1])) / (self.n * self.Vt))


class AntiParallelDiode(object):
    """
    Class that implements a diode between two pins
    """
    nb_pins = 2
    
    def __init__(self, Is=1e-14, n=1.24, Vt = 26e-3):
        self.Is = Is
        self.n = n
        self.Vt = Vt

    def __repr__(self):
        return "Antiparallel diodes between pins (%s,%s)" % (self.pins[0], self.pins[1])
    
    def update_model(self, model):
        pass

    def update_steady_state(self, state, dt):
        pass

    def update_state(self, state):
        pass

    def get_current(self, pin_index, state, steady_state):
        return self.Is * (self.one_diode - 1 / self.one_diode) * (1 if 1 == pin_index else -1)

    def get_gradient(self, pin_index_ref, pin_index, state, steady_state):
        return self.Is / (self.n * self.Vt) * (self.one_diode + 1/ self.one_diode ) * (1 if 0 == pin_index else -1) * (1 if 1 == pin_index_ref else -1)

    def precompute(self, state, steady_state):
        self.one_diode = math.exp((retrieve_voltage(state, self.pins[0]) - retrieve_voltage(state, self.pins[1])) / (self.n * self.Vt))
