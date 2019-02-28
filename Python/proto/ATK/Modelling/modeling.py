#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

import numpy as np

EPS = 1.e-8
MAX_ITER = 200 # should probabl have one for steady state and one for non steady state
NUMERICAL_JACOBIAN_DX = 1.e-6


def retrieve_voltage(state, pin):
    """
    Helper function to get the voltage for a given pin
    """
    return state[pin[0]][pin[1]]

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
        self.compute_numerical_jacobian = False
        
    def add_component(self, component, pins):
        """
        add a new component
        :param component: component to add
        :param pins: list of tuples indicating how the component is connected
        """
        self.components.append(component)
        component.pins = pins
        component.update_model(self)
        for (i, pin) in enumerate(pins):
            t, pos = pin
            self.pins[t][pos].append((component, i))
            
    def __repr__(self):
        return "Model with %i pins:\n  " % len(self.pins) + \
                "\n  ".join(["%fV at static pin %i" % (voltage, i) for voltage, i in enumerate(self.static_state)]) + \
                "\nComponents:\n  " + \
                "\n  ".join((repr(component) for component in self.components))
        
    def setup(self):
        """
        Initializes the internal state
        :param steady_state: if set to True (default), computes a steady state
        """
        for component in self.components:
            component.update_steady_state(self.state, self.dt)

        self.solve(True)
    
        for component in self.components:
            component.update_steady_state(self.state, self.dt)

        self.initialized = True
        
    def compute_current(self, pin, state, steady_state):
        """
        Compute Kirschhoff law for the non static pin
        Compute also the jacobian for all the connected pins
        :param pin: tuple indicating which pin we compute the current for
        :param steady_state: if set to True (default), computes for a steady state
        """
        eq = sum([component.get_current(i, state, steady_state) for (component, i) in pin])
        jac = [0] * len(self.dynamic_state)
        for (component, j) in pin:
            for (i, component_pin) in enumerate(component.pins):
                if component_pin[0] == "D":
                    jac[component_pin[1]] += component.get_gradient(j, i, state, steady_state)
        return eq, jac
    
    def solve(self, steady_state):
        """
        Actually solve the equation system
        :param steady_state: if set to True (default), computes for a steady state
        """
        iteration = 0
        while iteration < MAX_ITER and not self.iterate(steady_state):
            iteration = iteration + 1        

    def iterate(self, steady_state):
        """
        Do one iteration
        :param steady_state: if set to True (default), computes for a steady state
        """
        eqs = []
        jacobian = []
        for i, pin in enumerate(self.dynamic_pins):
            if self.dynamic_pins_equation[i] is None:
                eq, jac = self.compute_current(pin, self.state, steady_state)
            else:
                component, eq_number = self.dynamic_pins_equation[i]
                eq, jac = component.add_equation(self.state, steady_state, eq_number)
            eqs.append(eq)
            jacobian.append(jac)

        eqs = np.array(eqs)
        jacobian = np.array(jacobian)
        
        if self.compute_numerical_jacobian:
            self.numerical_jacobian = []
            for j in range(len(self.dynamic_pins)):
                dynamic_state = np.array(self.dynamic_state)
                dynamic_state[j] += NUMERICAL_JACOBIAN_DX
                state = {
                    'D': dynamic_state,
                    'S': self.static_state,
                    'I': self.input_state,
                    }
                neqs = []
                for i, pin in enumerate(self.dynamic_pins):
                    if self.dynamic_pins_equation[i] is None:
                        eq, _ = self.compute_current(pin, state, steady_state)
                    else:
                        component, eq_number = self.dynamic_pins_equation[i]
                        eq, _ = component.add_equation(state, steady_state, eq_number)
                    neqs.append(eq)
                self.numerical_jacobian.append((np.array(neqs) - eqs) / NUMERICAL_JACOBIAN_DX)

        if np.all(np.abs(eqs) < EPS):
            return True
        
        delta = np.linalg.solve(jacobian, eqs)
        if np.all(np.abs(delta) < EPS):
            return True

        max_delta = np.max(np.abs(delta))
        if(max_delta > 1):
            delta /= max_delta
            
        self.dynamic_state -= delta
                
        return False
        
    def __call__(self, input):
        """
        Works out the value for the new input vector
        :param input: vector of input values
        """
        if not self.initialized:
            self.setup()
        
        self.input_state[:] = input

        self.solve(False)
        
        for component in self.components:
            component.update_state(self.state)

        return self.dynamic_state
