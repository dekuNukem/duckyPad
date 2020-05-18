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

lf = LabelFrame(root, text="duckyPad Folder", width=300, height=300)
lf.pack(fill="both", expand=True)
lf.place(x=100, y=100)

# t = Label(lf, text="")
# t.grid()
working_folder_select_button = Button(lf, text = "Select...")
working_folder_select_button.place(x=100, y=100)
# working_folder_select_button.grid(row=0, column=0, stick='w') 



root.mainloop()
