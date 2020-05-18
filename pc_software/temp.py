
def gui_update_non_block():
    root.update()
    root.update_idletasks()

while 1:
	gui_update_non_block()
###

root = Tk()
root.title("duckyPad configurator")
root.geometry(str(MAIN_WINDOW_WIDTH) + "x" + str(MAIN_WINDOW_HEIGHT))
root.resizable(width=FALSE, height=FALSE)

lf = LabelFrame(root, text="duckyPad Folder", width=300, height=300)
lf.pack(fill="both", expand=True)
lf.place(x=100, y=100)

# t = Label(lf, text="")
# t.grid()
working_folder_select_button = Button(lf, text = "Select...")
working_folder_select_button.place(x=100, y=100)
# working_folder_select_button.grid(row=0, column=0, stick='w') 


############

import os
import sys
import time
import random
from tkinter import *

MAIN_WINDOW_WIDTH = 800
MAIN_WINDOW_HEIGHT = 533


root = Tk()
root.title("duckyPad configurator")
root.geometry(str(MAIN_WINDOW_WIDTH) + "x" + str(MAIN_WINDOW_HEIGHT))
root.resizable(width=FALSE, height=FALSE)

w = Label(root, text="Hello, world!")
w.pack()

lf = LabelFrame(root, text="Specialist:") 
lf.pack()


t = Label(lf, text="Choose your specialist:")
t.grid(columnspan=2, stick='w')

specialistchoose = IntVar()

r1 = Radiobutton(lf, text="Cardiology", variable=specialistchoose, value=1)
r1.grid(row=1, column=0, stick='w') 

r2 = Radiobutton(lf, text="Gastroenterology", variable=specialistchoose, value=2)
r2.grid(row=1, column=1, stick='w') 

r3 = Radiobutton(lf, text="Dermatology", variable=specialistchoose, value=3)
r3.grid(row=1, column=2, stick='w')

r4 = Radiobutton(lf, text="Psychiatry", variable=specialistchoose, value=4)
r4.grid(row=2, column=0, stick='w')

r5 = Radiobutton(lf, text="Dentist", variable=specialistchoose, value=5)
r5.grid(row=2, column=1, stick='w')


root.mainloop()
####################