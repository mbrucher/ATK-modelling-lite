#!/bin/env python

from __future__ import division
from __future__ import print_function

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

class Modeler(object):
    """
    Modeling class
    """
    def __init__(self, size):
        self.size = size
        self.components = []
        self.pins = [[] for i in range(size)]
        self.static_pins = [False] * size
        
    def add_component(self, component, pins):
        self.components.append(component)
        component.pins = pins
        for (i, pin) in enumerate(pins):
            self.pins[pin].append((component, i))
        

if __name__ == "__main__":
  model = Modeler(3)

  model.add_component(Voltage(0), [0])
  model.add_component(Voltage(5), [1])
  model.add_component(Resistor(100), [0, 2])
  model.add_component(Resistor(200), [1, 2])
  
  print(model.pins)
