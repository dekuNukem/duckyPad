import random

import matplotlib.pyplot as plt 
import numpy as np

fig, ax = plt.subplots()

x = [0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2]
y = [4, 3, 2, 1, 0, 4, 3, 2, 1, 0, 4, 3, 2, 1, 0]

ax.set_xlim(-1, 3) 
ax.set_ylim(-1, 5) 

colours=['blue','green','red','orange','cyan','black','pink','magenta']

def setcolor_xy(ix, iy, r, g, b):
    global ax
    ax.plot(x[ix*5], y[iy], marker='s', markeredgecolor = 'none', markerfacecolor = (r/255, g/255, b/255), markersize=50, linestyle='None')

def rand_255():
    g = random.randint(0, 256)
    r = random.randint(0, 256 - g)
    b = 256 - r - g;
    return (r, g, b)


current_color = rand_255()
current_frame = 0
while 1:
    setcolor_xy(2, 4, current_color[0], current_color[1], current_color[2])
    current_frame += 1
    print(current_frame)
    plt.pause(0.03)

# shadow shape layer, they blend into colors