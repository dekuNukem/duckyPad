import os
import sys
import time
import random
import duck_objs
from tkinter import *
from tkinter import filedialog

MAIN_WINDOW_WIDTH = 800
MAIN_WINDOW_HEIGHT = 533
PADDING = 10
HIGHT_ROOT_FOLDER_LF = 50
INVALID_ROOT_FOLDER_STRING = "<-- Please select your duckyPad root folder"

def select_root_folder():
	global dp_root_folder_display
	global root_folder_path_label
	global profile_var
	dir_result = filedialog.askdirectory()
	if len(dir_result) <= 0:
		return
	dp_root_folder_path= dir_result
	dp_root_folder_display.set("Selected: " + dir_result)
	root_folder_path_label.config(foreground='navy')

	profile_list = duck_objs.build_profile(dir_result)
	profile_var.set([' '+x.name for x in profile_list])

def debug_set_root_folder():
	global dp_root_folder_display
	global root_folder_path_label
	global profile_var
	global profile_list
	dir_result = "/Users/allen/my_local/duckyPad/sample_profiles"
	dp_root_folder_path= dir_result
	dp_root_folder_display.set("Selected: " + dir_result)
	root_folder_path_label.config(foreground='navy')

	profile_list = duck_objs.build_profile(dir_result)
	profile_var.set([' '+x.name for x in profile_list])

def profile_shift_up():
	global profile_var
	global profile_list
	selection = profile_listbox.curselection()
	if len(selection) <= 0 or selection[0] == 0:
		print("profile_shift_up: nothing to do")
		return
	source = selection[0]
	destination = selection[0] - 1
	profile_list[destination], profile_list[source] = profile_list[source], profile_list[destination]
	profile_var.set([' '+x.name for x in profile_list])
	profile_listbox.selection_clear(0, len(profile_list))
	profile_listbox.selection_set(destination)

def profile_shift_down():
	global profile_var
	global profile_list
	selection = profile_listbox.curselection()
	if len(selection) <= 0 or selection[0] == len(profile_list) - 1:
		print("profile_shift_up: nothing to do")
		return
	source = selection[0]
	destination = selection[0] + 1
	profile_list[destination], profile_list[source] = profile_list[source], profile_list[destination]
	profile_var.set([' '+x.name for x in profile_list])
	profile_listbox.selection_clear(0, len(profile_list))
	profile_listbox.selection_set(destination)

root = Tk()
root.title("duckyPad configurator")
root.geometry(str(MAIN_WINDOW_WIDTH) + "x" + str(MAIN_WINDOW_HEIGHT))
root.resizable(width=FALSE, height=FALSE)
profile_list = []

# ------------- Folder select -------------
dp_root_folder_display = StringVar()
dp_root_folder_path= ''
dp_root_folder_display.set(INVALID_ROOT_FOLDER_STRING)

root_folder_lf = LabelFrame(root, text="duckyPad Folder", width=MAIN_WINDOW_WIDTH - PADDING*2, height=HIGHT_ROOT_FOLDER_LF)
root_folder_lf.pack()
root_folder_lf.place(x=PADDING, y=0)
root_folder_lf.pack_propagate(False) 
root.update()

root_folder_select_button = Button(root_folder_lf, text="Select...", command=select_root_folder, width='8')
root_folder_select_button.pack()
root_folder_select_button.place(x=PADDING, y=0)

# ------------- Profiles frame -------------
profile_var = StringVar()
profiles_lf = LabelFrame(root, text="Profiles", width=int(MAIN_WINDOW_WIDTH / 3 - PADDING * 1.3), height=MAIN_WINDOW_HEIGHT - HIGHT_ROOT_FOLDER_LF - PADDING)
profiles_lf.pack()
profiles_lf.place(x=PADDING, y=HIGHT_ROOT_FOLDER_LF)
profiles_lf.pack_propagate(False)
root.update()

profile_listbox = Listbox(profiles_lf, listvariable=profile_var, height=16)
profile_listbox.pack()

profile_listbox.place(x=32, y=PADDING)


profile_up_button = Button(profiles_lf, text="↑", command=profile_shift_up)
profile_up_button.pack()
profile_up_button.place(x=5, y=80, width=20, height=40)

profile_up_button = Button(profiles_lf, text="↓", command=profile_shift_down)
profile_up_button.pack()
profile_up_button.place(x=5, y=140, width=20, height=40)

BUTTON_WIDTH = int(profiles_lf.winfo_width() / 3.5)
BUTTON_HEIGHT = 25
BUTTON_Y_POS = 290

profile_add_button = Button(profiles_lf, text="Add", command=None)
profile_add_button.pack()
profile_add_button.place(x=PADDING, y=BUTTON_Y_POS, width=BUTTON_WIDTH, height=BUTTON_HEIGHT)

profile_add_button = Button(profiles_lf, text="Remove", command=None)
profile_add_button.pack()
profile_add_button.place(x=PADDING * 1.5 + BUTTON_WIDTH, y=BUTTON_Y_POS, width=BUTTON_WIDTH, height=BUTTON_HEIGHT)

profile_add_button = Button(profiles_lf, text="Rename", command=None)
profile_add_button.pack()
profile_add_button.place(x=PADDING * 2 + BUTTON_WIDTH * 2, y=BUTTON_Y_POS, width=BUTTON_WIDTH, height=BUTTON_HEIGHT)

# ------------- Keys frame -------------
keys_lf = LabelFrame(root, text="Keys", width=int(MAIN_WINDOW_WIDTH / 3 - PADDING * 1.3), height=MAIN_WINDOW_HEIGHT - HIGHT_ROOT_FOLDER_LF - PADDING)
keys_lf.pack()
keys_lf.place(x=profiles_lf.winfo_x() + profiles_lf.winfo_width() + PADDING, y=profiles_lf.winfo_y())
keys_lf.pack_propagate(False) 
root.update()

# ------------- Scripts frame -------------
scripts_lf = LabelFrame(root, text="Scripts", width=int(MAIN_WINDOW_WIDTH / 3 - PADDING * 1.3), height=MAIN_WINDOW_HEIGHT - HIGHT_ROOT_FOLDER_LF - PADDING)
scripts_lf.pack()
scripts_lf.place(x=keys_lf.winfo_x() + keys_lf.winfo_width() + PADDING, y=keys_lf.winfo_y())

root_folder_path_label = Label(master=root_folder_lf, textvariable=dp_root_folder_display, foreground='red')
root_folder_path_label.pack()
root_folder_path_label.place(x=90, y=0)

# --------------------------
try:
	debug_set_root_folder()
except Exception as e:
	print(e)
root.mainloop()
