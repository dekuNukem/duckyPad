import os
import sys
import time
import random
from tkinter import *
from tkinter import filedialog

MAIN_WINDOW_WIDTH = 800
MAIN_WINDOW_HEIGHT = 533
PADDING = 10
HIGHT_ROOT_FOLDER_LF = 50
INVALID_ROOT_FOLDER_STRING = "<-- Please select root folder"

def select_root_folder():
	global dp_root_folder_display
	global root_folder_path_label
	dir_result = filedialog.askdirectory()
	if len(dir_result) <= 0:
		return
	dp_root_folder = dir_result
	dp_root_folder_display.set("Selected: " + dir_result)
	root_folder_path_label.config(foreground='navy')

root = Tk()
root.title("duckyPad configurator")
root.geometry(str(MAIN_WINDOW_WIDTH) + "x" + str(MAIN_WINDOW_HEIGHT))
root.resizable(width=FALSE, height=FALSE)

dp_root_folder_display = StringVar()
dp_root_folder = ''
dp_root_folder_display.set(INVALID_ROOT_FOLDER_STRING)

root_folder_lf = LabelFrame(root, text="duckyPad Folder", width=MAIN_WINDOW_WIDTH - PADDING*2, height=HIGHT_ROOT_FOLDER_LF)
root_folder_lf.pack()
root_folder_lf.place(x=PADDING, y=0)

root_folder_select_button = Button(root_folder_lf, text="Select...", command=select_root_folder, width = '8')
root_folder_select_button.pack()
root_folder_select_button.place(x=PADDING, y=0)
root.update()

profiles_lf = LabelFrame(root, text="Profiles", width=int(MAIN_WINDOW_WIDTH / 3 - PADDING * 1.3), height=MAIN_WINDOW_HEIGHT - HIGHT_ROOT_FOLDER_LF - PADDING)
profiles_lf.pack()
profiles_lf.place(x=PADDING, y=HIGHT_ROOT_FOLDER_LF)
root.update()

keys_lf = LabelFrame(root, text="Keys", width=int(MAIN_WINDOW_WIDTH / 3 - PADDING * 1.3), height=MAIN_WINDOW_HEIGHT - HIGHT_ROOT_FOLDER_LF - PADDING)
keys_lf.pack()
keys_lf.place(x=profiles_lf.winfo_x() + profiles_lf.winfo_width() + PADDING, y=profiles_lf.winfo_y())
keys_lf.pack_propagate(False) 
root.update()

scripts_lf = LabelFrame(root, text="Scripts", width=int(MAIN_WINDOW_WIDTH / 3 - PADDING * 1.3), height=MAIN_WINDOW_HEIGHT - HIGHT_ROOT_FOLDER_LF - PADDING)
scripts_lf.pack()
scripts_lf.place(x=keys_lf.winfo_x() + keys_lf.winfo_width() + PADDING, y=keys_lf.winfo_y())

root_folder_path_label = Label(master=root_folder_lf, textvariable=dp_root_folder_display, foreground='red')
root_folder_path_label.pack()
root_folder_path_label.place(x=90, y=0)

root.mainloop()
