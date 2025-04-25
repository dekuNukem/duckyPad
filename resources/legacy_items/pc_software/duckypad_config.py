import os
import time
import copy
import shutil
from datetime import datetime
import traceback
import duck_objs
import webbrowser
import check_update
from tkinter import *
from tkinter import filedialog
from tkinter import simpledialog
from tkinter.colorchooser import askcolor
from tkinter import messagebox
import urllib.request
from appdirs import *
import json
import subprocess
import hid_op
import threading
import ds3_preprocessor
import make_bytecode

"""
0.13.5
changed old HOLD to EMUK command

0.13.6
Added japanese IME keys "KATAKANAHIRAGANA", "HENKAN", "MUHENKAN", "KATAKANA", "HIRAGANA", "ZENKAKUHANKAKU"

0.14.0
added EMUK replacement
only show last 50 characters when deleting folders
fixed syntax check bug where MMOUSE isnt recognized
script checking now provides error details
defaultdelay and defaultchardelay now resets correctly when running a new script in pc test-run
ask to confirm if trying to quit while saving data

0.15.0 2023 01 23
working on adding DSB support

1.0.0 2023 02 06
duckyScript 3 public beta

1.0.1 2023 02 13
minor bug fixes
better handles saving when code contains errors
removed unused code
fixed a function detection bug

1.1.0 2023 02 14
updated dsb binary format to allow banked loading
cleaned up opcode values

1.1.1 2023 02 16
changed loop break command to LBREAK to avoid conflict with BREAK keyboard key

1.2.0 2023 02 17
added profile import function
script display now hidden if no key is selected
automatically selects first profile after loading
added color to key apply and remove button
added minimum firmware version check

1.2.1 2023 02 21
added hid busy check

1.2.2 2023 03 01
fixed HID busy detection bug

1.3.0 2023 05 02
Fixed a firmware version parse bug
getting ready for public release
added firmware version compatibility check with upper and lower bound, both HID and file based.

1.3.5 2023 05 12
fixed a bug where it tries to load junk macOS files
added back COMMAND key 

1.4.0 2023 07 01
added _TIME_S read-only variable
Updated colour pickers to provide an appropriate initial colour and title for the dialog window. (PR#135)

1.4.1 2023 07 01
Fixed a crash when typing EMUK command too slowly

1.4.2 2023 09 12
added REM_BLOCK and REM_END

1.5.0 2023 09 18
added STRINGLN_BLOCK and STRINGLN_END
added STRING_BLOCK and STRING_END
adjust INJECT_MOD behaviour

1.5.1 2023 09 20
STRINGLN_BLOCK and STRING_BLOCK now preserves empty lines and white spaces

1.6.1 2023 10 10
automatically expands MOUSE_MOVE is value is more than 127
checks if duckypad is busy before trying to connect

1.6.2 2023 11 11
increased max profile to 64

1.6.3 2023 11 30
automatically splits STRING/STRINGLN commands if too long

1.6.4 2024 01 22
Fixed a bug where TRUE and FALSE is replaced with 1 and 0 inside STRING statements

1.6.5 2024 10 29
added exist_ok to makedirs() to suppress exception

"""

THIS_VERSION_NUMBER = '1.6.5'
MIN_DUCKYPAD_FIRMWARE_VERSION = "1.1.2"
MAX_DUCKYPAD_FIRMWARE_VERSION = "1.10.10"

ENV_UI_SCALE = os.getenv("DUCKYPAD_UI_SCALE")
UI_SCALE = float(ENV_UI_SCALE) if ENV_UI_SCALE else 1

def ensure_dir(dir_path):
    if not os.path.exists(dir_path):
        os.makedirs(dir_path, exist_ok=True)

def is_root():
    return os.getuid() == 0

def scaled_size(size: int) -> int:
    return int(size * UI_SCALE)

appname = 'duckypad_config'
appauthor = 'dekuNukem'
save_path = user_data_dir(appname, appauthor, roaming=True)
backup_path = os.path.join(save_path, 'profile_backups')
ensure_dir(save_path)
ensure_dir(backup_path)
save_filename = os.path.join(save_path, 'config.txt')
config_dict = {}
hid_dump_path = os.path.join(save_path, "hid_dump")
hid_modified_dir_path = os.path.join(save_path, "hid_new")

print("This window will print debug information!")
print("Used for troubleshooting if it crashes!")

try:
    with open(save_filename) as json_file:
        temp = json.load(json_file)
        if isinstance(temp, dict):
            config_dict = temp
        else:
            raise ValueError("not a valid config file")
except Exception as e:
    pass

config_dict['auto_backup_enabled'] = True

default_button_color = 'SystemButtonFace'
if 'linux' in sys.platform:
    default_button_color = 'grey'

MAIN_WINDOW_WIDTH = scaled_size(800)
MAIN_WINDOW_HEIGHT = scaled_size(605)
MAIN_COLUMN_HEIGHT = scaled_size(533)
PADDING = scaled_size(10)
HEIGHT_ROOT_FOLDER_LF = scaled_size(50)
INVALID_ROOT_FOLDER_STRING = "<-- Press to connect to duckyPad"
last_rgb = (238,130,238)
dp_settings = duck_objs.dp_global_settings()
discord_link_url = "https://raw.githubusercontent.com/dekuNukem/duckyPad/master/resources/discord_link.txt"

def open_discord_link():
    try:
        webbrowser.open(str(urllib.request.urlopen(discord_link_url).read().decode('latin-1')).split('\n')[0])
    except Exception as e:
        messagebox.showerror("Error", "Failed to open discord link!\n"+str(e))

def create_help_window():
    help_window = Toplevel(root)
    help_window.title("duckyPad help")
    help_window.geometry(str(scaled_size(280)) + "x" + str(scaled_size(180)))
    help_window.resizable(width=FALSE, height=FALSE)
    help_window.grab_set()

    user_manual_label = Label(master=help_window, text="Not sure what to do? Please read...")
    user_manual_label.place(x=scaled_size(35), y=scaled_size(5))
    user_manual_button = Button(help_window, text="User Manual", command=open_duckypad_user_manual_url)
    user_manual_button.place(x=scaled_size(60), y=scaled_size(30), width=scaled_size(160))

    troubleshoot_label = Label(master=help_window, text="Problems? Please see...")
    troubleshoot_label.place(x=scaled_size(35), y=scaled_size(60))
    troubleshoot_button = Button(help_window, text="Troubleshooting Guides", command=open_duckypad_troubleshooting_url)
    troubleshoot_button.place(x=scaled_size(50), y=scaled_size(85), width=scaled_size(180))

    discord_label = Label(master=help_window, text="Questions or comments? Ask in...")
    discord_label.place(x=scaled_size(35), y=scaled_size(60 + 55))
    discord_button = Button(help_window, text="Official Discord Chatroom", command=open_discord_link)
    discord_button.place(x=scaled_size(50), y=scaled_size(85 + 55), width=scaled_size(180))

def open_duckypad_user_manual_url():
    webbrowser.open('https://github.com/dekuNukem/duckyPad/blob/master/getting_started.md')

def open_duckypad_troubleshooting_url():
    webbrowser.open('https://github.com/dekuNukem/duckyPad/blob/master/troubleshooting.md')

def app_update_click(event):
    webbrowser.open('https://github.com/dekuNukem/duckyPad/releases/latest')

def reset_key_button_relief():
    for item in key_button_list:
        item.config(borderwidth=1, relief="solid")

def hex_to_rgb(hex_str):
    hex_str = hex_str.strip('#')
    return tuple(int(hex_str[i:i+2], 16) for i in (0, 2, 4))

def rgb_to_hex(rgb_tuple):
    return '#%02x%02x%02x' % rgb_tuple

def ui_reset():
    global selected_key
    profile_add_button.config(state=DISABLED)
    profile_remove_button.config(state=DISABLED)
    profile_rename_button.config(state=DISABLED)
    profile_up_button.config(state=DISABLED)
    profile_down_button.config(state=DISABLED)
    profile_dupe_button.config(state=DISABLED)
    save_button.config(state=DISABLED)
    kd_R1.config(state=DISABLED)
    kd_R2.config(state=DISABLED)
    dim_unused_keys_checkbox.config(state=DISABLED)
    key_rename_button.config(state=DISABLED)
    key_remove_button.config(state=DISABLED)
    for button in script_command_button_list:
        button.config(state=DISABLED)
    key_name_entrybox.config(state=DISABLED)
    bg_color_label.config(fg='grey')
    kd_color_label.config(fg='grey')
    key_name_text.config(fg='grey')
    key_color_text.config(fg='grey')
    reset_key_button_relief()
    update_key_button_appearances(None)
    key_name_entrybox.delete(0, 'end')
    selected_key = None
    key_color_button.config(background=default_button_color)
    bg_color_button.config(background=default_button_color)
    kd_color_button.config(background=default_button_color)
    key_color_rb1.config(state=DISABLED)
    key_color_rb2.config(state=DISABLED)
    script_textbox.delete(1.0, 'end')
    check_syntax_label.config(text="", fg="green")
    sleepmode_slider.config(state=DISABLED)
    profile_import_button.config(state=DISABLED)

def check_firmware_update(current_fw_str=None):
    if current_fw_str is not None:
        return check_update.get_firmware_update_status(current_fw_str), current_fw_str
    filelist = os.listdir(dp_root_folder_path)
    if 'last_profile.kbd' in filelist and 'dp_stats.txt' not in filelist:
        return 1, None
    if 'dp_stats.txt' in filelist:
        with open(os.path.join(dp_root_folder_path, 'dp_stats.txt')) as dp_stats_file:
            for line in dp_stats_file:
                if line.startswith('fw '):
                    line = line.replace('\n', '').replace('\r', '').replace('fw ', '')
                    return check_update.get_firmware_update_status(line), line
    return 2, None

def fw_update_click(event):
    webbrowser.open('https://github.com/dekuNukem/duckyPad/blob/master/firmware_updates_and_version_history.md')

def print_fw_update_label(current_fw_str=None):
    fw_result, this_version = check_firmware_update(current_fw_str)
    if fw_result == 0:
        dp_fw_update_label.config(text='Firmware (' + str(this_version) +'): Up to date', fg='black', bg=default_button_color)
        dp_fw_update_label.unbind("<Button-1>")
    elif fw_result == 1:
        dp_fw_update_label.config(text='Firmware (' + str(this_version) +'): Update available! Click me!', fg='black', bg='orange', cursor="hand2")
        dp_fw_update_label.bind("<Button-1>", fw_update_click)
    else:
        dp_fw_update_label.config(text='Firmware: Unknown', fg='black', bg=default_button_color)
        dp_fw_update_label.unbind("<Button-1>")
    return this_version

def get_fw_str_hid():
    dp_info = hid_op.get_dp_info()
    if dp_info is None:
        return None
    return f"{dp_info[3]}.{dp_info[4]}.{dp_info[5]}"
    
FW_OK = 0
FW_TOO_LOW = 1
FW_TOO_HIGH = 2
FW_UNKNOWN = 3

def check_fw_support(current_fw_str):
    try:
        if check_update.versiontuple(current_fw_str) < check_update.versiontuple(MIN_DUCKYPAD_FIRMWARE_VERSION):
            return FW_TOO_LOW
        if check_update.versiontuple(current_fw_str) > check_update.versiontuple(MAX_DUCKYPAD_FIRMWARE_VERSION):
            return FW_TOO_HIGH
        return FW_OK
    except Exception as e:
        print('check_fw_support', current_fw_str, e)
        return FW_UNKNOWN

def select_root_folder(root_path=None, check_fw=True):
    global profile_list
    global dp_root_folder_path
    if root_path is None:
        root_path = filedialog.askdirectory()
    if len(root_path) <= 0:
        return
    dp_root_folder_path = root_path
    dp_root_folder_display.set("Selected: " + root_path)
    root_folder_path_label.config(foreground='navy')
    profile_list = duck_objs.build_profile(root_path)
    dp_settings.load_from_path(dp_root_folder_path)

    if check_fw:
        duckypad_fw_ver = print_fw_update_label()
        fw_status = check_fw_support(duckypad_fw_ver)
        if fw_status == FW_TOO_LOW or fw_status == FW_TOO_HIGH:
            incompatible_fw_msgbox(duckypad_fw_ver, fw_status)

    ui_reset()
    update_profile_display()
    enable_buttons()
    try:
        profile_lstbox.select_set(0)
        update_profile_display()
    except Exception as e:
        print("select_root_folder:", e)

HID_NOP = 0
HID_DUMP = 1
HID_SAVE = 2
current_hid_op = HID_NOP
is_using_hid = False

def incompatible_fw_msgbox(current_fw_str, fw_status):
    if fw_status == FW_TOO_LOW:
        if messagebox.askokcancel("Info", f"duckyPad firmware too low!\n\nCurrent: {current_fw_str}\nSupported: Between {MIN_DUCKYPAD_FIRMWARE_VERSION} and {MAX_DUCKYPAD_FIRMWARE_VERSION}.\n\nSee how to update it?"):
            fw_update_click(None)
    elif fw_status == FW_TOO_HIGH:
        if messagebox.askokcancel("Info", f"duckyPad firmware too high!\n\nCurrent: {current_fw_str}\nSupported: Between {MIN_DUCKYPAD_FIRMWARE_VERSION} and {MAX_DUCKYPAD_FIRMWARE_VERSION}.\n\nSee how to update this app?"):
            app_update_click(None)
    else:
        messagebox.showinfo("Info", f"duckyPad firmware unknown!\n\n")

def connect_button_click():
    global current_hid_op
    global is_using_hid

    is_using_hid = False

    if hid_op.get_duckypad_path() is None:
        if(messagebox.askokcancel("Info", "duckyPad not found!\n\nConfigure via SD card instead?") == False):
            return
        select_root_folder()
        return

    init_success = True
    hid_op.duckypad_hid_close()
    try:
        hid_op.duckypad_hid_init()
        is_dp_ready, comment = hid_op.is_dp_ready()
        if is_dp_ready is False:
            messagebox.showerror("Error", comment)
            return
        fw_str = get_fw_str_hid()
        fw_status = check_fw_support(fw_str)
        if fw_status != FW_OK:
            init_success = False
            incompatible_fw_msgbox(fw_str, fw_status)
        print_fw_update_label(fw_str)
    except Exception as e:
        print("connect_button_click 1", e)
        init_success = False

    if init_success:
        current_hid_op = HID_DUMP
        is_using_hid = True
        return

    if init_success is False and 'linux' in sys.platform:
        box_result = messagebox.askyesnocancel("Info", "duckyPad detected, but additional permissions needed.\n\nClick Yes for instructions\n\nClick No to configure via SD card.")
        if box_result is True:
            webbrowser.open('https://github.com/dekuNukem/duckyPad/blob/master/app_posix.md')
        elif box_result is False:
            select_root_folder()
        return

    if init_success is False and 'darwin' in sys.platform and is_root() is False:
        box_result = messagebox.askyesnocancel("Info", "duckyPad detected, but additional permissions needed to access it.\n\nClick Yes for instructions\n\nClick No to configure via SD card.")
        if box_result is True:
            webbrowser.open('https://github.com/dekuNukem/duckyPad/blob/master/troubleshooting.md#usb-configuration--autoswitcher-isnt-working-on-macos')
        elif box_result is False:
            select_root_folder()
        return

    if init_success is False and 'darwin' in sys.platform and is_root() is True:
        box_result = messagebox.askyesnocancel("Info", "duckyPad detected, however, due to macOS restrictions, you need to enable some privacy settings.\n\nClick Yes to learn how.\n\nClick No to configure via SD card.")
        if box_result is True:
            webbrowser.open('https://github.com/dekuNukem/duckyPad/blob/master/troubleshooting.md#usb-configuration--autoswitcher-isnt-working-on-macos')
        elif box_result is False:
            select_root_folder()
        return

def enable_buttons():
    profile_add_button.config(state=NORMAL)
    profile_remove_button.config(state=NORMAL)
    profile_rename_button.config(state=NORMAL)
    profile_up_button.config(state=NORMAL)
    profile_down_button.config(state=NORMAL)
    profile_dupe_button.config(state=NORMAL)
    save_button.config(state=NORMAL)
    backup_button.config(state=NORMAL)
    kd_R1.config(state=NORMAL)
    kd_R2.config(state=NORMAL)
    dim_unused_keys_checkbox.config(state=NORMAL)
    key_rename_button.config(state=NORMAL)
    key_remove_button.config(state=NORMAL)
    for button in script_command_button_list:
        button.config(state=NORMAL)
    key_name_entrybox.config(state=NORMAL)
    bg_color_label.config(fg='black')
    kd_color_label.config(fg='black')
    key_name_text.config(fg='black')
    key_color_text.config(fg='black')
    sleepmode_slider.config(state=NORMAL)
    sleepmode_slider.set(dp_settings.sleep_after_minutes)
    profile_import_button.config(state=NORMAL)

# def debug_set_root_folder():
#     global profile_list
#     global dp_root_folder_path
#     dir_result = "/Users/allen/Desktop/sample_profiles"
#     # dir_result = "/Users/allen/Desktop/empty"
#     dp_root_folder_path= dir_result
#     dp_root_folder_display.set("Selected: " + dir_result)
#     root_folder_path_label.config(foreground='navy')

#     profile_list = duck_objs.build_profile(dir_result)
#     update_profile_display()
#     enable_buttons()

def profile_shift_up():
    global profile_var
    global profile_list
    selection = profile_lstbox.curselection()
    if len(selection) <= 0 or selection[0] == 0:
        return
    source = selection[0]
    destination = selection[0] - 1
    profile_list[destination], profile_list[source] = profile_list[source], profile_list[destination]
    update_profile_display()
    profile_lstbox.selection_clear(0, len(profile_list))
    profile_lstbox.selection_set(destination)
    update_profile_display()

def profile_shift_down():
    global profile_var
    global profile_list
    selection = profile_lstbox.curselection()
    if len(selection) <= 0 or selection[0] == len(profile_list) - 1:
        return
    source = selection[0]
    destination = selection[0] + 1
    profile_list[destination], profile_list[source] = profile_list[source], profile_list[destination]
    update_profile_display()
    profile_lstbox.selection_clear(0, len(profile_list))
    profile_lstbox.selection_set(destination)
    update_profile_display()

def adapt_color(rgb_tuple):
    if (rgb_tuple[0]*0.299 + rgb_tuple[1]*0.587 + rgb_tuple[2]*0.114) > 145:
        return "black"
    return 'white'

def update_profile_display():
    global selected_key
    profile_var.set([' '+x.name for x in profile_list]) # update profile listbox
    if len(profile_lstbox.curselection()) <= 0:
        return
    index = profile_lstbox.curselection()[0]
    bg_color_hex = rgb_to_hex(profile_list[index].bg_color)
    bg_color_button.config(background=bg_color_hex)

    if profile_list[index].kd_color is None:
        kd_R1.select()
        kd_color_button.config(background=default_button_color)
    else:
        kd_R2.select()
        kd_color_button.config(background=rgb_to_hex(profile_list[index].kd_color))

    if profile_list[index].dim_unused:
        dim_unused_keys_checkbox.select()
    else:
        dim_unused_keys_checkbox.deselect()

    update_key_button_appearances(index)
    reset_key_button_relief()
    key_name_entrybox.delete(0, 'end')
    selected_key = None
    key_color_button.config(background=default_button_color)
    key_color_rb1.config(state=DISABLED)
    key_color_rb2.config(state=DISABLED)
    script_textbox.delete(1.0, 'end')
    check_syntax_label.config(text="", fg="green")

def update_key_button_appearances(profile_index):
    if profile_index is None:
        for x in range(15):
            key_button_list[x].config(background=default_button_color, text='')
        return
    for count, item in enumerate(profile_list[profile_index].keylist):
        if item is not None:
            this_color = None
            if item.color is not None:
                key_button_list[count].config(background=rgb_to_hex(item.color))
                this_color = item.color
            else:
                key_button_list[count].config(background=rgb_to_hex(profile_list[profile_index].bg_color))
                this_color = profile_list[profile_index].bg_color
            key_button_list[count].config(text=item.name[:7], foreground=adapt_color(this_color))
        elif item is None and profile_list[profile_index].dim_unused is False:
            key_button_list[count].config(text='', background=rgb_to_hex(profile_list[profile_index].bg_color))
        elif item is None and profile_list[profile_index].dim_unused:
            key_button_list[count].config(background=default_button_color, text='')

def kd_radiobutton_auto_click():
    global profile_list
    selection = profile_lstbox.curselection()
    if len(selection) <= 0:
        return
    profile_list[selection[0]].kd_color = None
    update_profile_display()

def kd_radiobutton_custom_click():
    global profile_list
    selection = profile_lstbox.curselection()
    if len(selection) <= 0:
        return
    profile_list[selection[0]].kd_color = last_rgb
    update_profile_display()

def dim_unused_keys_click():
    global profile_list
    selection = profile_lstbox.curselection()
    if len(selection) <= 0:
        return
    profile_list[selection[0]].dim_unused = bool(dim_unused_keys_checkbox_var.get())
    update_profile_display()

def on_profile_lstbox_select(event):
    update_profile_display()

def bg_color_click(event):
    global profile_list
    global last_rgb
    selection = profile_lstbox.curselection()
    if len(selection) <= 0:
        return
    result = askcolor(color=profile_list[selection[0]].bg_color, title="Background color for " + profile_list[selection[0]].name + " profile")[0]
    if result is None:
        return
    last_rgb = result
    profile_list[selection[0]].bg_color = result
    update_profile_display()

def kd_color_click(event):
    global profile_list
    selection = profile_lstbox.curselection()
    if len(selection) <= 0 or kd_color_var.get() == 0:
        return
    result = askcolor(color=profile_list[selection[0]].kd_color, title="Activation color for " + profile_list[selection[0]].name + " profile")[0]
    if result is None:
        return
    profile_list[selection[0]].kd_color = result
    update_profile_display()

invalid_filename_characters = ['<', '>', ':', '"', '/', '\\', '|', '?', '*']

def clean_input(str_input, len_limit=None, clean_filename=True):
    result = ''.join([x for x in str_input if 32 <= ord(x) <= 126 and x not in invalid_filename_characters])
    if clean_filename is False:
        result = ''.join([x for x in str_input if 32 <= ord(x) <= 126])
    while('  ' in result):
        result = result.replace('  ', ' ')
    if len_limit is not None:
        result = result[:len_limit]
    return result.strip()

def profile_add_click():
    global profile_list
    if len(profile_list) >= duck_objs.MAX_PROFILE_COUNT:
        return
    answer = simpledialog.askstring("Input", "New profile name?", parent=profiles_lf)
    if answer is None:
        return

    insert_point = len(profile_list)
    try:
        insert_point = profile_lstbox.curselection()[0] + 1
    except Exception as e:
        # print('insert:', e)
        pass

    if insert_point >= 9:
        answer = clean_input(answer, 12)
    else:
        answer = clean_input(answer, 13)

    if len(answer) <= 0:# or answer in [x.name for x in profile_list]:
        return

    new_profile = duck_objs.dp_profile()
    new_profile.name = answer
    profile_list.insert(insert_point, new_profile)
    update_profile_display()
    profile_lstbox.selection_clear(0, len(profile_list))
    profile_lstbox.selection_set(insert_point)
    update_profile_display()

def profile_remove_click():
    global profile_list
    selection = profile_lstbox.curselection()
    if len(selection) <= 0:
        return
    profile_list.pop(selection[0])
    update_profile_display()
    profile_lstbox.selection_clear(0, len(profile_list))
    profile_lstbox.selection_set(selection[0])
    update_profile_display()
    if len(profile_list) <= 0 or len(profile_lstbox.curselection()) <= 0:
        update_key_button_appearances(None)
        reset_key_button_relief()

def profile_dupe_click():
    global profile_list
    selection = profile_lstbox.curselection()
    if len(selection) <= 0:
        return
    answer = simpledialog.askstring("Input", "New name?", parent=profiles_lf, initialvalue=profile_list[selection[0]].name)
    if answer is None:
        return
    answer = clean_input(answer, 13)
    if len(answer) <= 0: # or answer in [x.name for x in profile_list]:
        return
    new_profile = copy.deepcopy(profile_list[selection[0]])
    new_profile.name = answer
    profile_list.insert(selection[0] + 1, new_profile)
    update_profile_display()
    profile_lstbox.selection_clear(0, len(profile_list))
    profile_lstbox.selection_set(selection[0] + 1)
    update_profile_display()

def profile_rename_click():
    global profile_list
    selection = profile_lstbox.curselection()
    if len(selection) <= 0:
        return
    answer = simpledialog.askstring("Input", "New name?", parent=profiles_lf, initialvalue=profile_list[selection[0]].name)
    if answer is None:
        return
    answer = clean_input(answer, 13)
    if len(answer) <= 0 or answer in [x.name for x in profile_list]:
        return
    profile_list[selection[0]].name = answer
    update_profile_display()

def validate_data_objs(save_path):
    # update path and indexs of profile and keys
    for profile_index, this_profile in enumerate(profile_list):
        this_profile.path = os.path.join(save_path, 'profile'+str(profile_index+1)+'_'+str(this_profile.name))
        for key_index, this_key in enumerate(this_profile.keylist):
            if this_key is None:
                continue
            this_key.path = os.path.join(this_profile.path, 'key'+str(key_index+1)+'.txt')
            this_key.index = key_index + 1

def compile_all_scripts():
    try:
        for this_profile in profile_list:
            for this_key in this_profile.keylist:
                if this_key is not None:
                    this_key.binary_array = make_bytecode.make_dsb(this_key.script.split('\n'), profile_list)
                    if len(this_key.binary_array) >= 65530:
                        messagebox.showerror("Error", f'Script size too large!\n\nProfile: {this_profile.name}\nKey: {this_key.name}')
                        return False
        return True
    except Exception as e:
        error_msg = "Code contains error!\n"
        error_msg += f"Profile [{this_profile.name}] Key [{this_key.name}]:\n"
        error_msg += str(e)
        messagebox.showerror("Error", error_msg)
    return False

def save_everything(save_path):
    if compile_all_scripts() is False:
        return False
    dp_root_folder_display.set("Saving...")
    root.update()
    try:
        validate_data_objs(save_path)
        ensure_dir(save_path)
        my_dirs = [d for d in os.listdir(save_path) if os.path.isdir(os.path.join(save_path, d))]
        my_dirs = [x for x in my_dirs if x.startswith('profile') and x[7].isnumeric() and '_' in x]
        my_dirs = [os.path.join(save_path, d) for d in my_dirs if d.startswith("profile")]
        for item in my_dirs:
            try:
                shutil.rmtree(item)
                time.sleep(0.05)
            except FileNotFoundError:
                continue
        for this_profile in profile_list:
            os.mkdir(this_profile.path)
            time.sleep(0.05)

            config_file = open(os.path.join(this_profile.path, 'config.txt'), 'w', encoding='utf8', newline='')
            for this_key in this_profile.keylist:
                if this_key is None:
                    continue
                config_file.write('z' + str(this_key.index) + ' ' + str(this_key.name) + '\n')

            config_file.write('BG_COLOR %d %d %d\n' % (this_profile.bg_color))
            if this_profile.kd_color is not None:
                config_file.write('KEYDOWN_COLOR %d %d %d\n' % (this_profile.kd_color))
            if this_profile.dim_unused is False:
                config_file.write('DIM_UNUSED_KEYS 0\n')
            for this_key in this_profile.keylist:
                if this_key is None:
                    continue
                # newline='' is important, it forces python to not write \r, only \n
                # otherwise it will read back as double \n
                with open(this_key.path, 'w', encoding='utf8', newline='') as key_file:
                    key_file.write(this_key.script)

                dsb_path = this_key.path
                pre, ext = os.path.splitext(dsb_path)
                dsb_path = pre + '.dsb'

                with open(dsb_path, 'wb') as dsb_file:
                    dsb_file.write(this_key.binary_array)   
                if this_key.color is not None:
                    config_file.write('SWCOLOR_%d %d %d %d\n' % (this_key.index, this_key.color[0], this_key.color[1], this_key.color[2]))
            config_file.close()

        dps_path = os.path.join(save_path, 'dp_settings.txt')
        try:
            found = False
            for index, line in enumerate(dp_settings.list_of_lines):
                if 'sleep_after_min' in line:
                    found = True
                    dp_settings.list_of_lines[index] = "sleep_after_min " + str(sleepmode_slider.get()) + "\n";
            if found is False:
                dp_settings.list_of_lines.append("sleep_after_min " + str(sleepmode_slider.get()) + "\n")
        except Exception as e:
            print("dps", e)

        with open(dps_path, 'w+', encoding='utf8', newline='') as setting_file:
            setting_file.writelines(dp_settings.list_of_lines);

        dp_root_folder_display.set("Saved!")
        return True
    except Exception as e:
        error_msg = f"Save Failed:\n\n{e}"
        messagebox.showerror("Error", error_msg)
        dp_root_folder_display.set("Save FAILED!")
    return False

def make_default_backup_dir_name():
    return 'duckyPad_backup_' + datetime.now().strftime("%Y-%m-%dT%H-%M-%S")

def save_click():
    global is_using_hid
    global current_hid_op
    if save_everything(os.path.join(backup_path, make_default_backup_dir_name())) is False:
        return
    if is_using_hid is False:
        save_everything(dp_root_folder_path)
    else:
        save_everything(hid_modified_dir_path)
        current_hid_op = HID_SAVE

def backup_button_click():
    messagebox.showinfo("Backups", "All your backups are here!\n\nCopy back to SD card to restore")
    if 'darwin' in sys.platform:
        subprocess.Popen(["open", backup_path])
    elif 'linux' in sys.platform:
        subprocess.Popen(["xdg-open", backup_path])
    else:
        webbrowser.open(backup_path)

def key_button_click_event(event):
    key_button_click(event.widget)

def key_button_click(button_widget):
    global last_rgb
    global selected_key
    global key_button_clicked_at
    if len(profile_lstbox.curselection()) <= 0:
        return
    profile_index = profile_lstbox.curselection()[0]
    selected_key = key_button_list.index(button_widget)
    reset_key_button_relief()
    button_widget.config(borderwidth=7, relief='sunken')
    key_name_entrybox.delete(0, 'end')
    if profile_list[profile_index].keylist[selected_key] is not None:
        scripts_lf.place(x=scaled_size(536), y=scaled_size(50))
        empty_script_lf.place_forget()
        key_name_entrybox.insert(0, profile_list[profile_index].keylist[selected_key].name)
        script_textbox.delete(1.0, 'end')
        script_textbox.insert(1.0, profile_list[profile_index].keylist[selected_key].script.rstrip('\n').rstrip('\r'))
    else:
        scripts_lf.place_forget()
        empty_script_lf.place(x=scaled_size(600), y=scaled_size(300))
        key_color_button.config(background=default_button_color)
        key_color_rb1.config(state=DISABLED)
        key_color_rb2.config(state=DISABLED)
        script_textbox.delete(1.0, 'end')
        return

    key_color_rb1.config(state=NORMAL)
    key_color_rb2.config(state=NORMAL)
    if profile_list[profile_index].keylist[selected_key].color is None:
        key_color_rb1.select()
        key_color_button.config(background=default_button_color)
    else:
        key_color_rb2.select()
        last_rgb = profile_list[profile_index].keylist[selected_key].color
        key_color_button.config(background=rgb_to_hex(profile_list[profile_index].keylist[selected_key].color))
    key_button_clicked_at = modified_count
    check_syntax()

root = Tk()
root.title("duckyPad configurator v" + THIS_VERSION_NUMBER)
root.geometry(str(MAIN_WINDOW_WIDTH) + "x" + str(MAIN_WINDOW_HEIGHT))
root.resizable(width=FALSE, height=FALSE)
profile_list = []

# ------------- Folder select -------------
dp_root_folder_display = StringVar()
dp_root_folder_path= ''
dp_root_folder_display.set(INVALID_ROOT_FOLDER_STRING)

root_folder_lf = LabelFrame(root, text="Files", width=scaled_size(779), height=HEIGHT_ROOT_FOLDER_LF)
root_folder_lf.place(x=PADDING, y=0)
root.update()

help_button = Button(root_folder_lf, text="Help!", command=create_help_window)
help_button.place(x=scaled_size(5), y=0, width=scaled_size(65), height=scaled_size(25))

root_folder_select_button = Button(root_folder_lf, text="Connect", command=connect_button_click)
root_folder_select_button.place(x=scaled_size(75), y=0, width=scaled_size(75), height=scaled_size(25))

root_folder_path_label = Label(master=root_folder_lf, textvariable=dp_root_folder_display, foreground='red')
root_folder_path_label.place(x=scaled_size(155), y=0)

save_button = Button(root_folder_lf, text="Save", command=save_click, state=DISABLED)
save_button.place(x=scaled_size(630), y=0, width=scaled_size(65), height=scaled_size(25))

backup_button = Button(root_folder_lf, text="Backup...", command=backup_button_click)
backup_button.place(x=scaled_size(700), y=0, width=scaled_size(65), height=scaled_size(25))

# ------------- Profiles frame -------------

profile_var = StringVar()
profiles_lf = LabelFrame(root, text="Profiles", width=int(MAIN_WINDOW_WIDTH / 3 - PADDING * 1.3), height=MAIN_COLUMN_HEIGHT - HEIGHT_ROOT_FOLDER_LF - PADDING)
profiles_lf.place(x=PADDING, y=HEIGHT_ROOT_FOLDER_LF)
root.update()

profile_lstbox = Listbox(profiles_lf, listvariable=profile_var, height=16, exportselection=0) #, selectmode='single'?
profile_lstbox.place(x=scaled_size(32), y=PADDING, width=scaled_size(182), height=scaled_size(270))
profile_lstbox.bind('<<ListboxSelect>>', on_profile_lstbox_select)

profile_up_button = Button(profiles_lf, text="↑", command=profile_shift_up, state=DISABLED)
profile_up_button.place(x=scaled_size(5), y=scaled_size(60), width=scaled_size(20), height=scaled_size(40))

profile_remove_button = Button(profiles_lf, text="X", command=profile_remove_click, state=DISABLED)
profile_remove_button.place(x=scaled_size(5), y=scaled_size(110), width=scaled_size(20), height=scaled_size(40))

profile_down_button = Button(profiles_lf, text="↓", command=profile_shift_down, state=DISABLED)
profile_down_button.place(x=scaled_size(5), y=scaled_size(160), width=scaled_size(20), height=scaled_size(40))

BUTTON_WIDTH = int(profiles_lf.winfo_width() / 2.5)
BUTTON_HEIGHT = scaled_size(25)
BUTTON_Y_POS = scaled_size(295)

profile_add_button = Button(profiles_lf, text="New", command=profile_add_click, state=DISABLED)
profile_add_button.place(x=PADDING*2, y=BUTTON_Y_POS, width=BUTTON_WIDTH, height=BUTTON_HEIGHT)

profile_dupe_button = Button(profiles_lf, text="Duplicate", command=profile_dupe_click, state=DISABLED)
profile_dupe_button.place(x=PADDING * 2.5 + BUTTON_WIDTH, y=BUTTON_Y_POS, width=BUTTON_WIDTH, height=BUTTON_HEIGHT)

profile_rename_button = Button(profiles_lf, text="Rename", command=profile_rename_click, state=DISABLED)
profile_rename_button.place(x=PADDING * 2.5 + BUTTON_WIDTH, y=BUTTON_Y_POS + BUTTON_HEIGHT + int(PADDING/2), width=BUTTON_WIDTH, height=BUTTON_HEIGHT)

bg_color_label = Label(master=profiles_lf, text="Background color:", fg='grey')
bg_color_label.place(x=scaled_size(20), y=scaled_size(355))

bg_color_button = Label(master=profiles_lf, borderwidth=1, relief="solid")
bg_color_button.place(x=scaled_size(160), y=scaled_size(356), width=scaled_size(60), height=scaled_size(20))
bg_color_button.bind("<Button-1>", bg_color_click)

kd_color_label = Label(master=profiles_lf, text="Activation color:", fg='grey')
kd_color_label.place(x=scaled_size(20), y=scaled_size(380))

kd_color_button = Label(master=profiles_lf, borderwidth=1, relief="solid")
kd_color_button.place(x=scaled_size(160), y=scaled_size(407), width=scaled_size(60), height=scaled_size(20))
kd_color_button.bind("<Button-1>", kd_color_click)

dim_unused_keys_checkbox_var = IntVar()
dim_unused_keys_checkbox = Checkbutton(profiles_lf, text="Dim unused keys", variable=dim_unused_keys_checkbox_var, command=dim_unused_keys_click, state=DISABLED)
dim_unused_keys_checkbox.place(x=scaled_size(20), y=scaled_size(425))

kd_color_var = IntVar()
kd_R1 = Radiobutton(profiles_lf, text="      Auto", variable=kd_color_var, value=0, command=kd_radiobutton_auto_click, state=DISABLED)
kd_R1.place(x=scaled_size(130), y=scaled_size(380))
kd_R2 = Radiobutton(profiles_lf, text="", variable=kd_color_var, value=1, command=kd_radiobutton_custom_click, state=DISABLED)
kd_R2.place(x=scaled_size(130), y=scaled_size(405))

# ------------- Keys frame -------------
selected_key = None

keys_lf = LabelFrame(root, text="Keys", width=int(MAIN_WINDOW_WIDTH / 3 - PADDING * 1.3), height=MAIN_COLUMN_HEIGHT - HEIGHT_ROOT_FOLDER_LF - PADDING)
keys_lf.place(x=profiles_lf.winfo_x() + profiles_lf.winfo_width() + PADDING, y=profiles_lf.winfo_y())
root.update()

key_instruction = Label(master=keys_lf, text="click to edit, drag to rearrange")
root.update()
key_instruction.place(x=scaled_size(30), y=0)

KEY_BUTTON_HEADROOM = scaled_size(20)
KEY_BUTTON_WIDTH = scaled_size(75)
KEY_BUTTON_HEIGHT = scaled_size(45)
KEY_BUTTON_GAP = int((keys_lf.winfo_width() - 3 * KEY_BUTTON_WIDTH) / 4.5)

def search_button(rootx, rooty):
    for index, item in enumerate(key_button_list):
        xstart = item.winfo_rootx()
        xend = item.winfo_rootx() + item.winfo_width()
        ystart = item.winfo_rooty()
        yend = item.winfo_rooty() + item.winfo_height()
        if xstart < rootx < xend and ystart < rooty < yend:
            return index
    return None

drag_source_button_index = None
drag_destination_button_index = None
def button_drag_start(event):
    global drag_source_button_index
    global drag_destination_button_index
    if len(profile_lstbox.curselection()) <= 0:
        return
    profile_index = profile_lstbox.curselection()[0]
    drag_source_button_index = key_button_list.index(event.widget)
    # if empty button
    if profile_list[profile_index].keylist[drag_source_button_index] is None:
        drag_source_button_index = None
        drag_destination_button_index = None
        return
    drag_destination_button_index = search_button(event.x_root, event.y_root)
    if drag_source_button_index == drag_destination_button_index:
        drag_source_button_index = None
        drag_destination_button_index = None
        return
    reset_key_button_relief()
    event.widget.config(borderwidth=7, relief='sunken')
    if drag_source_button_index is not None and drag_destination_button_index is not None and drag_destination_button_index != drag_source_button_index:
        key_button_list[drag_destination_button_index].config(text='move\nhere', background='white', foreground='black', borderwidth=4)
    else:
        update_key_button_appearances(profile_index)

def update_keylist_index():
    if len(profile_lstbox.curselection()) <= 0:
        return
    profile_index = profile_lstbox.curselection()[0]
    for index, item in enumerate(profile_list[profile_index].keylist):
        if item is not None:
            item.index = index + 1

def button_drag_release(event):
    global drag_source_button_index
    global drag_destination_button_index
    if len(profile_lstbox.curselection()) <= 0:
        return
    if drag_source_button_index == drag_destination_button_index:
        return
    # print('source:', drag_source_button_index)
    # print('destination:', drag_destination_button_index)
    # print('------')
    profile_index = profile_lstbox.curselection()[0]
    update_key_button_appearances(profile_index)
    reset_key_button_relief()
    if drag_source_button_index is not None and drag_destination_button_index is not None:
        profile_list[profile_index].keylist[drag_destination_button_index], profile_list[profile_index].keylist[drag_source_button_index] = profile_list[profile_index].keylist[drag_source_button_index], profile_list[profile_index].keylist[drag_destination_button_index]
        update_profile_display()
        update_keylist_index()
    if drag_source_button_index is not None and drag_destination_button_index is None:
        key_button_click(key_button_list[drag_source_button_index])
    if drag_destination_button_index is not None:
        key_button_click(key_button_list[drag_destination_button_index])
    drag_source_button_index = None
    drag_destination_button_index = None

key_button_xy_list = [(KEY_BUTTON_GAP,KEY_BUTTON_HEADROOM+PADDING), (KEY_BUTTON_GAP*2+KEY_BUTTON_WIDTH,KEY_BUTTON_HEADROOM+PADDING), (KEY_BUTTON_GAP*3+KEY_BUTTON_WIDTH*2,KEY_BUTTON_HEADROOM+PADDING), (KEY_BUTTON_GAP,KEY_BUTTON_HEADROOM+PADDING*2+KEY_BUTTON_HEIGHT), (KEY_BUTTON_GAP*2+KEY_BUTTON_WIDTH,KEY_BUTTON_HEADROOM+PADDING*2+KEY_BUTTON_HEIGHT), (KEY_BUTTON_GAP*3+KEY_BUTTON_WIDTH*2,KEY_BUTTON_HEADROOM+PADDING*2+KEY_BUTTON_HEIGHT), (KEY_BUTTON_GAP,KEY_BUTTON_HEADROOM+PADDING*3+KEY_BUTTON_HEIGHT*2), (KEY_BUTTON_GAP*2+KEY_BUTTON_WIDTH,KEY_BUTTON_HEADROOM+PADDING*3+KEY_BUTTON_HEIGHT*2), (KEY_BUTTON_GAP*3+KEY_BUTTON_WIDTH*2,KEY_BUTTON_HEADROOM+PADDING*3+KEY_BUTTON_HEIGHT*2), (KEY_BUTTON_GAP,KEY_BUTTON_HEADROOM+PADDING*4+KEY_BUTTON_HEIGHT*3), (KEY_BUTTON_GAP*2+KEY_BUTTON_WIDTH,KEY_BUTTON_HEADROOM+PADDING*4+KEY_BUTTON_HEIGHT*3), (KEY_BUTTON_GAP*3+KEY_BUTTON_WIDTH*2,KEY_BUTTON_HEADROOM+PADDING*4+KEY_BUTTON_HEIGHT*3), (KEY_BUTTON_GAP,KEY_BUTTON_HEADROOM+PADDING*5+KEY_BUTTON_HEIGHT*4), (KEY_BUTTON_GAP*2+KEY_BUTTON_WIDTH,KEY_BUTTON_HEADROOM+PADDING*5+KEY_BUTTON_HEIGHT*4), (KEY_BUTTON_GAP*3+KEY_BUTTON_WIDTH*2,KEY_BUTTON_HEADROOM+PADDING*5+KEY_BUTTON_HEIGHT*4)]
key_button_list = []
for x in range(15):
    this_button = Label(master=keys_lf, borderwidth=1, relief="solid", background=default_button_color, font=(None, 13))

    this_button.place(x=key_button_xy_list[x][0], y=key_button_xy_list[x][1], width=KEY_BUTTON_WIDTH, height=KEY_BUTTON_HEIGHT)
    this_button.bind("<Button-1>", key_button_click_event)
    this_button.bind("<B1-Motion>", button_drag_start)
    this_button.bind("<ButtonRelease-1>", button_drag_release)
    key_button_list.append(this_button)

key_name_text = Label(master=keys_lf, text="Key name:", fg='grey')
key_name_text.place(x=PADDING, y=scaled_size(305))
root.update()

def key_name_entrybox_return_pressed(event):
    key_rename_click()

key_name_entrybox = Entry(keys_lf, state=DISABLED)
key_name_entrybox.place(x=key_name_text.winfo_width() + PADDING, y=scaled_size(305), width=scaled_size(145))
key_name_entrybox.bind('<Return>', key_name_entrybox_return_pressed)

KEY_BUTTON_GAP = int((keys_lf.winfo_width() - 2 * BUTTON_WIDTH) / 3.5)

def key_rename_click():
    if is_key_selected() == False:
        return
    profile_index = profile_lstbox.curselection()[0]
    result = clean_input(key_name_entrybox.get(), 7, clean_filename=False)
    if len(result) <= 0: # or result in [x.name for x in profile_list[profile_index].keylist if x is not None]
        return
    if profile_list[profile_index].keylist[selected_key] is not None:
        profile_list[profile_index].keylist[selected_key].name = result
    else:
        new_key = duck_objs.dp_key()
        new_key.name = result
        profile_list[profile_index].keylist[selected_key] = new_key
        update_keylist_index()
    update_key_button_appearances(profile_index)
    key_button_click(key_button_list[selected_key])

def key_remove_click():
    if is_key_selected() == False:
        return
    profile_index = profile_lstbox.curselection()[0]
    profile_list[profile_index].keylist[selected_key] = None
    update_key_button_appearances(profile_index)
    key_button_click(key_button_list[selected_key])

key_rename_button = Button(keys_lf, text="Apply", command=key_rename_click, state=DISABLED, fg="green")
key_rename_button.place(x=KEY_BUTTON_GAP, y=scaled_size(340), width=BUTTON_WIDTH, height=BUTTON_HEIGHT)
root.update()
key_remove_button = Button(keys_lf, text="Remove", command=key_remove_click, state=DISABLED, fg="red")
key_remove_button.place(x=KEY_BUTTON_GAP*2+key_rename_button.winfo_width(), y=scaled_size(340), width=BUTTON_WIDTH, height=BUTTON_HEIGHT)

key_color_text = Label(master=keys_lf, text="Key color:", fg='grey')
key_color_text.place(x=PADDING, y=scaled_size(380))
root.update()

def key_color_rb1_click():
    if is_key_selected() == False:
        return
    profile_index = profile_lstbox.curselection()[0]
    if profile_list[profile_index].keylist[selected_key] is not None:
        profile_list[profile_index].keylist[selected_key].color = None
    update_key_button_appearances(profile_index)
    key_button_click(key_button_list[selected_key])

def key_color_rb2_click():
    if is_key_selected() == False:
        return
    profile_index = profile_lstbox.curselection()[0]
    if profile_list[profile_index].keylist[selected_key] is not None:
        profile_list[profile_index].keylist[selected_key].color = last_rgb
    update_key_button_appearances(profile_index)
    key_button_click(key_button_list[selected_key])

def is_key_selected():
    if len(profile_lstbox.curselection()) <= 0:
        return False
    if selected_key is None:
        return False
    return True

def key_color_button_click(event):
    global last_rgb
    if is_key_selected() == False:
        return
    profile_index = profile_lstbox.curselection()[0]
    if profile_list[profile_index].keylist[selected_key] is not None:
        # Color picker should have an initial colour set in colour picker
        initial_color = profile_list[profile_index].keylist[selected_key].color if profile_list[profile_index].keylist[selected_key].color is not None else profile_list[profile_index].bg_color
        result = askcolor(color=initial_color, title="Key color for " + profile_list[profile_index].keylist[selected_key].name)[0]
        if result is None:
            return
        last_rgb = result
        profile_list[profile_index].keylist[selected_key].color = result
    update_key_button_appearances(profile_index)
    key_button_click(key_button_list[selected_key])

key_color_type_var = IntVar()
key_color_rb1 = Radiobutton(keys_lf, text="Same as background", variable=key_color_type_var, value=0, command=key_color_rb1_click,state=DISABLED)
key_color_rb1.place(x=scaled_size(85), y=scaled_size(380))
key_color_rb2 = Radiobutton(keys_lf, text="", variable=key_color_type_var, value=1, command=key_color_rb2_click, state=DISABLED)
key_color_rb2.place(x=scaled_size(85), y=scaled_size(405))

key_color_button = Label(master=keys_lf, borderwidth=1, relief="solid")
key_color_button.place(x=scaled_size(135), y=scaled_size(407), width=scaled_size(60), height=scaled_size(20))
key_color_button.bind("<Button-1>", key_color_button_click)

# ------------- Scripts frame -------------
scripts_lf = LabelFrame(root, text="Scripts", width=int(MAIN_WINDOW_WIDTH / 3 - PADDING * 1.3), height=MAIN_COLUMN_HEIGHT - HEIGHT_ROOT_FOLDER_LF - PADDING)

def open_duckyscript_url():
    webbrowser.open('https://github.com/dekuNukem/duckyPad/blob/master/duckyscript_info.md#writing-duckyscript')

def script_instruction_click(event):
    open_duckyscript_url()

script_instruction = Label(master=scripts_lf, text="Read more about Duckyscript...", fg="blue", cursor="hand2")
root.update()
script_instruction.place(x=scaled_size(25), y=0)
script_instruction.bind("<Button-1>", script_instruction_click)

modified_count = 0
key_button_clicked_at = 0
last_textbox_edit = 0
modification_checked = 0

def script_textbox_modified():
    global modified_count
    global last_textbox_edit
    global modification_checked
    if is_key_selected() == False:
        return
    modified_count += 1
    last_textbox_edit = time.time()
    profile_index = profile_lstbox.curselection()[0]
    cantthinkofaname = ""
    if modified_count - key_button_clicked_at > 2:
        if profile_list[profile_index].keylist[selected_key] is not None:
            cantthinkofaname = "Checking..."
        check_syntax_label.config(text=cantthinkofaname, fg="black")
    if profile_list[profile_index].keylist[selected_key] is not None:
        profile_list[profile_index].keylist[selected_key].script = script_textbox.get(1.0, END)
        modification_checked = 0
    
def script_textbox_event(event):
    script_textbox_modified()
    script_textbox.tk.call(script_textbox._w, 'edit', 'modified', 0)

script_textbox = Text(scripts_lf, relief='solid', borderwidth=1, padx=2, pady=2, spacing3=5, wrap="word")
script_textbox.place(x=key_button_list[0].winfo_x(), y=KEY_BUTTON_HEADROOM+PADDING-3, width=key_button_list[-1].winfo_x() + KEY_BUTTON_WIDTH - KEY_BUTTON_GAP, height=key_button_list[-1].winfo_y() - key_button_list[0].winfo_y() + KEY_BUTTON_HEIGHT + 5)
root.update()
script_textbox.bind("<<Modified>>", script_textbox_event)
script_textbox.tag_configure("error", background="#ffff00")

script_common_commands_lf = LabelFrame(scripts_lf, text="Common commands", width=script_textbox.winfo_width(), height=scaled_size(105))
script_common_commands_lf.place(x=PADDING, y=scaled_size(300))
root.update()

def check_syntax():
    if is_key_selected() == False:
        return
    profile_index = profile_lstbox.curselection()[0]
    if profile_list[profile_index].keylist[selected_key] is None:
        return
    script_textbox.tag_remove("error", '1.0', 'end')
    program_listing = profile_list[profile_index].keylist[selected_key].script.split('\n')
    result_dict = ds3_preprocessor.run_all(program_listing)
    if result_dict["is_success"]:
        script_textbox.tag_remove("error", '1.0', 'end')
        check_syntax_label.config(text="Code seems OK..", fg="green")
    else:
        error_lnum = result_dict['error_line_number_starting_from_1']
        script_textbox.tag_add("error", str(error_lnum)+".0", str(error_lnum)+".0 lineend")
        check_syntax_label.config(text=result_dict['comments'], fg='red')

check_syntax_label = Label(scripts_lf, text="")
check_syntax_label.place(x=scaled_size(10), y=scaled_size(417))
root.update()

SCRIPT_BUTTON_WIDTH = script_textbox.winfo_width()/3.4
SCRIPT_BUTTON_GAP = scaled_size(5)
PADDING = scaled_size(2)

script_button_xy_list = [(SCRIPT_BUTTON_GAP, PADDING), (SCRIPT_BUTTON_GAP*2+SCRIPT_BUTTON_WIDTH, PADDING), (SCRIPT_BUTTON_GAP*3+SCRIPT_BUTTON_WIDTH*2, PADDING), (SCRIPT_BUTTON_GAP, PADDING+BUTTON_HEIGHT+2), (SCRIPT_BUTTON_GAP*2+SCRIPT_BUTTON_WIDTH, PADDING+BUTTON_HEIGHT+2), (SCRIPT_BUTTON_GAP*3+SCRIPT_BUTTON_WIDTH*2, PADDING+BUTTON_HEIGHT+2), (SCRIPT_BUTTON_GAP, (PADDING+BUTTON_HEIGHT)*2+2), (SCRIPT_BUTTON_GAP*2+SCRIPT_BUTTON_WIDTH, (PADDING+BUTTON_HEIGHT)*2+2), (SCRIPT_BUTTON_GAP*3+SCRIPT_BUTTON_WIDTH*2, (PADDING+BUTTON_HEIGHT)*2+2)]
script_button_commands = ["STRINGLN", "STRING", "DELAY", "CTRL", "SHIFT", "ALT", "ENTER", "REPEAT", "more..."]
script_command_button_list = []

for x in range(9):
    def this_func(x=x):
        script_textbox.insert(INSERT, "\n" + script_button_commands[x] + " ")
    this_button = Button(script_common_commands_lf, text=script_button_commands[x], command=this_func, state=DISABLED)
    this_button.place(x=script_button_xy_list[x][0], y=script_button_xy_list[x][1], width=SCRIPT_BUTTON_WIDTH, height=BUTTON_HEIGHT)
    script_command_button_list.append(this_button)
script_command_button_list[-1].config(command=open_duckyscript_url)

PADDING = scaled_size(10)

def add_s(word, value):
    if value == 1:
        return word
    return word + 's'

def minutes_to_str(value):
    value = int(value)
    if value == 0:
        return "Never"
    this_hours = int(value/60)
    this_minutes = value % 60
    result = ''
    if this_hours > 0:
        result += str(this_hours) + add_s(" hour", this_hours)
    result += " " + str(this_minutes) + add_s(" minute", this_minutes)
    return result

def slider_adjust_sleepmode(value):
    enter_sleep_mode_label.config(text="Sleep after: " + minutes_to_str(value))

settings_lf = LabelFrame(root, text="Settings", width=scaled_size(516), height=scaled_size(70))
settings_lf.place(x=scaled_size(10), y=scaled_size(525))
enter_sleep_mode_label = Label(master=settings_lf, text="Sleep after: Never")
enter_sleep_mode_label.place(x=scaled_size(10), y=0)

sleepmode_slider = Scale(settings_lf)
sleepmode_slider.config(from_=0, to=360, length=190, showvalue=0, sliderlength=20, orient=HORIZONTAL, command=slider_adjust_sleepmode)
sleepmode_slider.set(0)
sleepmode_slider.place(x=scaled_size(10), y=scaled_size(20))
sleepmode_slider.config(state=DISABLED)

updates_lf = LabelFrame(root, text="Updates", width=scaled_size(253), height=scaled_size(70))
updates_lf.place(x=scaled_size(536), y=scaled_size(525))

pc_app_update_label = Label(master=updates_lf)
pc_app_update_label.place(x=scaled_size(5), y=scaled_size(0))
update_stats = check_update.get_pc_app_update_status(THIS_VERSION_NUMBER)

if update_stats == 0:
    pc_app_update_label.config(text='This app (' + str(THIS_VERSION_NUMBER) + '): Up to date', fg='black', bg=default_button_color)
    pc_app_update_label.unbind("<Button-1>")
elif update_stats == 1:
    pc_app_update_label.config(text='This app (' + str(THIS_VERSION_NUMBER) + '): Update available! Click me!', fg='black', bg='orange', cursor="hand2")
    pc_app_update_label.bind("<Button-1>", app_update_click)
else:
    pc_app_update_label.config(text='This app (' + str(THIS_VERSION_NUMBER) + '): Unknown', fg='black', bg=default_button_color)
    pc_app_update_label.unbind("<Button-1>")

dp_fw_update_label = Label(master=updates_lf, text="Firmware: Unknown")
dp_fw_update_label.place(x=scaled_size(5), y=scaled_size(25))

def open_profile_autoswitcher_url():
    webbrowser.open('https://github.com/dekuNukem/duckyPad-profile-autoswitcher/blob/master/README.md#duckypad-profile-auto-switcher')

def import_profile_click():
    global profile_list
    # messagebox.showinfo("Import", f"Select a folder containing the profiles")
    import_path = filedialog.askdirectory()
    if len(import_path) <= 0:
        return
    is_success, content = duck_objs.import_profile(import_path)
    if is_success is False:
        messagebox.showinfo("Import", f"Import failed:\n\n{content}")
        return
    profile_list += content
    update_profile_display()

autoswitch_button = Button(settings_lf, text="Profile Autoswitcher", command=open_profile_autoswitcher_url)
autoswitch_button.place(x=scaled_size(250), y=0, width=scaled_size(200), height=scaled_size(40))

profile_import_button = Button(profiles_lf, text="Import", command=import_profile_click, state=DISABLED)
profile_import_button.place(x=PADDING * 2, y=BUTTON_Y_POS + BUTTON_HEIGHT + int(PADDING/2), width=BUTTON_WIDTH, height=BUTTON_HEIGHT)

empty_script_lf = Label(root, text="<-- Select a key")
empty_script_lf.place(x=scaled_size(600), y=scaled_size(300))
root.update()

def repeat_func():
    global modification_checked
    if time.time() - last_textbox_edit >= 0.5 and modification_checked == 0:
        check_syntax()
        modification_checked = 1
    root.after(500, repeat_func)

def t1_worker():
    global current_hid_op
    while(1):
        time.sleep(0.2)
        if current_hid_op == HID_NOP:
            continue
        is_dp_ready, comment = hid_op.is_dp_ready()
        if is_dp_ready is False:
            messagebox.showerror("Error", comment)
            dp_root_folder_display.set("")
            current_hid_op = HID_NOP
            continue
        if current_hid_op == HID_DUMP:
            root_folder_path_label.config(foreground='navy')
            dp_root_folder_display.set("Loading...")
            current_hid_op = HID_NOP
            try:
                hid_op.dump_from_hid(hid_dump_path, dp_root_folder_display)
                select_root_folder(hid_dump_path, check_fw=False)
                print("done!")
                dp_root_folder_display.set("done!")
            except Exception as e:
                messagebox.showerror("Error", "error:\n\n"+str(traceback.format_exc()))
                dp_root_folder_display.set("HID load error!")
                continue
        if current_hid_op == HID_SAVE:
            hid_op.duckypad_hid_close()
            try:
                hid_op.duckypad_hid_init()
                hid_op.duckypad_hid_file_sync(hid_dump_path, hid_modified_dir_path, dp_root_folder_display)
                hid_op.duckypad_hid_sw_reset()
                try:
                    shutil.rmtree(hid_dump_path)
                    time.sleep(0.05)
                except FileNotFoundError:
                    pass
                os.rename(hid_modified_dir_path, hid_dump_path)
            except Exception as e:
                messagebox.showerror("error", "Save error: " + str(traceback.format_exc()))
                dp_root_folder_display.set("Save FAILED!")
            current_hid_op = HID_NOP

t1 = threading.Thread(target=t1_worker, daemon=True)
t1.start()

def on_closing():
    if current_hid_op == HID_NOP:
        root.destroy()
    elif messagebox.askokcancel("Quit", "Still saving data! Quit anyway?"):
        root.destroy()

root.protocol("WM_DELETE_WINDOW", on_closing)

root.after(500, repeat_func)
# if os.name == 'posix':
#     debug_set_root_folder()
# root.iconbitmap('icon.ico')    
root.mainloop()
