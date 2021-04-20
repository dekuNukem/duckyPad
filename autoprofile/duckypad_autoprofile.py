import time
import hid_rw
import hid_helper
import get_window
from tkinter import *
from tkinter import filedialog
from tkinter import simpledialog
from tkinter import messagebox
import urllib.request
import tkinter.scrolledtext as ScrolledText
import traceback

THIS_VERSION_NUMBER = '0.1.0'
MAIN_WINDOW_WIDTH = 640
MAIN_WINDOW_HEIGHT = 640
PADDING = 10

def find_duckypad():
    root.after(500, find_duckypad)
    try:
        result = hid_helper.duckypad_get_info()
    except Exception as e:
        connection_info_str.set("Looking for duckyPad...")
        connection_info_label.config(foreground='red')
        disable_buttons()
        print(traceback.format_exc())
        return
    connection_info_str.set(f"duckyPad found!      Model: {result['model']}      Serial: {result['serial']}      Firmware: {result['fw_ver']}")
    connection_info_label.config(foreground='navy')
    enable_buttons()

def update_windows():
    windows_str = 'Application' + ' '*9 + "Window Title\n"
    windows_str += "------------------------------------\n"
    for item in get_window.get_list_of_all_windows():
        gap = 20 - len(item[0])
        windows_str += str(item[0]) + ' '*gap + str(item[1]) + '\n'
    windows_list_text_area.config(state=NORMAL)
    windows_list_text_area.delete(1.0, "end")
    windows_list_text_area.insert(1.0, windows_str)
    windows_list_text_area.config(state=DISABLED)

def prev_prof_click():
    buffff = [0] * 64
    buffff[0] = 5
    buffff[2] = 2
    hid_rw.duckypad_hid_write(buffff)

def next_prof_click():
    buffff = [0] * 64
    buffff[0] = 5
    buffff[2] = 3
    hid_rw.duckypad_hid_write(buffff)

def enable_buttons():
    prev_profile_button.config(state=NORMAL)
    next_profile_button.config(state=NORMAL)

def disable_buttons():
    prev_profile_button.config(state=DISABLED)
    next_profile_button.config(state=DISABLED)

root = Tk()
root.title("duckyPad profile auto switcher " + THIS_VERSION_NUMBER)
root.geometry(str(MAIN_WINDOW_WIDTH) + "x" + str(MAIN_WINDOW_HEIGHT))
root.resizable(width=FALSE, height=FALSE)

# --------------------

connection_info_str = StringVar()
connection_info_lf = LabelFrame(root, text="Connection", width=620, height=60)
connection_info_lf.place(x=PADDING, y=0) 
connection_info_label = Label(master=connection_info_lf, textvariable=connection_info_str)
connection_info_label.place(x=PADDING, y=5)


# --------------------

dashboard_lf = LabelFrame(root, text="Dashboard", width=620, height=60)
dashboard_lf.place(x=PADDING, y=60) 
prev_profile_button = Button(dashboard_lf, text="Prev Profile", command=prev_prof_click, state=DISABLED)
prev_profile_button.place(x=PADDING, y=5)
next_profile_button = Button(dashboard_lf, text="Next Profile", command=next_prof_click, state=DISABLED)
next_profile_button.place(x=100, y=5)

# --------------------

window_list_lf = LabelFrame(root, text="Window list", width=620, height=240)
window_list_lf.place(x=PADDING, y=120) 
window_list_fresh_button = Button(window_list_lf, text="Refresh", command=update_windows)
window_list_fresh_button.config(width=80, height=1)
window_list_fresh_button.place(x=20, y=190)
windows_list_text_area = ScrolledText.ScrolledText(window_list_lf, wrap='none', width = 73, height = 11)
windows_list_text_area.place(x=5, y=5)
root.update()
update_windows()

# ----------------

root.after(500, find_duckypad)
root.mainloop()
