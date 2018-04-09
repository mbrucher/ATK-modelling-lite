# ATK-modelling
A JIT-based modelling tool (real-time spice without MNA).

This is still a work in progress, just at the stage of a Python prototype. The final goal is to have a dynamic SPICE interpreter and in the non-lite version a fully JIT version (which will also generate the corresponding C++ code).

## Python prototype

The Python prototype is not meant to have as many features as the C++ module. Its goal is to help figure out the design and the issue that a C++ module will face, both as dynamic and static modelers.

### Python Modeler

The Python modeler is capable of describing circuits that have the following elements:
* Resistors,
* Capacitors,
* Coils,
* Diodes,
* Antiparallel diodes (because they are faster to describe and simulate than 2 diodes),
* NPN and PNP transistors,
* Perfect and ideal OpAmp.

Pins (voltage) can be:
* static ('S'), meaning that it is fixed voltage for the simulation,
* input ('I'), meaning that it is going to be a future input for the simulation,
* dynamic ('D'), meaning that the state of this pin will be computed by the modeler.

The circuit is first analyzed in steady state before inputs can be driven inside the model.

The issues are the following:
* steady state of a coil is not perfectly done, the state of the two pins should be collapsed and solved as one. Currently, it is solved with a very small resistance
* it is not possible to have a constant additional voltage on top of a dynamic pin.

### SPICE parser

The SPICE parser is very dull and stupid. As it's not a real parser, there are a few things the SPICE modeler has to do for the moment:
* a first pass on all the lines creates the models,
* dynamic pins are not known until all voltages are parsed. If it were a parser, one could just get all the voltages and set the static states,
* not all the SPICE variables for a given model are known, which would lead to errors when encountered.

