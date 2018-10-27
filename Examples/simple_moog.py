#/usr/bin/env python

from ATK.Core import DoubleInPointerFilter, DoubleOutPointerFilter
from ATK.Modelling import AST, DoubleModellerFilter

ast = AST()

cir = """*** rails ***
V1   v+ 0 DC 10

*** "ladder" ***
Vref ref 0 DC -2
Vs1b s1b 0 DC 3
Vs2b s2b 0 DC 4
Vs3b s3b 0 DC 5
Vs4b s4b 0 DC 6

* input LTP
Q3  s1- in+ lt Q2N3904
Q4  s1+ in- lt Q2N3904
* "stage 1"
C1  s1- s1+ 47n
Q5  s2- s1b s1- Q2N3904
Q6  s2+ s1b s1+ Q2N3904
* "stage 2"
C2  s2- s2+ 47n
Q7  s3- s2b s2- Q2N3904
Q8  s3+ s2b s2+ Q2N3904
* "stage 3"
C3  s3- s3+ 47n
Q9  s4- s3b s3- Q2N3904
Q10 s4+ s3b s3+ Q2N3904
* "stage 4"
C4  s4- s4+ 47n
Q11 v+ s4b s4- Q2N3904
Q12 v+ s4b s4+ Q2N3904

*** output buffer ***
EGain out 0 s4+ s4- 3

*** dc blocking on output ***
C10 out in- 1
R24 in- 0 3.3k

*** expo current source ***
* built with pnp-buffered npn transistor,
* in this case we just use a current source
Is1 lt 0 10u

* input signal 50hz
V4 in+ 0 0 1 50 0 0 0

.TRAN 10u 100m 0m 5u

.MODEL Q2N3904 NPN(VT=0.026 IS=6.73E-15 BF=416.4 BR=0.7374 NE=1.259)
.END""".split("\n")

for line in cir:
  if len(line) == 0 or line[0] == "*":
    continue
  try:
    ast.parse_string(line)
  except:
    pass

import numpy as np
from matplotlib import pyplot as plt

plt.style.use("dark_background")

length = 10000
sampling_rate = 100000
t = np.arange(length)[None, :] * (1/sampling_rate)

x = np.sin(2 * np.pi * 50 * t)

input = DoubleInPointerFilter(x)
input.input_sampling_rate = sampling_rate
input.output_sampling_rate = sampling_rate

filter = DoubleModellerFilter.create_dynamic_filter(ast)
filter.input_sampling_rate = sampling_rate
filter.output_sampling_rate = sampling_rate
filter.set_input_port(0, input, 0)

y = np.zeros((1, length))
output = DoubleOutPointerFilter(y)
output.input_sampling_rate = sampling_rate
output.output_sampling_rate = sampling_rate
output.set_input_port(0, filter, 10)

output.process(length)

plt.plot(t[0], x[0])
plt.plot(t[0], y[0])
plt.title("Moog filter in pure Python")
plt.show()

