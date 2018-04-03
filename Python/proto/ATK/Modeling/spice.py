#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

import re

from modeling import Modeler
from passive import Capacitor, Resistor

digits = re.compile("([\d\.e-]+)(.*)")

def scale(suffix):
    if suffix[:3].lower() == "meg":
        return 1e3
    if suffix[:3].lower() == "mil":
        return 2.54e-6
    
    d = {
            'f': 1.e-15,
            'n': 1.e-12,
            'p': 1.e-9,
            'u': 1.e-6,
            'm': 1.e-3,
            'k': 1.e3,
            'g': 1.e9,
            't': 1.e12,
        }
    if len(suffix) > 0:
        suf = suffix[0].lower()
        if suf in d:
            return d[suf]
    return 1

def parse_number(number):
    match = digits.match(number)
    return float(match.group(1)) * scale(match.group(2))

class SpiceModel(object):
    def __init__(self):
        self.components = []
        self.static = set((0,)) # Always start with pin 0 = GND
        self.dynamic = set()
        self.input = set()
        # pins, we will use this list to map names to actual SPICE pins that
        # should be in one the other three dictionaries
        self.pins = {
                'gnd': ('S', 0),
                '0'  : ('S', 0)
            }
        
        self.nb_static_pins = 1
        self.nb_dynamic_pins = 0
        self.nb_input_pins = 0
    
    def create_model(self, netlist):
        """
        Create a new model (indicated by a starting '.')
        """
        pass
    
    def populate_models(self, netlist):
        """
        Create the internal list of all component custom models that will be used after
        """
        for line in netlist:
            if line[0][0] == '.':
                self.create_model(line)

    def handle_pin(self, pin):
        """
        Return a pin value or create one
        """
        if pin not in self.pins:
            self.pins[pin] = 'D', len(self.dynamic)
            self.dynamic.add(pin)
            self.nb_dynamic_pins += 1
        return self.pins[pin]

    def create_nothing(self, line):
        """
        Create nothing
        """
        pass
    
    def create_capacitor(self, line):
        """
        Create a capacitor
        """
        pin0 = self.handle_pin(line[1])
        pin1 = self.handle_pin(line[2])
        C = parse_number(line[3])
        comp = Capacitor(C)
        comp.pins = (pin0, pin1)
        self.components.append(comp)

    def create_resistor(self, line):
        """
        Create a resistor
        """
        pin0 = self.handle_pin(line[1])
        pin1 = self.handle_pin(line[2])
        R = parse_number(line[3])
        comp = Resistor(R)
        comp.pins = (pin0, pin1)
        self.components.append(comp)

    def create_voltage(self, line):
        """
        Create either a fix voltage pin or an input pin
        """
        pass
    
    dispatch_component = {
            '.': create_nothing,
            'c': create_capacitor,
            'r': create_resistor,
            'v': create_voltage,
            }

    def populate_components(self, netlist):
        """
        Creates all the components, setting up the pins (static, dynamic and input)
        """
        for line in netlist:
            self.dispatch_component[line[0][0].lower()](self, line)
    
    def parse(self, netlist):
        """
        Parse the netlist
        """
        self.populate_models(netlist)
        self.populate_components(netlist)
    
    def create(self):
        """
        Create the Model for the previous netlist
        """
        model = Modeler(self.nb_dynamic_pins, self.nb_static_pins, self.nb_input_pins)
        
        return model
    
    def __repr__(self):
        return "SPICE model with (%i,%i,%i) pins:\n  " % (self.nb_static_pins, self.nb_dynamic_pins, self.nb_input_pins) + \
                "\n  ".join(("%s mapped to %s" % pin for pin in self.pins.items())) + \
                "\nComponents:\n  " + \
                "\n  ".join((repr(component) for component in self.components))


def create(filename):
    """
    Create a model based on a SPICE cir file
    """
    f = open(filename)
    
    netlist = []
    
    print("Analyzing netlist: %s" % f.readline().strip())
    
    for line in f:
        line = line.strip()
        if len(line) == 0 or line[0] == '*':
            continue
        if line[0] == '+':
            netlist[-1].extend(line.split())
        else:
            netlist.append(line.split())

    model = SpiceModel()
    model.parse(netlist)
    print(model)
    return model.create()

if __name__ == "__main__":
    import sys
    
    model = create(sys.argv[1])
