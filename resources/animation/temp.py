import matplotlib.pyplot as plt

x=[0,0,1,1,2,2,3,3]
y=[-1,3,2,-2,0,2,3,1]
colour=['blue','green','red','orange','cyan','black','pink','magenta']
values=[2,6,10,8,0,9,3,6]

for i in range(len(x)):
    plt.plot(x[i], y[i], linestyle='none', color=colour[i], marker='o')

plt.axis([-1,4,-3,4])
plt.show()



import matplotlib.pyplot as plt 
import numpy as np

fig, ax = plt.subplots()

x = [0, 1, 2, 3, 4]
y = [0, 1, 2, 3, 4]

points, = ax.plot(x, y, marker='s', markeredgecolor = 'none', markerfacecolor = (1,0.5,0.5), markersize=200, linestyle='None')
ax.set_xlim(0, len(x)) 
ax.set_ylim(0, len(y)) 

while 1:
    new_x = np.random.randint(10, size=5)
    new_y = np.random.randint(10, size=5)
    points.set_data(new_x, new_y)
    plt.pause(0.5)



import matplotlib.pyplot as plt 
import numpy as np

fig, ax = plt.subplots()

x = [0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2]
y = [4, 3, 2, 1, 0, 4, 3, 2, 1, 0, 4, 3, 2, 1, 0]

ax.set_xlim(-1, 3) 
ax.set_ylim(-1, 5) 

colours=['blue','green','red','orange','cyan','black','pink','magenta']

def setcolor_xy(x, y, r, g, b):
    ax.plot(x[x], y[y], marker='s', markeredgecolor = 'none', markerfacecolor = (r, g, b), markersize=50, linestyle='None')

# plt.plot(x[i], y[i], linestyle='none', color=colour[i], marker='o')
while 1:
    for iiiii in range(len(x)):
        ax.plot(x[iiiii], y[iiiii], marker='s', markeredgecolor = 'none', markerfacecolor = colours[iiiii % len(colours)], markersize=50, linestyle='None')
    plt.pause(0.5)