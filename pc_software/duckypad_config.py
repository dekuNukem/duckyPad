import os
import sys
import time
import copy
import shutil
import pathlib
from datetime import datetime
import traceback
import duck_objs
import webbrowser
import check_update
import ds_syntax_check
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


def ensure_dir(dir_path):
    if not os.path.exists(dir_path):
        os.makedirs(dir_path)

appname = 'duckypad_config'
appauthor = 'dekuNukem'
save_path = user_data_dir(appname, appauthor, roaming=True)
backup_path = os.path.join(save_path, 'profile_backups')
ensure_dir(save_path)
ensure_dir(backup_path)
save_filename = os.path.join(save_path, 'config.txt')
print(backup_path)
config_dict = {}
config_dict['auto_backup_enabled'] = True

try:
    with open(save_filename) as json_file:
        temp = json.load(json_file)
        if isinstance(temp, dict):
            config_dict = temp
        else:
            raise ValueError("not a valid config file")
except Exception as e:
    pass

def save_config():
    try:
        with open(save_filename, 'w', encoding='utf8') as save_file:
                save_file.write(json.dumps(config_dict, sort_keys=True))
    except Exception as e:
        messagebox.showerror("Error", "Save failed!\n\n"+str(traceback.format_exc()))

default_button_color = 'SystemButtonFace'
if 'linux' in sys.platform:
    default_button_color = 'grey'

THIS_VERSION_NUMBER = '0.12.1'
MAIN_WINDOW_WIDTH = 800
MAIN_WINDOW_HEIGHT = 625
MAIN_COLOUM_HEIGHT = 533
PADDING = 10
HIGHT_ROOT_FOLDER_LF = 50
INVALID_ROOT_FOLDER_STRING = "<-- Press to connect to duckyPad"
last_rgb = (238,130,238)
dp_settings = duck_objs.dp_global_settings()
discord_link_url = "https://raw.githubusercontent.com/dekuNukem/duckyPad/master/resources/discord_link.txt"
sd_card_keymap_list = []

def open_discord_link():
    try:
        webbrowser.open(str(urllib.request.urlopen(discord_link_url).read().decode('utf-8')).split('\n')[0])
    except Exception as e:
        messagebox.showerror("Error", "Failed to open discord link!\n"+str(e))

def create_help_window():
    help_window = Toplevel(root)
    help_window.title("duckyPad help")
    help_window.geometry("280x180")
    help_window.resizable(width=FALSE, height=FALSE)
    help_window.grab_set()

    user_manual_label = Label(master=help_window, text="Not sure what to do? Please read...")
    user_manual_label.place(x=35, y=5)
    user_manual_button = Button(help_window, text="User Manual", command=open_duckypad_user_manual_url)
    user_manual_button.place(x=60, y=30, width=160)

    troubleshoot_label = Label(master=help_window, text="Problems? Please see...")
    troubleshoot_label.place(x=35, y=60)
    troubleshoot_button = Button(help_window, text="Troubleshooting Guides", command=open_duckypad_troubleshooting_url)
    troubleshoot_button.place(x=50, y=85, width=180)

    discord_label = Label(master=help_window, text="Questions or comments? Ask in...")
    discord_label.place(x=35, y=60+55)
    discord_button = Button(help_window, text="Official Discord Chatroom", command=open_discord_link)
    discord_button.place(x=50, y=85+55, width=180)

def open_duckypad_user_manual_url():
    webbrowser.open('https://github.com/dekuNukem/duckyPad/blob/master/getting_started.md')

def open_duckypad_troubleshooting_url():
    webbrowser.open('https://github.com/dekuNukem/duckyPad/blob/master/troubleshooting.md')

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
    save_as_button.config(state=DISABLED)
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
    clear_and_disable_script_textbox()
    syntax_check_result_label.config(text="", fg="green")
    sleepmode_slider.config(state=DISABLED)

def check_firmware_update():
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

def fw_update_click(what):
    webbrowser.open('https://github.com/dekuNukem/duckyPad/blob/master/firmware_updates_and_version_history.md')

def print_fw_update_label():
    fw_result, this_version = check_firmware_update()
    if fw_result == 0:
        dp_fw_update_label.config(text='Firmware (' + str(this_version) +'): Up to date', fg='black', bg=default_button_color)
        dp_fw_update_label.unbind("<Button-1>")
    elif fw_result == 1:
        dp_fw_update_label.config(text='Firmware (' + str(this_version) +'): Update available! Click me!', fg='black', bg='orange', cursor="hand2")
        dp_fw_update_label.bind("<Button-1>", fw_update_click)
    else:
        dp_fw_update_label.config(text='Firmware: Unknown', fg='black', bg=default_button_color)
        dp_fw_update_label.unbind("<Button-1>")

def select_root_folder():
    global profile_list
    global sd_card_keymap_list
    global dp_root_folder_path
    dir_result = filedialog.askdirectory()
    if len(dir_result) <= 0:
        return
    dp_root_folder_path = dir_result
    dp_root_folder_display.set("Selected: " + dir_result)
    root_folder_path_label.config(foreground='navy')
    profile_list = duck_objs.build_profile(dir_result)
    dp_settings.load_from_path(dp_root_folder_path)
    try:
        sd_card_keymap_list = duck_objs.load_keymap(dp_root_folder_path)
    except Exception as e:
        print(e)
    print_fw_update_label()
    ui_reset()
    update_profile_display()
    enable_buttons()

def connect_button_click():
    try:
        hid_op.duckypad_hid_init()
    except Exception as e:
        messagebox.showerror("Error", "Connection error: " + str(e) + "\n\nYou can mount duckyPad SD card on your PC and select it instead")
        select_root_folder()

def enable_buttons():
    profile_add_button.config(state=NORMAL)
    profile_remove_button.config(state=NORMAL)
    profile_rename_button.config(state=NORMAL)
    profile_up_button.config(state=NORMAL)
    profile_down_button.config(state=NORMAL)
    profile_dupe_button.config(state=NORMAL)
    save_button.config(state=NORMAL)
    save_as_button.config(state=NORMAL)
    kd_R1.config(state=NORMAL)
    kd_R2.config(state=NORMAL)
    dim_unused_keys_checkbox.config(state=NORMAL)
    key_rename_button.config(state=NORMAL)
    key_remove_button.config(state=NORMAL)
    execute_button.config(state=NORMAL, fg="red")
    keyboard_layout_button.config(state=NORMAL)
    for button in script_command_button_list:
        button.config(state=NORMAL)
    key_name_entrybox.config(state=NORMAL)
    bg_color_label.config(fg='black')
    kd_color_label.config(fg='black')
    key_name_text.config(fg='black')
    key_color_text.config(fg='black')
    sleepmode_slider.config(state=NORMAL)
    sleepmode_slider.set(dp_settings.sleep_after_minutes)

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
    clear_and_disable_script_textbox()
    syntax_check_result_label.config(text="", fg="green")

def clear_and_disable_script_textbox():
    script_textbox.delete(1.0, 'end')
    script_textbox.config(state=DISABLED)

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
    result = askcolor()[-1]
    if result is None:
        return
    last_rgb = hex_to_rgb(result)
    profile_list[selection[0]].bg_color = hex_to_rgb(result)
    update_profile_display()

def kd_color_click(event):
    global profile_list
    selection = profile_lstbox.curselection()
    if len(selection) <= 0 or kd_color_var.get() == 0:
        return
    result = askcolor()[-1]
    if result is None:
        return
    profile_list[selection[0]].kd_color = hex_to_rgb(result)
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

def dump_keymap(save_path):
    global sd_card_keymap_list
    file_list = [d for d in os.listdir(save_path) if d.startswith("dpkm_") and d.endswith(".txt")]
    for item in file_list:
        file_path = os.path.join(save_path, item)
        try:
            os.remove(file_path)
        except Exception:
            pass
    for item in sd_card_keymap_list:
        if item.url is not None:
            item.content = str(urllib.request.urlopen(item.url).read().decode('utf-8')).split('\n')
    for item in sd_card_keymap_list:
        file_path = os.path.join(save_path, item.file_name)
        with open(file_path, 'w', encoding='utf8') as keymap_file:
            keymap_file.writelines(s.replace('\n', '').replace('\r', '') + '\n' for s in item.content);

def save_everything(save_path):
    global last_save
    save_result_label.config(text='Saving...', fg="white", bg="blue", cursor="")
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
            config_file = open(os.path.join(this_profile.path, 'config.txt'), 'w')
            for this_key in this_profile.keylist:
                if this_key is None:
                    continue
                this_key.check_loop()
                config_file.write('z' + str(this_key.index) + ' ' + str(this_key.name) + '\n')
                if this_key.has_loop:
                    config_file.write('s' + str(this_key.index) + ' ' + str(this_key.max_loop+1) + '\n')

            config_file.write('BG_COLOR %d %d %d\n' % (this_profile.bg_color))
            if this_profile.kd_color is not None:
                config_file.write('KEYDOWN_COLOR %d %d %d\n' % (this_profile.kd_color))
            if this_profile.dim_unused is False:
                config_file.write('DIM_UNUSED_KEYS 0\n')
            for this_key in this_profile.keylist:
                if this_key is None:
                    continue
                with open(this_key.path, 'w', encoding='utf8') as key_file:
                    key_file.write(this_key.script)
                if this_key.color is not None:
                    config_file.write('SWCOLOR_%d %d %d %d\n' % (this_key.index, this_key.color[0], this_key.color[1], this_key.color[2]))
            config_file.close()

        keymap_folder_path = os.path.join(save_path, 'keymaps')
        ensure_dir(keymap_folder_path)
        dump_keymap(keymap_folder_path)

        dps_path = os.path.join(save_path, 'dp_settings.txt')
        dps_lines = ["sleep_after_min " + str(sleepmode_slider.get()) + "\n"]
        try:
            dps_file = open(dps_path, encoding='utf-8')
            dps_lines = dps_file.readlines()
            dps_file.close()
            for index, line in enumerate(dps_lines):
                if 'sleep_after_min' in line:
                    dps_lines[index] = "sleep_after_min " + str(sleepmode_slider.get()) + "\n";
        except Exception:
            pass

        with open(dps_path, 'w+') as setting_file:
            setting_file.writelines(dps_lines);
        save_result_label.config(text='Saved!', fg="green", bg=default_button_color, cursor="")
        save_result_label.unbind("<Button-1>")

    except Exception as e:
        # print('save_click:', e)
        messagebox.showerror("Error", "Save Failed!\n\n"+str(e))
        # messagebox.showerror("Error", "Save Failed!\n"+str(traceback.format_exc()))
        save_result_label.config(text='Save FAILED!', fg="red", bg=default_button_color, cursor="")
        save_result_label.unbind("<Button-1>")
    last_save = time.time()

# def current_time_str():
#     ret = datetime.utcnow().isoformat(sep='T')
#     return (ret[:19] + "Z").replace(':', '-')

def make_default_backup_dir_name():
    return 'duckyPad_backup_' + datetime.now().strftime("%Y-%m-%dT%H-%M-%S")

def save_click():
    save_everything(dp_root_folder_path)
    if config_dict['auto_backup_enabled']:
        save_everything(os.path.join(backup_path, make_default_backup_dir_name()))

def backup_button_click():
    if config_dict['auto_backup_enabled']:
        messagebox.showinfo("Backups", "Auto backup is ON!\n\nAll your backups are here!")
        if 'darwin' in sys.platform:
        	subprocess.Popen(["open", backup_path])
        elif 'linux' in sys.platform:
        	subprocess.Popen(["xdg-open", backup_path])
        else:
        	webbrowser.open(backup_path)
    else:
        messagebox.showinfo("Backups", "Auto backup is OFF!\n\nSelect a folder to save a backup.")
        dir_result = filedialog.askdirectory(initialdir=os.path.join(os.path.expanduser('~'), "Desktop"))
        if len(dir_result) <= 0:
            return
        save_everything(os.path.join(dir_result, make_default_backup_dir_name()))

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
        key_name_entrybox.insert(0, profile_list[profile_index].keylist[selected_key].name)
        script_textbox.config(state=NORMAL)
        script_textbox.delete(1.0, 'end')
        script_textbox.insert(1.0, profile_list[profile_index].keylist[selected_key].script.rstrip('\n'))

    if profile_list[profile_index].keylist[selected_key] is None:
        key_color_button.config(background=default_button_color)
        key_color_rb1.config(state=DISABLED)
        key_color_rb2.config(state=DISABLED)
        clear_and_disable_script_textbox()
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
    check_syntax_click()

root = Tk()
root.title("duckyPad configurator v" + THIS_VERSION_NUMBER)
root.geometry(str(MAIN_WINDOW_WIDTH) + "x" + str(MAIN_WINDOW_HEIGHT))
root.resizable(width=FALSE, height=FALSE)
profile_list = []

# ------------- Folder select -------------
dp_root_folder_display = StringVar()
dp_root_folder_path= ''
dp_root_folder_display.set(INVALID_ROOT_FOLDER_STRING)

root_folder_lf = LabelFrame(root, text="Files", width=779, height=HIGHT_ROOT_FOLDER_LF)
root_folder_lf.place(x=PADDING, y=0) 
root.update()

help_button = Button(root_folder_lf, text="Help!", command=create_help_window)
help_button.place(x=5, y=0, width=65)

root_folder_select_button = Button(root_folder_lf, text="Connect", command=connect_button_click)
root_folder_select_button.place(x=75, y=0, width=75)

root_folder_path_label = Label(master=root_folder_lf, textvariable=dp_root_folder_display, foreground='red')
root_folder_path_label.place(x=155, y=0)

save_button = Button(root_folder_lf, text="Save", command=save_click, state=DISABLED)
save_button.place(x=535, y=0, width=50)

save_as_button = Button(root_folder_lf, text="Backup...", command=backup_button_click, state=DISABLED)
save_as_button.place(x=590, y=0, width=65)

save_result_label = Label(master=root_folder_lf, text="")
save_result_label.place(x=660, y=-6, width=110, height=35)

def app_update_click(event):
    webbrowser.open('https://github.com/dekuNukem/duckyPad/releases')

last_save = 0

# ------------- Profiles frame -------------

profile_var = StringVar()
profiles_lf = LabelFrame(root, text="Profiles", width=int(MAIN_WINDOW_WIDTH / 3 - PADDING * 1.3), height=MAIN_COLOUM_HEIGHT - HIGHT_ROOT_FOLDER_LF - PADDING)
profiles_lf.place(x=PADDING, y=HIGHT_ROOT_FOLDER_LF)
root.update()

profile_lstbox = Listbox(profiles_lf, listvariable=profile_var, height=16, exportselection=0) #, selectmode='single'?
profile_lstbox.place(x=32, y=PADDING, width=182, height=270)
profile_lstbox.bind('<<ListboxSelect>>', on_profile_lstbox_select)

profile_up_button = Button(profiles_lf, text="↑", command=profile_shift_up, state=DISABLED)
profile_up_button.place(x=5, y=80, width=20, height=40)

profile_down_button = Button(profiles_lf, text="↓", command=profile_shift_down, state=DISABLED)
profile_down_button.place(x=5, y=140, width=20, height=40)

BUTTON_WIDTH = int(profiles_lf.winfo_width() / 2.5)
BUTTON_HEIGHT = 25
BUTTON_Y_POS = 295

profile_add_button = Button(profiles_lf, text="New", command=profile_add_click, state=DISABLED)
profile_add_button.place(x=PADDING*2, y=BUTTON_Y_POS, width=BUTTON_WIDTH, height=BUTTON_HEIGHT)

profile_dupe_button = Button(profiles_lf, text="Duplicate", command=profile_dupe_click, state=DISABLED)
profile_dupe_button.place(x=PADDING * 2.5 + BUTTON_WIDTH, y=BUTTON_Y_POS, width=BUTTON_WIDTH, height=BUTTON_HEIGHT)

profile_remove_button = Button(profiles_lf, text="Remove", command=profile_remove_click, state=DISABLED)
profile_remove_button.place(x=PADDING * 2, y=BUTTON_Y_POS + BUTTON_HEIGHT + int(PADDING/2), width=BUTTON_WIDTH, height=BUTTON_HEIGHT)

profile_rename_button = Button(profiles_lf, text="Rename", command=profile_rename_click, state=DISABLED)
profile_rename_button.place(x=PADDING * 2.5 + BUTTON_WIDTH, y=BUTTON_Y_POS + BUTTON_HEIGHT + int(PADDING/2), width=BUTTON_WIDTH, height=BUTTON_HEIGHT)

bg_color_label = Label(master=profiles_lf, text="Background color:", fg='grey')
bg_color_label.place(x=20, y=355)

bg_color_button = Label(master=profiles_lf, borderwidth=1, relief="solid")
bg_color_button.place(x=160, y=356, width=60, height=20)
bg_color_button.bind("<Button-1>", bg_color_click)

kd_color_label = Label(master=profiles_lf, text="Activation color:", fg='grey')
kd_color_label.place(x=20, y=380)

kd_color_button = Label(master=profiles_lf, borderwidth=1, relief="solid")
kd_color_button.place(x=160, y=407, width=60, height=20)
kd_color_button.bind("<Button-1>", kd_color_click)

dim_unused_keys_checkbox_var = IntVar()
dim_unused_keys_checkbox = Checkbutton(profiles_lf, text="Dim unused keys", variable=dim_unused_keys_checkbox_var, command=dim_unused_keys_click, state=DISABLED)
dim_unused_keys_checkbox.place(x=22, y=425)

kd_color_var = IntVar()
kd_R1 = Radiobutton(profiles_lf, text="      Auto", variable=kd_color_var, value=0, command=kd_radiobutton_auto_click, state=DISABLED)
kd_R1.place(x=130, y=380)
kd_R2 = Radiobutton(profiles_lf, text="", variable=kd_color_var, value=1, command=kd_radiobutton_custom_click, state=DISABLED)
kd_R2.place(x=130, y=405)

# ------------- Keys frame -------------
selected_key = None

keys_lf = LabelFrame(root, text="Keys", width=int(MAIN_WINDOW_WIDTH / 3 - PADDING * 1.3), height=MAIN_COLOUM_HEIGHT - HIGHT_ROOT_FOLDER_LF - PADDING)
keys_lf.place(x=profiles_lf.winfo_x() + profiles_lf.winfo_width() + PADDING, y=profiles_lf.winfo_y()) 
root.update()

key_instruction = Label(master=keys_lf, text="click to edit, drag to rearrange")
root.update()
key_instruction.place(x=30, y=0)

KEY_BUTTON_HEADROOM = 20
KEY_BUTTON_WIDTH = 75
KEY_BUTTON_HEIGHT = 45
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
key_name_text.place(x=PADDING, y=305)
root.update()

def key_name_entrybox_return_pressed(event):
    key_rename_click()

key_name_entrybox = Entry(keys_lf, state=DISABLED)
key_name_entrybox.place(x=key_name_text.winfo_width()+PADDING, y=305, width=145)
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
    # print(profile_list[profile_index].keylist[selected_key])
    update_key_button_appearances(profile_index)
    key_button_click(key_button_list[selected_key])

def key_remove_click():
    if is_key_selected() == False:
        return
    profile_index = profile_lstbox.curselection()[0]
    profile_list[profile_index].keylist[selected_key] = None
    update_key_button_appearances(profile_index)
    key_button_click(key_button_list[selected_key])

key_rename_button = Button(keys_lf, text="Apply", command=key_rename_click, state=DISABLED)
key_rename_button.place(x=KEY_BUTTON_GAP, y=340, width=BUTTON_WIDTH, height=BUTTON_HEIGHT)
root.update()
key_remove_button = Button(keys_lf, text="Remove", command=key_remove_click, state=DISABLED)
key_remove_button.place(x=KEY_BUTTON_GAP*2+key_rename_button.winfo_width(), y=340, width=BUTTON_WIDTH, height=BUTTON_HEIGHT)

key_color_text = Label(master=keys_lf, text="Key color:", fg='grey')
key_color_text.place(x=PADDING, y=380)
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
        result = askcolor()[-1]
        if result is None:
            return
        last_rgb = hex_to_rgb(result)
        profile_list[profile_index].keylist[selected_key].color = hex_to_rgb(result)
    update_key_button_appearances(profile_index)
    key_button_click(key_button_list[selected_key])

key_color_type_var = IntVar()
key_color_rb1 = Radiobutton(keys_lf, text="Same as background", variable=key_color_type_var, value=0, command=key_color_rb1_click,state=DISABLED)
key_color_rb1.place(x=85, y=380)
key_color_rb2 = Radiobutton(keys_lf, text="", variable=key_color_type_var, value=1, command=key_color_rb2_click, state=DISABLED)
key_color_rb2.place(x=85, y=405)

key_color_button = Label(master=keys_lf, borderwidth=1, relief="solid")
key_color_button.place(x=135, y=407, width=60, height=20)
key_color_button.bind("<Button-1>", key_color_button_click)

# ------------- Scripts frame -------------
scripts_lf = LabelFrame(root, text="Scripts", width=int(MAIN_WINDOW_WIDTH / 3 - PADDING * 1.3), height=MAIN_COLOUM_HEIGHT - HIGHT_ROOT_FOLDER_LF - PADDING)
scripts_lf.place(x=keys_lf.winfo_x() + keys_lf.winfo_width() + PADDING, y=keys_lf.winfo_y())

def open_duckyscript_url():
    webbrowser.open('https://github.com/dekuNukem/duckyPad/blob/master/duckyscript_info.md')

def script_instruction_click(event):
    open_duckyscript_url()

script_instruction = Label(master=scripts_lf, text="Read more about Duckyscript...", fg="blue", cursor="hand2")
root.update()
script_instruction.place(x=25, y=0)
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
        syntax_check_result_label.config(text=cantthinkofaname, fg="black")
    if profile_list[profile_index].keylist[selected_key] is not None:
        profile_list[profile_index].keylist[selected_key].script = script_textbox.get(1.0, END)#.replace('\r','').strip().strip('\n')
        modification_checked = 0
    
def script_textbox_event(event):
    script_textbox_modified()
    script_textbox.tk.call(script_textbox._w, 'edit', 'modified', 0)

script_exe_warning_showed = False

def run_script():
    global script_exe_warning_showed
    if len(script_textbox.get("1.0",END)) <= 2:
        return
    warning_msg = "You are about to execute this script.\n> MAKE SURE YOU TRUST IT!\n> Result might differ from real duckyPad.\n\nExecution will start after a 2-second delay."
    if 'darwin' in sys.platform:
        warning_msg = "You are about to execute this script.\n> MAKE SURE YOU TRUST IT!\n> Result might differ from real duckyPad.\n> If nothing happens, give this app Accessibility permission.\n> Check Getting Started Guide for details. \n\nExecution will start after a 2-second delay."
    if(script_exe_warning_showed is False):
        if(messagebox.askokcancel("Warning", warning_msg) == False):
            return
    try:
        import autogui
        time.sleep(2)
        exe_result, exe_note = autogui.execute_file(script_textbox.get("1.0",END).replace('\r', '').split('\n'))
        if exe_result != 0:
            messagebox.showerror("Error", exe_note)
    except Exception as e:
        messagebox.showerror("Error", "execution failed:\n\n" + str(e))
    script_exe_warning_showed = True

script_textbox = Text(scripts_lf, relief='solid', borderwidth=1, padx=2, pady=2, spacing3=5, wrap="word", state=DISABLED)
script_textbox.place(x=key_button_list[0].winfo_x(), y=KEY_BUTTON_HEADROOM+PADDING-3, width=key_button_list[-1].winfo_x() + KEY_BUTTON_WIDTH - KEY_BUTTON_GAP, height=key_button_list[-1].winfo_y() - key_button_list[0].winfo_y() + KEY_BUTTON_HEIGHT + 5)
root.update()
script_textbox.bind("<<Modified>>", script_textbox_event)
script_textbox.tag_configure("error", background="#ffff00")

script_common_commands_lf = LabelFrame(scripts_lf, text="Common commands", width=script_textbox.winfo_width(), height=105)
script_common_commands_lf.place(x=PADDING, y=300)
root.update()

check_syntax_lf = LabelFrame(scripts_lf, text="Code check", width=script_textbox.winfo_width()/2, height=40)
check_syntax_lf.place(x=PADDING, y=407)
root.update()

SCRIPT_BUTTON_WIDTH = script_textbox.winfo_width()/3.4
SCRIPT_BUTTON_GAP = 5
PADDING = 2

execute_button = Button(scripts_lf, text="Run this script!", command=run_script, state=DISABLED)
execute_button.place(x=135, y=417, width=105, height=BUTTON_HEIGHT)
root.update()

script_button_xy_list = [(SCRIPT_BUTTON_GAP, PADDING), (SCRIPT_BUTTON_GAP*2+SCRIPT_BUTTON_WIDTH, PADDING), (SCRIPT_BUTTON_GAP*3+SCRIPT_BUTTON_WIDTH*2, PADDING), (SCRIPT_BUTTON_GAP, PADDING+BUTTON_HEIGHT+2), (SCRIPT_BUTTON_GAP*2+SCRIPT_BUTTON_WIDTH, PADDING+BUTTON_HEIGHT+2), (SCRIPT_BUTTON_GAP*3+SCRIPT_BUTTON_WIDTH*2, PADDING+BUTTON_HEIGHT+2), (SCRIPT_BUTTON_GAP, (PADDING+BUTTON_HEIGHT)*2+2), (SCRIPT_BUTTON_GAP*2+SCRIPT_BUTTON_WIDTH, (PADDING+BUTTON_HEIGHT)*2+2), (SCRIPT_BUTTON_GAP*3+SCRIPT_BUTTON_WIDTH*2, (PADDING+BUTTON_HEIGHT)*2+2)]
script_button_commands = ["STRING", "CTRL", "SHIFT", "ALT", "GUI", "ENTER", "DELAY", "REPEAT", "more..."]
script_command_button_list = []

for x in range(9):
    def this_func(x=x):
        script_textbox.insert(INSERT, "\n" + script_button_commands[x] + " ")
    this_button = Button(script_common_commands_lf, text=script_button_commands[x], command=this_func, state=DISABLED)
    this_button.place(x=script_button_xy_list[x][0], y=script_button_xy_list[x][1], width=SCRIPT_BUTTON_WIDTH, height=BUTTON_HEIGHT)
    script_command_button_list.append(this_button)
script_command_button_list[-1].config(command=open_duckyscript_url)

def check_syntax_click():
    if is_key_selected() == False:
        return
    has_errors = False
    profile_index = profile_lstbox.curselection()[0]
    if profile_list[profile_index].keylist[selected_key] is None:
        return
    script_textbox.tag_remove("error", '1.0', 'end')
    for count, line in enumerate(profile_list[profile_index].keylist[selected_key].script.split('\n')):
        # print(ds_syntax_check.parse_line(line), line)
        if ds_syntax_check.parse_line(line) != ds_syntax_check.PARSE_OK:
            # print("syntax error on line", count, ':', line)
            script_textbox.tag_add("error", str(count+1)+".0", str(count+1)+".0 lineend")
            # script_textbox.mark_set("insert", str(count+1)+".0")
            # script_textbox.see(str(count+1)+'.0')
            syntax_check_result_label.config(text='Error(s) found!', fg='red')
            has_errors = True
    if has_errors == False:
        script_textbox.tag_remove("error", '1.0', 'end')
        syntax_check_result_label.config(text="It looks alright..", fg="green")

syntax_check_result_label = Label(master=check_syntax_lf)
syntax_check_result_label.place(x=2, y=-4)

# --------------------------

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

kbl_online_listbox = None
kbl_on_sd_card_listbox = None
online_keymap_list = []
online_keymap_display_list = None
online_keymap_var = StringVar()
sd_keymap_var = StringVar()

def update_sd_listbox():
    sd_card_display_list = [x.display_name for x in sd_card_keymap_list]
    sd_card_display_list.insert(0, "English (US)")
    sd_keymap_var.set(sd_card_display_list)

def online_keymap_add_to_sd_button_click():
    global sd_card_keymap_list
    global kbl_online_listbox
    if len(kbl_online_listbox.curselection()) <= 0: # nothing selected
        return
    index = kbl_online_listbox.curselection()[0]
    if index == 0: # default keymap, already in firmware
        return
    if len(sd_card_keymap_list) >= 7:
        return
    if online_keymap_list[index-1].file_name in [x.file_name for x in sd_card_keymap_list]:
        return;
    sd_card_keymap_list.append(online_keymap_list[index-1])
    update_sd_listbox()

def remove_keymap_from_sd_card_button_click():
    global sd_card_keymap_list
    global kbl_on_sd_card_listbox
    if len(kbl_on_sd_card_listbox.curselection()) <= 0: # nothing selected
        return
    index = kbl_on_sd_card_listbox.curselection()[0]
    if index == 0: # default keymap, already in firmware
        return
    sd_card_keymap_list.pop(index-1)
    update_sd_listbox()

def add_local_keymap_to_sd_card_button_click():
    global sd_card_keymap_list
    local_keymap_file_path = filedialog.askopenfilename()
    if len(local_keymap_file_path) <= 0:
        return
    nameonly = os.path.basename(local_keymap_file_path)
    override_name_check = False
    if not (nameonly.startswith("dpkm_") and nameonly.endswith(".txt")):
        override_name_check = messagebox.askokcancel("Warning", "It doesn't look like a valid keymap file. Load it anyway?")
        if override_name_check is False:
            return
    this_keymap = duck_objs.load_keymap_from_file(local_keymap_file_path)
    if this_keymap.is_valid == 0:
        return
    if len(sd_card_keymap_list) >= 7:
        return
    if override_name_check:
        if not this_keymap.file_name.startswith("dpkm_"):
            this_keymap.file_name = "dpkm_" + this_keymap.file_name
        if not this_keymap.file_name.endswith(".txt"):
            this_keymap.file_name = this_keymap.file_name + '.txt'
    if this_keymap.file_name in [x.file_name for x in sd_card_keymap_list]:
        return;
    sd_card_keymap_list.append(this_keymap)
    update_sd_listbox()

def keymap_instruction_click(event):
    webbrowser.open('https://github.com/dekuNukem/duckyPad/blob/master/keymap_instructions.md')

def create_keyboard_layout_window():
    global kbl_on_sd_card_listbox
    global online_keymap_list
    global online_keymap_display_list
    global online_keymap_var
    global sd_keymap_var
    global kbl_online_listbox
    kbl_window = Toplevel(root)
    kbl_window.title("Keyboard layouts")
    kbl_window.geometry("480x210")
    kbl_window.resizable(width=FALSE, height=FALSE)
    kbl_window.grab_set()

    try:
        if len(online_keymap_list) <= 0:
            online_keymap_list = duck_objs.load_online_keymap()
    except Exception as e:
        messagebox.showerror("Error", e)

    update_sd_listbox()

    online_keymap_display_list = [x.display_name for x in online_keymap_list]
    online_keymap_display_list.insert(0, "English (US)")
    online_keymap_var.set(online_keymap_display_list)

    kbl_online_listbox = Listbox(kbl_window, listvariable=online_keymap_var, height=8, exportselection=0) #, selectmode='single'?
    kbl_online_listbox.place(x=20, y=30, width=160, height=150)

    online_kbl_label = Label(master=kbl_window, text="Available layouts:")
    online_kbl_label.place(x=20, y=5)

    kbl_on_sd_card_listbox = Listbox(kbl_window, listvariable=sd_keymap_var, height=8, exportselection=0) #, selectmode='single'?
    kbl_on_sd_card_listbox.place(x=295, y=30, width=160, height=150)

    online_keymap_add_to_sd_button = Button(kbl_window, text="Add\n--->", command=online_keymap_add_to_sd_button_click)
    online_keymap_add_to_sd_button.place(x=185, y=45, width=100)
    remove_keymap_from_sd_card_button = Button(kbl_window, text="Remove", command=remove_keymap_from_sd_card_button_click)
    remove_keymap_from_sd_card_button.place(x=185, y=100, width=100)
    add_local_keymap_to_sd_card_button = Button(kbl_window, text="Add local file", command=add_local_keymap_to_sd_card_button_click)
    add_local_keymap_to_sd_card_button.place(x=185, y=140, width=100)

    keymaps_on_sd_card_label = Label(master=kbl_window, text="Layouts on duckyPad:")
    keymaps_on_sd_card_label.place(x=295, y=5)

    keymap_instruction_label = Label(master=kbl_window, text="Click me to learn more about keymaps, including how to make your own!", fg="blue", cursor="hand2")
    keymap_instruction_label.place(x=45, y=185)
    keymap_instruction_label.bind("<Button-1>", keymap_instruction_click)

settings_lf = LabelFrame(root, text="Settings", width=516, height=90)
settings_lf.place(x=10, y=525) 
enter_sleep_mode_label = Label(master=settings_lf, text="Sleep after: Never")
enter_sleep_mode_label.place(x=10, y=0)

sleepmode_slider = Scale(settings_lf)
sleepmode_slider.config(from_=0, to=360, length=190, showvalue=0, sliderlength=20, orient=HORIZONTAL, command=slider_adjust_sleepmode)
sleepmode_slider.set(0)
sleepmode_slider.place(x=10, y=20)
sleepmode_slider.config(state=DISABLED)

def auto_backup_click():
    config_dict['auto_backup_enabled'] = bool(auto_backup_checkbox_var.get())
    save_config()

auto_backup_checkbox_var = IntVar()
auto_backup_checkbox_var.set(int(config_dict['auto_backup_enabled']))
auto_backup_checkbox = Checkbutton(settings_lf, text="Profile auto-backup", variable=auto_backup_checkbox_var, command=auto_backup_click)
auto_backup_checkbox.place(x=10, y=45)

updates_lf = LabelFrame(root, text="Updates", width=253, height=90)
updates_lf.place(x=536, y=525)

pc_app_update_label = Label(master=updates_lf)
pc_app_update_label.place(x=5, y=10)
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
dp_fw_update_label.place(x=5, y=35)

keyboard_layout_button = Button(settings_lf, text="Keyboard Layouts...", command=create_keyboard_layout_window, state=DISABLED)
keyboard_layout_button.place(x=220, y=13, width=140, height=BUTTON_HEIGHT)

def open_profile_autoswitcher_url():
    webbrowser.open('https://github.com/dekuNukem/duckyPad-profile-autoswitcher')

autoswitch_button = Button(settings_lf, text="Profile\nAutoswitcher", command=open_profile_autoswitcher_url)
autoswitch_button.place(x=370, y=0, width=125, height=40)

root.update()

def repeat_func():
    global modification_checked
    if time.time() - last_textbox_edit >= 0.5 and modification_checked == 0:
        check_syntax_click()
        modification_checked = 1
    if time.time() - last_save > 2 and 'update' not in save_result_label.cget("text").lower():
        save_result_label.config(text='')
    root.after(500, repeat_func)

root.after(500, repeat_func)
# if os.name == 'posix':
#     debug_set_root_folder()
# root.iconbitmap('icon.ico')    
root.mainloop()
