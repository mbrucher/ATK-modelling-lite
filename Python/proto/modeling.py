#!/bin/env python

from __future__ import division
from __future__ import print_function

import numpy as np

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
    
    def get_current(self, pin_index, state):
        return (state[self.pins[1]] - state[self.pins[0]])/ self.R * (1 if self.pins[0] == pin_index else -1)

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
        self.initialized = False
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
        
    def setup(self):
        """
        Initializes the internal state
        """
        self.mapping = {}
        for (i, pin) in enumerate(self.pins):
            if not self.static_pins[i]:
                self.mapping[i] = len(self.mapping)
            else:
                self.state[i] = self.get_state(i, pin)
                
        self.initialized = True
        
    def compute_current(self, index, pin):
        return sum([component.get_current(index, self.state) for (component, index) in pin])
    
    def __call__(self, input):
        """
        Works out the value for the new input vector
        """
        if not self.initialized:
            self.setup()
        eqs = []
        for (i, pin) in enumerate(self.pins):
            if not self.static_pins[i]:
                eqs.append(self.compute_current(i, pin))
        
        print(eqs)
        return self.state

if __name__ == "__main__":
    model = Modeler(3)

    model.add_component(Voltage(0), [0])
    model.add_component(Voltage(5), [1])
    model.add_component(Resistor(100), [0, 2])
    model.add_component(Resistor(200), [1, 2])
  
    print(model)

    print(model([]))
    print(model.mapping)
