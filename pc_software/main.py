import os
import sys
import time
import random
import duck_objs
from tkinter import *
from tkinter import filedialog
from tkinter import ttk
from tkinter.colorchooser import askcolor

MAIN_WINDOW_WIDTH = 800
MAIN_WINDOW_HEIGHT = 533
PADDING = 10
HIGHT_ROOT_FOLDER_LF = 50
INVALID_ROOT_FOLDER_STRING = "<-- Please select your duckyPad root folder"

def select_root_folder():
    global profile_list
    dir_result = filedialog.askdirectory()
    if len(dir_result) <= 0:
        return
    dp_root_folder_path= dir_result
    dp_root_folder_display.set("Selected: " + dir_result)
    root_folder_path_label.config(foreground='navy')

    profile_list = duck_objs.build_profile(dir_result)
    profile_var.set([' '+x.name for x in profile_list])
    enable_buttons()

def enable_buttons():
    profile_add_button.config(state='normal')
    profile_remove_button.config(state='normal')
    profile_rename_button.config(state='normal')
    profile_up_button.config(state='normal')
    profile_down_button.config(state='normal')
    profile_dupe_button.config(state='normal')

def debug_set_root_folder():
    global profile_list
    dir_result = "/Users/allen/Desktop/sample_profiles"
    dp_root_folder_path= dir_result
    dp_root_folder_display.set("Selected: " + dir_result)
    root_folder_path_label.config(foreground='navy')

    profile_list = duck_objs.build_profile(dir_result)
    profile_var.set([' '+x.name for x in profile_list])
    enable_buttons()

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
    print("curselection:", profile_listbox.curselection())

def profile_shift_down():
    global profile_var
    global profile_list
    selection = profile_listbox.curselection()
    if len(selection) <= 0 or selection[0] == len(profile_list) - 1:
        print("profile_shift_down: nothing to do")
        return
    source = selection[0]
    destination = selection[0] + 1
    profile_list[destination], profile_list[source] = profile_list[source], profile_list[destination]
    profile_var.set([' '+x.name for x in profile_list])
    profile_listbox.selection_clear(0, len(profile_list))
    profile_listbox.selection_set(destination)
    print("curselection:", profile_listbox.curselection())

def rgb_to_hex(rgb_tuple):
    return '#%02x%02x%02x' % rgb_tuple

def update_profile_display(index):
    bg_color_hex = "#abcdef"
    if profile_list[index].bg_color is not None:
        bg_color_hex = rgb_to_hex(profile_list[index].bg_color)
    bg_color_button.config(highlightbackground=bg_color_hex)
    
    kd_color_hex = "#abcdef"
    if profile_list[index].kd_color is not None:
        kd_color_hex = rgb_to_hex(profile_list[index].kd_color)
    kd_color_button.config(highlightbackground=kd_color_hex)

def on_profile_listbox_select(event):
    update_profile_display(int(event.widget.curselection()[0]))
    
def bg_color_button_click():
    print("does it still work?")

def kd_color_sel():
    print("hello world")

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

root_folder_path_label = Label(master=root_folder_lf, textvariable=dp_root_folder_display, foreground='red')
root_folder_path_label.pack()
root_folder_path_label.place(x=90, y=0)

# ------------- Profiles frame -------------

profile_var = StringVar()
profiles_lf = LabelFrame(root, text="Profiles", width=int(MAIN_WINDOW_WIDTH / 3 - PADDING * 1.3), height=MAIN_WINDOW_HEIGHT - HIGHT_ROOT_FOLDER_LF - PADDING)
profiles_lf.pack()
profiles_lf.place(x=PADDING, y=HIGHT_ROOT_FOLDER_LF)
profiles_lf.pack_propagate(False)
root.update()

profile_listbox = Listbox(profiles_lf, listvariable=profile_var, height=16) #, selectmode='single'?
profile_listbox.pack()
profile_listbox.place(x=32, y=PADDING, width=182)
profile_listbox.bind('<<ListboxSelect>>', on_profile_listbox_select)

profile_up_button = Button(profiles_lf, text="↑", command=profile_shift_up, state=DISABLED)
profile_up_button.pack()
profile_up_button.place(x=5, y=80, width=20, height=40)

profile_down_button = Button(profiles_lf, text="↓", command=profile_shift_down, state=DISABLED)
profile_down_button.pack()
profile_down_button.place(x=5, y=140, width=20, height=40)

BUTTON_WIDTH = int(profiles_lf.winfo_width() / 2.5)
BUTTON_HEIGHT = 25
BUTTON_Y_POS = 295

profile_add_button = Button(profiles_lf, text="Add", command=None, state=DISABLED)
profile_add_button.pack()
profile_add_button.place(x=PADDING*2, y=BUTTON_Y_POS, width=BUTTON_WIDTH, height=BUTTON_HEIGHT)

profile_dupe_button = Button(profiles_lf, text="Duplicate", command=None, state=DISABLED)
profile_dupe_button.pack()
profile_dupe_button.place(x=PADDING * 2.5 + BUTTON_WIDTH, y=BUTTON_Y_POS, width=BUTTON_WIDTH, height=BUTTON_HEIGHT)

profile_remove_button = Button(profiles_lf, text="Remove", command=None, state=DISABLED)
profile_remove_button.pack()
profile_remove_button.place(x=PADDING * 2, y=BUTTON_Y_POS + BUTTON_HEIGHT + int(PADDING/2), width=BUTTON_WIDTH, height=BUTTON_HEIGHT)

profile_rename_button = Button(profiles_lf, text="Rename", command=None, state=DISABLED)
profile_rename_button.pack()
profile_rename_button.place(x=PADDING * 2.5 + BUTTON_WIDTH, y=BUTTON_Y_POS + BUTTON_HEIGHT + int(PADDING/2), width=BUTTON_WIDTH, height=BUTTON_HEIGHT)

bg_color_label = Label(master=profiles_lf, text="Backgroud color:")
bg_color_label.pack()
bg_color_label.place(x=20, y=355)

bg_color_button = Button(profiles_lf, command=bg_color_button_click, highlightthickness=20)
bg_color_button.pack()
bg_color_button.place(x=155, y=356, width=60, height=20)

kd_color_label = Label(master=profiles_lf, text="Key-down color:")
kd_color_label.pack()
kd_color_label.place(x=20, y=380)

kd_color_button = Button(profiles_lf, highlightthickness=20, command=None)
kd_color_button.pack()
kd_color_button.place(x=155, y=402, width=60, height=20)

dim_unused_keys_checkbox = Checkbutton(profiles_lf, text="Dim unused keys", variable=None)
dim_unused_keys_checkbox.pack()
dim_unused_keys_checkbox.place(x=22, y=425)

kd_color_var = IntVar()
kd_R1 = Radiobutton(profiles_lf, text="    Auto", variable=kd_color_var, value=1, command=kd_color_sel)
kd_R1.pack()
kd_R1.place(x=130, y=380)
kd_R2 = Radiobutton(profiles_lf, text="", variable=kd_color_var, value=2, command=kd_color_sel)
kd_R2.pack()
kd_R2.place(x=130, y=400)

test_label = Label(master=profiles_lf, text='test', foreground='white', background='red')
test_label.pack()
test_label.place(x=20, y=400)

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

# --------------------------

debug_set_root_folder()
# askcolor()
root.mainloop()
