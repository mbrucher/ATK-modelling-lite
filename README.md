# ATK-modelling
A JIT-based modelling tool (real-time spice with MNA).

This is still a work in progress, just at the stage of a Python prototype. The final goal is to have a dynamic SPICE interpreter and in the non-lite version a fully JIT version (which will also generate the corresponding C++ code).

## Python prototype

The Python prototype is not meant to have as many features as the C++ module. Its goal is to help figure out the design and the issue that a C++ module will face, both as dynamic and static modellers.

### Python Modeller

The Python modeller is capable of describing circuits that have the following elements:
* Resistors,
* Capacitors,
* Coils,
* Diodes,
* Antiparallel diodes (because they are faster to describe and simulate than 2 diodes),
* NPN and PNP transistors,
* Perfect and ideal OpAmp.
* Static current generator
* Voltage gain (4 pins)

Pins (voltage) can be:
* static ('S'), meaning that it is fixed voltage for the simulation,
* input ('I'), meaning that it is going to be a future input for the simulation,
* dynamic ('D'), meaning that the state of this pin will be computed by the modeller.

The circuit is first analyzed in steady state before inputs can be driven inside the model.

The issues are the following:
* steady state of a coil is not perfectly done, the state of the two pins should be collapsed and solved as one. Currently, it is solved with a very small resistance
* it is not possible to have a constant additional voltage on top of a dynamic pin.

### SPICE parser

The SPICE parser is very dull and stupid. As it's not a real parser, there are a few things the SPICE modeller has to do for the moment:
* a first pass on all the lines creates the models,
* dynamic pins are not known until all voltages are parsed. If it were a parser, one could just get all the voltages and set the static states,
* not all the SPICE variables for a given model are known, which would lead to errors when encountered.
* output pins are randomly assigned

## C++ implementation

The C++ implementation uses Audio ToolKit framework to allow easy integration in a plugin.

### Dynamic modeller

The dynamic modeller supports all the components from the Python modeller:
* Resistors,
* Capacitors,
* Coils,
* Diodes,
* Antiparallel diodes (because they are faster to describe and simulate than 2 diodes),
* NPN and PNP transistors,
* Perfect and ideal OpAmp (3 pins)
* Static current generator
* Voltage gain (4 pins)

Pins are identified by an enum class **PinType**:
* Static,
* Dynamic,
* Input

A model can be created by setting the number of input, static and dynamic pins and then populate the model by creating components.

### SPICE parser for the dynamic modeller

SPICE netlists can be parsed to create a dynamic modeller as well. The parser is based on Boost Spirit X3 and can parse lots of files, but can still fail on some cases. Continuation lines (**+**) are not yet supported. 

The process is different than for the Python parser. First all lines are processed and an AST is built with the components on one side and the different mdoels on another side. Then voltages are processed to create static and inputs voltages. Any voltage pin that has more than **DC** is considered as an input.

The second pass creates the components in the order of the declared components. Creation of the dynamic voltages (and possible outputs) are created on the fly when assigned to the components.

**There is no support at this point for mapping between pins and the index (for inputs and for outputs).**

The Newton Raphson process is based on Eigen Householder decomposition. 

**Note that not all netlists are supported, include keywords are not processed as well as any other transient analysis.**

### SPICE JIT for a static modeller

### Optimizer modeller

