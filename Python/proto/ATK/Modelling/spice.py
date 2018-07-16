#!/usr/bin/env python
# -*- coding: utf-8 -*-

from __future__ import division
from __future__ import print_function

import re
from collections import defaultdict

from modeling import Modeler
from passive import Capacitor, Diode, Coil, Resistor
from active import TransistorNPN, TransistorPNP

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
        self.models = defaultdict(dict)
        self.components = []
        self.static = set((0,)) # Always start with pin 0 = GND
        self.dynamic = set()
        self.input = set()
        self.static_state = [0]
        # pins, we will use this list to map names to actual SPICE pins that
        # should be in one the other three dictionaries
        self.pins = {
                'gnd': ('S', 0),
                '0'  : ('S', 0)
            }
        self.temp_pins = set()

        self.nb_static_pins = 1
        self.nb_dynamic_pins = 0
        self.nb_input_pins = 0
    
    def create_model(self, model):
        """
        Create a new model (indicated by a starting '.model')
        """
        def parse_variable(variable):
            split = variable.split('=')
            split[1] = parse_number(split[1])
            return (split[0].capitalize(), split[1])
        
        if "(" in model[2]:
            tosplit = model[2].split("(")
            variables = [tosplit[1]] + model[3:]
            model[2] = tosplit[0]
        else:
            variables = model[3:]
            variables[0] = variables[0][1:]
        variables[-1] = variables[-1][:-1]
        variables = dict([parse_variable(variable) for variable in variables])
        self.models[model[1]] = (model[2].lower(), variables)
    
    def populate_models(self, netlist):
        """
        Create the internal list of all component custom models that will be used after
        """
        for line in netlist:
            if line[0].lower() == '.model':
                self.create_model(line)

    def handle_pin(self, pin):
        """
        Return a dummy pin value
        """
        if pin not in self.pins:
            self.temp_pins.add(pin)
            return pin
        return self.pins[pin]

    def handle_static_pin(self, pin):
        """
        Return a pin value or create a static one
        """
        if pin not in self.pins:
            self.pins[pin] = 'S', len(self.static)
            self.static.add(pin)
            self.nb_static_pins += 1
        return self.pins[pin]

    def handle_input_pin(self, pin):
        """
        Return a pin value or create an input one
        """
        if pin not in self.pins:
            self.pins[pin] = 'I', len(self.input)
            self.input.add(pin)
            self.nb_input_pins += 1
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
        comp.pins = [pin0, pin1]
        self.components.append(comp)

    def create_coil(self, line):
        """
        Create a coil
        """
        pin0 = self.handle_pin(line[1])
        pin1 = self.handle_pin(line[2])
        L = parse_number(line[3])
        comp = Coil(L)
        comp.pins = [pin0, pin1]
        self.components.append(comp)

    def create_diode(self, line):
        """
        Create a diode
        """
        pin0 = self.handle_pin(line[1])
        pin1 = self.handle_pin(line[2])
        params = self.models[line[3]]
        assert params[0] == 'd'
        comp = Diode(**(params[1]))
        comp.pins = [pin0, pin1]
        self.components.append(comp)

    def create_transistor(self, line):
        """
        Create a transistor
        """
        pin0 = self.handle_pin(line[1])
        pin1 = self.handle_pin(line[2])
        pin2 = self.handle_pin(line[3])
        params = self.models[line[4]]
        comp = self.dispatch_transistor[params[0]](**(params[1]))
        comp.pins = [pin1, pin0, pin2]
        self.components.append(comp)

    def create_resistor(self, line):
        """
        Create a resistor
        """
        pin0 = self.handle_pin(line[1])
        pin1 = self.handle_pin(line[2])
        R = parse_number(line[3])
        comp = Resistor(R)
        comp.pins = [pin0, pin1]
        self.components.append(comp)

    def create_voltage(self, line):
        """
        Create either a fix voltage pin or an input pin
        """
        if len(line) == 4 or (line[3] == "DC" and len(line) == 5):
            pin0 = self.handle_static_pin(line[1])
            pin1 = self.handle_static_pin(line[2])
            V = parse_number(line[4] if len(line) > 4 else line[3])
            if pin0 == ('S', 0):
                self.static_state.append(-V)
            elif pin1 == ('S', 0):
                self.static_state.append(V)
            else:
                assert False
        else:
            # probably AC or sin, so tag this as Input
            pin0 = self.handle_input_pin(line[1])
            pin1 = self.handle_input_pin(line[2])
            assert pin0 == ('S', 0) or pin1 == ('S', 0)
    
    dispatch_component = {
            '.': create_nothing,
            'c': create_capacitor,
            'd': create_diode,
            'l': create_coil,
            'q': create_transistor,
            'r': create_resistor,
            'v': create_voltage,
            }
    
    dispatch_transistor = {
            'npn': TransistorNPN,
            'pnp': TransistorPNP,
            }

    def populate_components(self, netlist):
        """
        Creates all the components, setting up the pins (static, dynamic and input)
        """
        for line in netlist:
            self.dispatch_component[line[0][0].lower()](self, line)

    def populate_dynamic_pins(self):
        """
        Assignes the dynamic pins
        """
        for pin in self.temp_pins:
            if pin not in self.pins:
                self.pins[pin] = 'D', len(self.dynamic)
                self.dynamic.add(pin)
                self.nb_dynamic_pins += 1

        for component in self.components:
            for (i, pin) in enumerate(component.pins):
                if pin in self.temp_pins:
                    component.pins[i] = self.pins[pin]
    
    def parse(self, netlist):
        """
        Parse the netlist
        """
        self.populate_models(netlist)
        self.populate_components(netlist)
        self.populate_dynamic_pins()
    
    def create(self):
        """
        Create the Model for the previous netlist
        """
        model = Modeler(self.nb_dynamic_pins, self.nb_static_pins, self.nb_input_pins)
        model.static_state[:] = self.static_state
        
        return model
    
    def __repr__(self):
        return "SPICE model with (%i,%i,%i) pins:\n  " % (self.nb_static_pins, self.nb_dynamic_pins, self.nb_input_pins) + \
                "\n  ".join(("%s mapped to %s" % pin for pin in self.pins.items())) + \
                "\nComponents:\n  " + \
                "\n  ".join((repr(component) for component in self.components)) + \
                "\nModels:\n  " + \
                "\n  ".join((repr(model) for model in self.models.items()))


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
            netlist[-1].extend(line.strip().split())
        else:
            netlist.append(line.strip().split())

    model = SpiceModel()
    model.parse(netlist)
    print(model)
    return model.create()

if __name__ == "__main__":
    import sys
    
    model = create(sys.argv[1])
