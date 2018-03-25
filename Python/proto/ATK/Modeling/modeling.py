#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

import numpy as np
import math

EPS = 1.e-8
MAX_ITER = 8

class Voltage(object):
    """
    Class that sets a pin to a specific voltage
    """
    nb_pins = 1
    
    def __init__(self, V):
        self.V = V
        self.pins = []
        
    def __repr__(self):
        return "%.0fV at pin %i" % (self.V, self.pins[0])
    
    def update_model(self, model, index):
        model.static_pins[index] = True
        
    def update_steady_state(self, state, dt):
        pass

    def update_state(self, state):
        pass

class Input(Voltage):
    """
    Class that sets a pin to a specific voltage based on input condition
    Just another name for Voltage node
    """
    pass

class Resistor(object):
    """
    Class that implements a resistor between two pins
    """
    nb_pins = 2
    
    def __init__(self, R):
        self.R = R
        self.pins = []

    def __repr__(self):
        return "%.0fohms between pins (%i,%i)" % (self.R, self.pins[0], self.pins[1])

    def update_model(self, model, index):
        pass
    
    def update_steady_state(self, state, dt):
        pass

    def update_state(self, state):
        pass

    def get_current(self, pin_index, state):
        return (state[self.pins[1]] - state[self.pins[0]]) / self.R * (1 if 0 == pin_index else -1)

    def get_gradient(self, pin_index_ref, pin_index, state):
        return (1 if 1 == pin_index else -1) * (1 if 0 == pin_index_ref else -1) / self.R


class Capacitor(object):
    """
    Class that implements a capacitor between two pins
    """
    nb_pins = 2
    
    def __init__(self, C):
        self.C = C
        self.pins = []

    def __repr__(self):
        return "%.0fohms between pins (%i,%i)" % (self.R, self.pins[0], self.pins[1])

    def update_model(self, model, index):
        pass
    
    def update_steady_state(self, state, dt):
        self.dt = dt
        self.c2t = (2 * self.C) / dt
        print("c2t = %f" % self.c2t)
        
        self.iceq = self.c2t * (state[self.pins[1]] - state[self.pins[0]])

    def update_state(self, state):
        self.iceq = 2 * self.c2t * (state[self.pins[1]] - state[self.pins[0]]) - self.iceq
        print("  %f %f" % (state[self.pins[1]], state[self.pins[0]]))
        print("  %f" % self.iceq)

    def get_current(self, pin_index, state):
        return ((state[self.pins[1]] - state[self.pins[0]]) * self.c2t - self.iceq) * (1 if 0 == pin_index else -1)

    def get_gradient(self, pin_index_ref, pin_index, state):
        return (1 if 1 == pin_index else -1) * (1 if 0 == pin_index_ref else -1) * self.c2t

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
        self.pins = []

    def __repr__(self):
        return "%.0fohms between pins (%i,%i)" % (self.R, self.pins[0], self.pins[1])

    def update_model(self, model, index):
        pass
    
    def update_steady_state(self, state, dt):
        pass

    def update_state(self, state):
        pass

    def ib(self, state):
        Vbe = state[self.pins[0]] - state[self.pins[2]]
        Vbc = state[self.pins[0]] - state[self.pins[1]]
        return self.Is * ((math.exp(Vbe / self.Vt) - 1) / self.Bf + (math.exp(Vbc / self.Vt) - 1) / self.Br)

    def ib_vbe(self, state):
        Vbe = state[self.pins[0]] - state[self.pins[2]]
        return self.Is * math.exp(Vbe / self.Vt) / self.Vt / self.Bf

    def ib_vbc(self, state):
        Vbc = state[self.pins[0]] - state[self.pins[1]]
        return self.Is * math.exp(Vbc / self.Vt) / self.Vt / self.Br

    def ic(self, state):
        Vbe = state[self.pins[0]] - state[self.pins[2]]
        Vbc = state[self.pins[0]] - state[self.pins[1]]
        return self.Is * ((math.exp(Vbe / self.Vt) - math.exp(Vbc / self.Vt)) - (math.exp(Vbc / self.Vt) - 1) / self.Br)

    def ic_vbe(self, state):
        Vbe = state[self.pins[0]] - state[self.pins[2]]
        return self.Is * math.exp(Vbe / self.Vt) / self.Vt

    def ic_vbc(self, state):
        Vbc = state[self.pins[0]] - state[self.pins[1]]
        return self.Is * (-math.exp(Vbc / self.Vt) - math.exp(Vbc / self.Vt) / self.Br) / self.Vt

    def get_current(self, pin_index, state):
        if pin_index == 0:
            return -self.ib(state)
        elif pin_index == 1:
            return -self.ic(state)
        return self.ib(state) + self.ic(state)

    def get_gradient(self, pin_index_ref, pin_index, state):
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

class Modeler(object):
    """
    Modeling class
    """
    def __init__(self, size):
        self.components = []
        self.pins = [[] for i in range(size)]
        self.static_pins = [False] * size
        self.state = np.zeros(size, dtype=np.float64)
        self.initialized = False
        
    def add_component(self, component, pins):
        self.components.append(component)
        component.pins = pins
        for (i, pin) in enumerate(pins):
            component.update_model(self, pin)
            self.pins[pin].append((component, i))
            
    def __repr__(self):
        return "Model with %i pins:\n  " % len(self.pins) + "\n  ".join((repr(component) for component in self.components))

    def get_state(self, index, pin):
        """
        Retrieve the static state for a pin
        """
        for component in pin:
            if hasattr(component[0], "V"):
                return component[0].V
        raise RuntimeError("Pin %i is declared static but can't retrieve a voltage state for it" % index)
        
    def setup(self, recompute_state = True):
        """
        Initializes the internal state
        """
        self.mapping = {}
        for (i, pin) in enumerate(self.pins):
            if not self.static_pins[i]:
                self.mapping[i] = len(self.mapping)
            else:
                self.state[i] = self.get_state(i, pin)
        
        if recompute_state:
            self.solve()

        for component in self.components:
            component.update_steady_state(self.state, self.dt)

        self.initialized = True
        
    def compute_current(self, pin):
        """
        Compute Kirschhoff law for the non static pin
        Compute also the jacobian for all the connected pins
        """
        eq = sum([component.get_current(i, self.state) for (component, i) in pin])
        jac = [0] * len(self.mapping)
        for (component, j) in pin:
            for (i, component_pin) in enumerate(component.pins):
                if not self.static_pins[component_pin]:
                    jac[self.mapping[component_pin]] += component.get_gradient(j, i, self.state)
        return eq, jac
    
    def solve(self):
        """
        Actually solve the equaltion system
        """
        iteration = 0
        while iteration < MAX_ITER and not self.iterate():
          iteration = iteration + 1        

    def iterate(self):
        """
        Do one iteration
        """        
        eqs = []
        jacobian = []
        for (pin_index, mapped) in self.mapping.items():
            eq, jac = self.compute_current(self.pins[pin_index])
            eqs.append(eq)
            jacobian.append(jac)
        
        eqs = np.array(eqs)
        jacobian = np.array(jacobian)
        if np.all(np.abs(jacobian)) < EPS:
            return True
        
        delta = np.linalg.solve(jacobian, eqs)
        if np.all(np.abs(delta)) < EPS:
            return True

        self.state[self.mapping.keys()] -= delta[self.mapping.values()]
                
        return False
        
    def __call__(self, input):
        """
        Works out the value for the new input vector
        """
        if not self.initialized:
            self.setup()
        
        for (key, value) in input.items():
            self.state[key] = value
        
        self.solve()
        
        for component in self.components:
            component.update_state(self.state)

        return self.state

if __name__ == "__main__":
    model = Modeler(3)

    model.add_component(Voltage(0), [0])
    model.add_component(Voltage(5), [1])
    model.add_component(Resistor(100), [0, 2])
    model.add_component(Resistor(200), [2, 1])
    
    model.dt = 1.e-3
  
    print(model)

    print(model({}))
    print(model({}))
