#!/usr/bin/env python3

# Make a legend for specific lines.
import matplotlib.pyplot as plt
import numpy as np
import os

FREQUENCY = 104     # Hz


# Load csv file into numpy arrays
fname = os.path.realpath(os.path.join(
    os.path.dirname(os.path.realpath(__file__)),
    '../data/', 'Andrew_walk_hop_walk_run_stripped.csv'))

raw_data = np.genfromtxt(fname, delimiter = ',', skip_header = 2, usecols =
        (4,5,6,7,8,9)).transpose()
# print(raw_data)

# alist:  [0] = ax, [1] = ay, [2] = az
alist = raw_data[0:3]
atitlelist = ['ax', 'ay', 'az']
# print(alist)

# alist:  [0] = gx, [1] = gy, [2] = gz
glist = raw_data[3:6]
gtitlelist = ['gx', 'gy', 'gz']
# print(glist)

# Plot ax ay az gx gz gy against t

fig1 = plt.figure()


mplt = fig1.add_subplot(311)
start = 1000
end   = 1300
num_records = end - start
delta_t = 1.0 / FREQUENCY # second
t = np.linspace(0.0, delta_t * num_records, num_records)
zeros = np.linspace(0.0, 0.0, num_records)
mplt.set_title('ax, ay, gz, hop')
mplt.plot(t, zeros, color = 'black')
mplt.plot(t,        raw_data[0, start:end], 'or--', color = 'red')
mplt.plot(t,        raw_data[1, start:end], 'or--', color = 'blue')
mplt.plot(t, 4.5 *  raw_data[5, start:end], 'or--', color = 'green')


mplt = fig1.add_subplot(312)
start = 1800
end   = 2100
num_records = end - start
delta_t = 1.0 / FREQUENCY # second
t = np.linspace(0.0, delta_t * num_records, num_records)
zeros = np.linspace(0.0, 0.0, num_records)
mplt.set_title('ax, ay, gz, run')
mplt.plot(t, zeros, color = 'black')
mplt.plot(t,        raw_data[0, start:end], 'or--', color = 'red')
mplt.plot(t,        raw_data[1, start:end], 'or--', color = 'blue')
mplt.plot(t, 4.5 *  raw_data[5, start:end], 'or--', color = 'green')


mplt = fig1.add_subplot(313)
start = 1400
end   = 1700
num_records = end - start
delta_t = 1.0 / FREQUENCY # second
t = np.linspace(0.0, delta_t * num_records, num_records)
zeros = np.linspace(0.0, 0.0, num_records)
mplt.set_title('ax, ay, gz, walk')
mplt.plot(t, zeros, color = 'black')
mplt.plot(t,        raw_data[0, start:end], 'or--', color = 'red')
mplt.plot(t,        raw_data[1, start:end], 'or--', color = 'blue')
mplt.plot(t, 4.5 *  raw_data[5, start:end], 'or--', color = 'green')

# aplot = fig1.add_subplot(111)
# aplot.set_title('a')
# axl = aplot.plot(t, alist[0], color = 'red')
# ayl = aplot.plot(t, alist[1], color = 'blue')
# azl = aplot.plot(t, alist[2], color = 'green')

# gplot = fig1.add_subplot(212, sharex = aplot)
# gplot.set_title('g')
# gxl = gplot.plot(t, glist[0], color = 'red')
# gyl = gplot.plot(t, glist[1], color = 'blue')
# gzl = gplot.plot(t, glist[2], color = 'green')


# All six graphs
# fig2 = plt.figure()

# tmpplt = fig2.add_subplot(231)
# tmpplt.set_title('ax')
# tmpplt.plot(t, alist[0], color = 'red')

# tmpplt = fig2.add_subplot(232)
# tmpplt.set_title('ay')
# tmpplt.plot(t, alist[1], color = 'blue')

# tmpplt = fig2.add_subplot(233)
# tmpplt.set_title('az')
# tmpplt.plot(t, alist[2], color = 'green')

# tmpplt = fig2.add_subplot(234)
# tmpplt.set_title('gx')
# tmpplt.plot(t, glist[0], color = 'red')

# tmpplt = fig2.add_subplot(235)
# tmpplt.set_title('gy')
# tmpplt.plot(t, glist[1], color = 'blue')

# tmpplt = fig2.add_subplot(236)
# tmpplt.set_title('gz')
# tmpplt.plot(t, glist[2], color = 'green')


# t1 = np.linspace(0.0, 2.0, 20)
# t2 = np.linspace(0.0, 2.0, 200)

# note that plot returns a list of lines.  The "l1, = plot" usage
# extracts the first element of the list into l1 using tuple
# unpacking.  So l1 is a Line2D instance, not a sequence of lines
# l1, = plt.plot(t2, np.exp(-t2))
# l2, l3 = plt.plot(t2, np.sin(2 * np.pi * t2), '--go', t1, np.log(1 + t1), '.')
# l4, = plt.plot(t2, np.exp(-t2) * np.sin(2 * np.pi * t2), 'rs-.')

# plt.legend((l2, l4), ('oscillatory', 'damped'), loc='upper right', shadow=True)
# plt.xlabel('time')
# plt.ylabel('volts')
# plt.title('Damped oscillation')

plt.show()
