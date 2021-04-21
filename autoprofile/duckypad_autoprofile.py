import time
import hid_rw
import get_window
from tkinter import *
from tkinter import filedialog
from tkinter import simpledialog
from tkinter import messagebox
import urllib.request
import tkinter.scrolledtext as ScrolledText
import traceback
import json
import os
import webbrowser

THIS_VERSION_NUMBER = '0.1.0'
MAIN_WINDOW_WIDTH = 640
MAIN_WINDOW_HEIGHT = 640
PADDING = 10

def find_duckypad():
    root.after(500, find_duckypad)
    if hid_rw.get_duckypad_path() is None:
        connection_info_str.set("Looking for duckyPad...")
        connection_info_label.config(foreground='red')
        disable_buttons()
        return

    connection_info_str.set("duckyPad found!")
    connection_info_label.config(foreground='navy')
    enable_buttons()

    try:
        result = hid_rw.duckypad_get_info()
        connection_info_str.set(f"duckyPad found!      Model: {result['model']}      Serial: {result['serial']}      Firmware: {result['fw_ver']}")
        connection_info_label.config(foreground='navy')
    except Exception as e:
        # print(traceback.format_exc())
        return
    
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

discord_link_url = "https://raw.githubusercontent.com/dekuNukem/duckyPad/master/resources/discord_link.txt"

def open_user_manual():
    webbrowser.open('https://github.com/dekuNukem/duckyPad/blob/master/getting_started.md')

def open_discord():
    try:
        webbrowser.open(str(urllib.request.urlopen(discord_link_url).read().decode('utf-8')).split('\n')[0])
    except Exception as e:
        messagebox.showerror("Error", "Failed to open discord link!\n"+str(e))

dashboard_lf = LabelFrame(root, text="Dashboard", width=620, height=60)
dashboard_lf.place(x=PADDING, y=60) 
prev_profile_button = Button(dashboard_lf, text="Prev Profile", command=prev_prof_click)
prev_profile_button.config(width=11, height=1)
prev_profile_button.place(x=PADDING, y=5)
next_profile_button = Button(dashboard_lf, text="Next Profile", command=next_prof_click)
next_profile_button.config(width=11, height=1)
next_profile_button.place(x=110, y=5)

user_manual_button = Button(dashboard_lf, text="User Manual", command=open_user_manual)
user_manual_button.config(width=11, height=1)
user_manual_button.place(x=210, y=5)

discord_button = Button(dashboard_lf, text="Discord", command=open_discord)
discord_button.config(width=11, height=1)
discord_button.place(x=310, y=5)

# --------------------

current_app_name_var = StringVar()
current_app_name_var.set("Current app name:")

current_window_title_var = StringVar()
current_window_title_var.set("Current Window Title:")

last_hid_profile = None

def duckypad_goto_profile(profile_number):
    global last_hid_profile
    if not 1 <= profile_number <= 31:
        return
    if profile_number != last_hid_profile:
        buffff = [0] * 64
        buffff[0] = 5
        buffff[2] = 1
        buffff[3] = profile_number
        hid_rw.duckypad_hid_write(buffff)
        last_hid_profile = profile_number

def update_current_app_and_title():
    root.after(250, update_current_app_and_title)

    window_id, app_name, window_title = get_window.get_active_window()
    current_app_name_var.set("App name:      " + str(app_name))
    current_window_title_var.set("Window title:  " + str(window_title))

    if rule_window is not None and rule_window.winfo_exists():
        return

    highlight_index = None
    for index, item in enumerate(autoswitch_rules_list):
        if item['enabled'] is False:
            continue
        app_name_condition = True
        if len(item['app_name']) > 0:
            app_name_condition = item['app_name'].lower() in app_name.lower()
        window_title_condition = True
        if len(item['window_title']) > 0:
            window_title_condition = item['window_title'].lower() in window_title.lower()
        # print(item)
        # print(app_name_condition, window_title_condition)
        if app_name_condition and window_title_condition:
            duckypad_goto_profile(int(item['switch_to']))
            highlight_index = index
            break

    for index, item in enumerate(autoswitch_rules_list):
        if index == highlight_index:
            profile_lstbox.itemconfig(index, fg='white', bg='green')
        else:
            profile_lstbox.itemconfig(index, fg='black', bg='white')
    
    # print('----------------')

# ----------------

app_name_entrybox = None
window_name_entrybox = None
switch_to_entrybox = None
autoswitch_rules_list = []

def clean_input(str_input):
    return str_input.strip()

def check_profile_number(raw_str):
    try:
        profile_number = int(clean_input(raw_str))
    except Exception:
        return None
    if 1 <= profile_number <= 31:
        return profile_number
    return None

def make_rule_str(rule_dict):
    rule_str = ''
    if rule_dict['enabled']:
        rule_str += "  * "
    else:
        rule_str += "    "
    if len(rule_dict['app_name']) > 0:
        rule_str += "     " + rule_dict['app_name']
    else:
        rule_str += "     " + "[Any]"

    next_item = rule_dict['window_title']
    if len(next_item) <= 0:
        next_item = "[Any]"
    gap = 29 - len(rule_str)
    rule_str += ' '*gap + next_item

    gap = 59 - len(rule_str)
    rule_str += ' '*gap + str(rule_dict['switch_to'])

    return rule_str

def update_rule_list_display():
    profile_var.set([make_rule_str(x) for x in autoswitch_rules_list])

def ensure_dir(dir_path):
    if not os.path.exists(dir_path):
        os.makedirs(dir_path)

save_path = os.path.join(os.getenv('APPDATA'), 'dekuNukem')
save_path = os.path.join(save_path, 'duckypad_autoswitcher')
save_filename = os.path.join(save_path, 'config.txt')

def save_config():
    try:
        ensure_dir(save_path)
        with open(save_filename, 'w', encoding='utf8') as save_file:
                save_file.write(json.dumps(autoswitch_rules_list, sort_keys=True))
    except Exception as e:
        messagebox.showerror("Error", "Save failed!\n\n"+str(traceback.format_exc()))

def save_rule_click(window, this_rule):
    if this_rule is None:
        rule_dict = {}
        rule_dict["app_name"] = clean_input(app_name_entrybox.get())
        rule_dict["window_title"] = clean_input(window_name_entrybox.get())
        rule_dict["switch_to"] = check_profile_number(switch_to_entrybox.get())
        rule_dict["enabled"] = True
        if rule_dict["switch_to"] is not None and rule_dict not in autoswitch_rules_list:
            autoswitch_rules_list.append(rule_dict)
            update_rule_list_display()
            save_config()
            window.destroy()
    elif this_rule is not None and check_profile_number(switch_to_entrybox.get()) is not None:
        this_rule["app_name"] = clean_input(app_name_entrybox.get())
        this_rule["window_title"] = clean_input(window_name_entrybox.get())
        this_rule["switch_to"] = check_profile_number(switch_to_entrybox.get())
        update_rule_list_display()
        save_config()
        window.destroy()

rule_window = None

def create_rule_window(existing_rule=None):
    global rule_window
    global app_name_entrybox
    global window_name_entrybox
    global switch_to_entrybox
    rule_window = Toplevel(root)
    rule_window.title("Edit rules")
    rule_window.geometry("640x480")
    rule_window.resizable(width=FALSE, height=FALSE)
    rule_window.grab_set()

    rule_edit_lf = LabelFrame(rule_window, text="Rules", width=620, height=100)
    rule_edit_lf.place(x=10, y=5)

    app_name_label = Label(master=rule_window, text="IF app name contains:")
    app_name_label.place(x=20, y=25)
    app_name_entrybox = Entry(rule_window)
    app_name_entrybox.place(x=230, y=25, width=200)
    
    window_name_label = Label(master=rule_window, text="AND window title contains:")
    window_name_label.place(x=20, y=50)
    window_name_entrybox = Entry(rule_window)
    window_name_entrybox.place(x=230, y=50, width=200)

    switch_to_label = Label(master=rule_window, text="THEN switch to profile #")
    switch_to_label.place(x=20, y=75)
    switch_to_entrybox = Entry(rule_window)
    switch_to_entrybox.place(x=230, y=75, width=200)

    if existing_rule is not None:
        app_name_entrybox.insert(0, existing_rule["app_name"])
        window_name_entrybox.insert(0, existing_rule["window_title"])
        switch_to_entrybox.insert(0, existing_rule["switch_to"])

    rule_done_button = Button(rule_edit_lf, text="Save", command=lambda:save_rule_click(rule_window, existing_rule))
    rule_done_button.config(width=18, height=1)
    rule_done_button.place(x=440, y=50)

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

# def on_profile_lstbox_select(event):
#     print('hi there!')

# def new_rule_click():
#     create_rule_window()

def delete_rule_click():
    selection = profile_lstbox.curselection()
    if len(selection) <= 0:
        return
    autoswitch_rules_list.pop(selection[0])
    update_rule_list_display()
    save_config()

def edit_rule_click():
    selection = profile_lstbox.curselection()
    if len(selection) <= 0:
        return
    create_rule_window(autoswitch_rules_list[selection[0]])

def toggle_rule_click():
    selection = profile_lstbox.curselection()
    if len(selection) <= 0:
        return
    autoswitch_rules_list[selection[0]]['enabled'] = not autoswitch_rules_list[selection[0]]['enabled']
    update_rule_list_display()
    save_config()

def rule_shift_up():
    selection = profile_lstbox.curselection()
    if len(selection) <= 0 or selection[0] == 0:
        return
    source = selection[0]
    destination = selection[0] - 1
    autoswitch_rules_list[destination], autoswitch_rules_list[source] = autoswitch_rules_list[source], autoswitch_rules_list[destination]
    update_rule_list_display()
    profile_lstbox.selection_clear(0, len(autoswitch_rules_list))
    profile_lstbox.selection_set(destination)
    update_rule_list_display()

def rule_shift_down():
    selection = profile_lstbox.curselection()
    if len(selection) <= 0 or selection[0] == len(autoswitch_rules_list) - 1:
        return
    source = selection[0]
    destination = selection[0] + 1
    autoswitch_rules_list[destination], autoswitch_rules_list[source] = autoswitch_rules_list[source], autoswitch_rules_list[destination]
    update_rule_list_display()
    profile_lstbox.selection_clear(0, len(autoswitch_rules_list))
    profile_lstbox.selection_set(destination)
    update_rule_list_display()

rules_lf = LabelFrame(root, text="Autoswitch rules", width=620, height=410)
rules_lf.place(x=PADDING, y=120) 

profile_var = StringVar()
profile_lstbox = Listbox(rules_lf, listvariable=profile_var, height=20, exportselection=0)
profile_lstbox.place(x=PADDING, y=30, width=500)
profile_lstbox.config(font='TkFixedFont')
# profile_lstbox.bind('<<ListboxSelect>>', on_profile_lstbox_select)
profile_lstbox.bind('<FocusOut>', lambda e: profile_lstbox.selection_clear(0, END))

rule_header_label = Label(master=rules_lf, text="Enabled   App name          Window Title                Profile", font='TkFixedFont')
rule_header_label.place(x=5, y=5)

new_rule_button = Button(rules_lf, text="New rule...", command=create_rule_window)
new_rule_button.config(width=11, height=1)
new_rule_button.place(x=520, y=30)

edit_rule_button = Button(rules_lf, text="Edit rule...", command=edit_rule_click)
edit_rule_button.config(width=11, height=1)
edit_rule_button.place(x=520, y=70)

move_up_button = Button(rules_lf, text="Move up", command=rule_shift_up)
move_up_button.config(width=11, height=1)
move_up_button.place(x=520, y=150)

toggle_rule_button = Button(rules_lf, text="On/Off", command=toggle_rule_click)
toggle_rule_button.config(width=11, height=1)
toggle_rule_button.place(x=520, y=190)

move_down_button = Button(rules_lf, text="Move down", command=rule_shift_down)
move_down_button.config(width=11, height=1)
move_down_button.place(x=520, y=230)

delete_rule_button = Button(rules_lf, text="Delete rule", command=delete_rule_click)
delete_rule_button.config(width=11, height=1)
delete_rule_button.place(x=520, y=300)

try:
    with open(save_filename) as json_file:
        autoswitch_rules_list = json.load(json_file)
    update_rule_list_display()
except Exception as e:
    print(e)

# ------------------

updates_lf = LabelFrame(root, text="Updates", width=620, height=80)
updates_lf.place(x=PADDING, y=530)

# ------------------

root.after(500, find_duckypad)
root.after(500, update_current_app_and_title)
root.mainloop()
