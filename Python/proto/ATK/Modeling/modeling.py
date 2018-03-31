#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

import numpy as np

EPS = 1.e-8
MAX_ITER = 200 # should probabl have one for steady state and one for non steady state

def retrieve_voltage(state, pin):
    return state[pin[0]][pin[1]]

class Voltage(object):
    """
    Class that sets a pin to a specific voltage
    """
    nb_pins = 1
    
    def __init__(self, V):
        self.V = V
        
    def __repr__(self):
        return "%.0fV at pin %s" % (self.V, self.pins[0])
    
    def update_model(self, model):
        pass

    def update_steady_state(self, state, dt):
        state[self.pins[0][0]][self.pins[0][1]] = self.V

    def update_state(self, state):
        pass

    def precompute(self, state, steady_state):
        pass

class Modeler(object):
    """
    Modeling class
    """
    def __init__(self, nb_dynamic_pins, nb_static_pins, nb_inputs = 0):
        self.components = []
        self.dynamic_pins = [[] for i in range(nb_dynamic_pins)]
        self.dynamic_pins_equation = [None] * nb_dynamic_pins
        self.static_pins = [[] for i in range(nb_static_pins)]
        self.input_pins = [[] for i in range(nb_inputs)]
        self.pins = {
                'D': self.dynamic_pins,
                'S': self.static_pins,
                'I': self.input_pins,
                }
        
        self.dynamic_state = np.zeros(nb_dynamic_pins, dtype=np.float64)
        self.static_state = np.zeros(nb_static_pins, dtype=np.float64)
        self.input_state = np.zeros(nb_inputs, dtype=np.float64)
        self.state = {
                'D': self.dynamic_state,
                'S': self.static_state,
                'I': self.input_state,
                }
        self.initialized = False
        
    def add_component(self, component, pins):
        self.components.append(component)
        component.pins = pins
        component.update_model(self)
        for (i, pin) in enumerate(pins):
            t, pos = pin
            self.pins[t][pos].append((component, i))
            
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
        
    def setup(self):
        """
        Initializes the internal state
        """
        for component in self.components:
            component.update_steady_state(self.state, self.dt)

        self.solve(True)

        for component in self.components:
            component.update_steady_state(self.state, self.dt)

        self.initialized = True
        
    def compute_current(self, pin, steady_state):
        """
        Compute Kirschhoff law for the non static pin
        Compute also the jacobian for all the connected pins
        """
        eq = sum([component.get_current(i, self.state, steady_state) for (component, i) in pin])
        jac = [0] * len(self.dynamic_state)
        for (component, j) in pin:
            for (i, component_pin) in enumerate(component.pins):
                if component_pin[0] == "D":
                    jac[component_pin[1]] += component.get_gradient(j, i, self.state, steady_state)
        return eq, jac
    
    def solve(self, steady_state):
        """
        Actually solve the equaltion system
        """
        iteration = 0
        while iteration < MAX_ITER and not self.iterate(steady_state):
            iteration = iteration + 1        

    def iterate(self, steady_state):
        """
        Do one iteration
        """
        for component in self.components:
            component.precompute(self.state, steady_state)
        eqs = []
        jacobian = []
        for i, pin in enumerate(self.dynamic_pins):
            if self.dynamic_pins_equation[i] is None:
                eq, jac = self.compute_current(pin, steady_state)
            else:
                component, eq_number = self.dynamic_pins_equation[i]
                eq, jac = component.add_equation(eq_number)
            eqs.append(eq)
            jacobian.append(jac)
                
        
        eqs = np.array(eqs)
        jacobian = np.array(jacobian)
        if np.all(np.abs(jacobian)) < EPS:
            return True
        
        delta = np.linalg.solve(jacobian, eqs)
        if np.all(np.abs(delta) < EPS):
            return True

        self.dynamic_state -= delta
                
        return False
        
    def __call__(self, input):
        """
        Works out the value for the new input vector
        """
        if not self.initialized:
            self.setup()
        
        self.input_state[:] = input

        self.solve(False)
        print(self.state)
        
        for component in self.components:
            component.update_state(self.state)

        return self.dynamic_state
