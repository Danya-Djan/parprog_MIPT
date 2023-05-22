#!/usr/bin/python

import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import numpy as np


TIME = 10
SIZE = 10

data = open('solution_mpi.txt', 'r')

DATA = []
for line in data:
    DATA.append(list(map(float, line.split())))
data.close()

x_band = np.linspace(0, SIZE, len(DATA))
t_band = np.linspace(0, TIME, len(DATA[0]))
y, x = np.meshgrid(t_band, x_band)
data_band = np.array(DATA)

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.plot_surface(x, y, data_band)
plt.savefig('solution_mpi.png')