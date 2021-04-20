import time
import hid_helper
from tkinter import *
from tkinter import filedialog
from tkinter import simpledialog
from tkinter.colorchooser import askcolor
from tkinter import messagebox
import urllib.request

THIS_VERSION_NUMBER = '0.1.0'
MAIN_WINDOW_WIDTH = 800
MAIN_WINDOW_HEIGHT = 600

def repeat_func():
    print('hello world!')
    root.after(500, repeat_func)

root = Tk()
root.title("duckyPad profile auto switcher" + THIS_VERSION_NUMBER)
root.geometry(str(MAIN_WINDOW_WIDTH) + "x" + str(MAIN_WINDOW_HEIGHT))
# root.resizable(width=FALSE, height=FALSE)

connection_info_lf = LabelFrame(root, text="Connection")
connection_info_lf.grid(sticky='WE', padx=5, pady=5, ipadx=5, ipady=5)

root.after(500, repeat_func)
root.mainloop()
