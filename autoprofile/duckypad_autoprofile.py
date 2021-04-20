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
        # disable_buttons()
        print(traceback.format_exc())
        return
    connection_info_str.set(f"duckyPad found!      Model: {result['model']}      Serial: {result['serial']}      Firmware: {result['fw_ver']}")
    connection_info_label.config(foreground='navy')
    enable_buttons()

def update_windows(textbox):
    windows_str = 'Application' + ' '*9 + "Window Title\n"
    windows_str += "------------------------------------\n"
    for item in get_window.get_list_of_all_windows():
        gap = 20 - len(item[0])
        windows_str += str(item[0]) + ' '*gap + str(item[1]) + '\n'
    textbox.config(state=NORMAL)
    textbox.delete(1.0, "end")
    textbox.insert(1.0, windows_str)
    textbox.config(state=DISABLED)

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

current_app_name_var = StringVar()
current_app_name_var.set("Current app name:")

current_window_title_var = StringVar()
current_window_title_var.set("Current Window Title:")

is_rule_window_open = False

def update_current_app_and_title():
    root.after(500, update_current_app_and_title)
    window_id, app_name, window_title = get_window.get_active_window()
    current_app_name_var.set("App name:      " + str(app_name))
    current_window_title_var.set("Window title:  " + str(window_title))
# ----------------

def on_closing(window, rule):
    print("bye!")
    window.destroy()

def create_rule_window():
    rule_window = Toplevel(root)
    rule_window.title("Edit rules")
    rule_window.geometry("640x480")
    rule_window.resizable(width=FALSE, height=FALSE)
    rule_window.grab_set()
    rule_window.protocol("WM_DELETE_WINDOW", lambda:on_closing(rule_window, None))

    rule_edit_lf = LabelFrame(rule_window, text="Rules", width=620, height=100)
    rule_edit_lf.place(x=10, y=5)

    app_name_label = Label(master=rule_window, text="IF app name contains:")
    app_name_label.place(x=20, y=25)
    app_name_entrybox = Entry(rule_window)
    app_name_entrybox.place(x=230, y=25, width=200)
    # app_name_entrybox.bind('<Return>', key_name_entrybox_return_pressed)

    window_name_label = Label(master=rule_window, text="AND window title contains:")
    window_name_label.place(x=20, y=50)
    window_name_entrybox = Entry(rule_window)
    window_name_entrybox.place(x=230, y=50, width=200)

    switch_to_label = Label(master=rule_window, text="THEN switch to profile #")
    switch_to_label.place(x=20, y=75)
    switch_to_entrybox = Entry(rule_window)
    switch_to_entrybox.place(x=230, y=75, width=200)

    match_all_label = Label(master=rule_window, text="(leave blank to match all)")
    match_all_label.place(x=450, y=25)
    match_all_label2 = Label(master=rule_window, text="(leave blank to match all)")
    match_all_label2.place(x=450, y=50)

    current_window_lf = LabelFrame(rule_window, text="Active window", width=620, height=80)
    current_window_lf.place(x=10, y=110)

    current_app_name_label = Label(master=current_window_lf, textvariable=current_app_name_var, font='TkFixedFont')
    current_app_name_label.place(x=10, y=5)
    current_window_title_label = Label(master=current_window_lf, textvariable=current_window_title_var, font='TkFixedFont')
    current_window_title_label.place(x=10, y=30)

    window_list_lf = LabelFrame(rule_window, text="All windows", width=620, height=270)
    window_list_lf.place(x=PADDING, y=195) 
    window_list_fresh_button = Button(window_list_lf, text="Refresh", command=lambda:update_windows(windows_list_text_area))
    window_list_fresh_button.config(width=80, height=1)
    window_list_fresh_button.place(x=20, y=220)
    windows_list_text_area = ScrolledText.ScrolledText(window_list_lf, wrap='none', width = 73, height = 13)
    windows_list_text_area.place(x=5, y=5)
    root.update()
    update_windows(windows_list_text_area)


def on_profile_lstbox_select(event):
    print('hi there!')

def new_rule_click():
    print('new_rule_click')
    create_rule_window()

def delete_rule_click():
    print('delete_rule_click')

rules_lf = LabelFrame(root, text="Autoswitch rules", width=620, height=300)
rules_lf.place(x=PADDING, y=120) 

profile_var = StringVar()
profile_lstbox = Listbox(rules_lf, listvariable=profile_var, height=16, exportselection=0)
profile_lstbox.place(x=PADDING, y=PADDING, width=500, height=260)
profile_lstbox.bind('<<ListboxSelect>>', on_profile_lstbox_select)

new_rule_button = Button(rules_lf, text="New rule...", command=new_rule_click)
new_rule_button.place(x=520, y=10)
delete_rule_button = Button(rules_lf, text="Delete rule", command=delete_rule_click)
delete_rule_button.place(x=520, y=50)

# ------------------

new_rule_click()

root.after(500, find_duckypad)
root.after(500, update_current_app_and_title)
root.mainloop()
