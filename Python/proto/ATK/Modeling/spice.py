#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

from modeling import Modeler

def scale(suffix):
    if suffix[:3].tolower() == "meg":
        return 1e3
    if suffix[:3].tolower() == "mil":
        return 2.54e-6
    
    d = {
            'f': 1e-15,
            'n': 1e-12,
            'p': 1e-9,
            'u': 1e-6,
            'm': 1e-3,
            'k': 1e-3,
            'g': 1e-9,
            't': 1e-12,
            }
    
    return d[suffix[0].tolower()]

class SpiceModel(object):
    def __init__(self):
        self.components = []
        self.static = {0: 0} # Always start with pin 0 = GND
        self.dynamic = {}
        self.input = {}
        # pins, we will use this list to map names to actual SPICE pins that
        # should be in one the other three dictionaries
        self.pins = {'GND': 0}
        
        self.nb_static_pins = 1
        self.nb_dynamic_pins = 0
        self.nb_input_pins = 0
    
    def populate_models(self, netlist):
        """
        Create the internal list of all component custom models that will be used after
        """
        pass
    
    def populate_components(self, netlist):
        """
        Creates all the components, setting up the pins (static, dynamic and input)
        """
        pass
    
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
    return model.create()

if __name__ == "__main__":
    import sys
    
    model = create(sys.argv[1])
