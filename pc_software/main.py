import os
import sys
import time
import copy
import random
import colorsys
import duck_objs
import webbrowser
import ds_syntax_check
from tkinter import *
from tkinter import filedialog
from tkinter import simpledialog
from tkinter.colorchooser import askcolor
from tkinter import messagebox

MAIN_WINDOW_WIDTH = 800
MAIN_WINDOW_HEIGHT = 533
PADDING = 10
HIGHT_ROOT_FOLDER_LF = 50
INVALID_ROOT_FOLDER_STRING = "<-- Please select your duckyPad root folder"
last_rgb = (238,130,238)

def reset_key_button_relief():
    for item in key_button_list:
        item.config(borderwidth=1, relief="solid")

def hex_to_rgb(hex_str):
    hex_str = hex_str.strip('#')
    return tuple(int(hex_str[i:i+2], 16) for i in (0, 2, 4))

def rgb_to_hex(rgb_tuple):
    return '#%02x%02x%02x' % rgb_tuple

def select_root_folder():
    global profile_list
    global dp_root_folder_path
    dir_result = filedialog.askdirectory()
    if len(dir_result) <= 0:
        return
    dp_root_folder_path= dir_result
    dp_root_folder_display.set("Selected: " + dir_result)
    root_folder_path_label.config(foreground='navy')

    profile_list = duck_objs.build_profile(dir_result)
    update_profile_display()
    enable_buttons()

def enable_buttons():
    profile_add_button.config(state=NORMAL)
    profile_remove_button.config(state=NORMAL)
    profile_rename_button.config(state=NORMAL)
    profile_up_button.config(state=NORMAL)
    profile_down_button.config(state=NORMAL)
    profile_dupe_button.config(state=NORMAL)
    save_button.config(state=NORMAL)

def debug_set_root_folder():
    global profile_list
    global dp_root_folder_path
    dir_result = "/Users/allen/Desktop/sample_profiles"
    # dir_result = "/Users/allen/Desktop/empty"
    dp_root_folder_path= dir_result
    dp_root_folder_display.set("Selected: " + dir_result)
    root_folder_path_label.config(foreground='navy')

    profile_list = duck_objs.build_profile(dir_result)
    update_profile_display()
    enable_buttons()

def profile_shift_up():
    global profile_var
    global profile_list
    selection = profile_listbox.curselection()
    if len(selection) <= 0 or selection[0] == 0:
        return
    source = selection[0]
    destination = selection[0] - 1
    profile_list[destination], profile_list[source] = profile_list[source], profile_list[destination]
    update_profile_display()
    profile_listbox.selection_clear(0, len(profile_list))
    profile_listbox.selection_set(destination)
    update_profile_display()

def profile_shift_down():
    global profile_var
    global profile_list
    selection = profile_listbox.curselection()
    if len(selection) <= 0 or selection[0] == len(profile_list) - 1:
        return
    source = selection[0]
    destination = selection[0] + 1
    profile_list[destination], profile_list[source] = profile_list[source], profile_list[destination]
    update_profile_display()
    profile_listbox.selection_clear(0, len(profile_list))
    profile_listbox.selection_set(destination)
    update_profile_display()

def adapt_color(rgb_tuple):
    if (rgb_tuple[0]*0.299 + rgb_tuple[1]*0.587 + rgb_tuple[2]*0.114) > 145:
        return "black"
    return 'white'

def update_profile_display():
    global selected_key
    profile_var.set([' '+x.name for x in profile_list]) # update profile listbox
    if len(profile_listbox.curselection()) <= 0:
        return
    index = profile_listbox.curselection()[0]
    bg_color_hex = "#abcdef"
    if profile_list[index].bg_color is not None:
        bg_color_hex = rgb_to_hex(profile_list[index].bg_color)
    bg_color_button.config(background=bg_color_hex)

    if profile_list[index].kd_color is None:
        kd_R1.select()
        kd_color_button.config(background='SystemButtonFace')
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
    key_color_button.config(background='SystemButtonFace')
    key_color_rb1.config(state=DISABLED)
    key_color_rb2.config(state=DISABLED)
    clear_and_disable_script_textbox()
    syntax_check_result_label.config(text="", fg="green")

def clear_and_disable_script_textbox():
    script_textbox.delete(1.0, 'end')
    script_textbox.config(state=DISABLED)

def update_key_button_appearances(profile_index):
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
        else:
            key_button_list[count].config(background='SystemButtonFace', text='')

def kd_radiobutton_auto_click():
    global profile_list
    selection = profile_listbox.curselection()
    if len(selection) <= 0:
        return
    profile_list[selection[0]].kd_color = None
    update_profile_display()

def kd_radiobutton_custom_click():
    global profile_list
    selection = profile_listbox.curselection()
    if len(selection) <= 0:
        return
    profile_list[selection[0]].kd_color = last_rgb
    update_profile_display()

def dim_unused_keys_click():
    global profile_list
    selection = profile_listbox.curselection()
    if len(selection) <= 0:
        return
    profile_list[selection[0]].dim_unused = bool(dim_unused_keys_checkbox_var.get())
    update_profile_display()

def on_profile_listbox_select(event):
    update_profile_display()
    
def bg_color_click(event):
    global profile_list
    global last_rgb
    selection = profile_listbox.curselection()
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
    selection = profile_listbox.curselection()
    if len(selection) <= 0 or kd_color_var.get() == 0:
        return
    result = askcolor()[-1]
    if result is None:
        return
    profile_list[selection[0]].kd_color = hex_to_rgb(result)
    update_profile_display()

def clean_input(str_input, len_limit=None):
    result = ''.join([x for x in str_input if 32 <= ord(x) <= 126])
    while('  ' in result):
        result = result.replace('  ', ' ')
    if len_limit is not None:
        result = result[:len_limit]
    return result.strip()

def profile_add_click():
    global profile_list
    answer = simpledialog.askstring("Input", "New profile name?", parent=profiles_lf)
    if answer is None:
        return
    answer = clean_input(answer, 13)
    if len(answer) <= 0 or answer in [x.name for x in profile_list]:
        return

    insert_point = len(profile_list)
    try:
        insert_point = profile_listbox.curselection()[0] + 1
    except Exception as e:
        print('insert:', e)

    new_profile = duck_objs.dp_profile()
    folder_name = 'profile' + str(len(profile_list)+1) + '_' + answer
    new_profile.path = os.path.join(dp_root_folder_path, folder_name)
    new_profile.name = answer
    profile_list.insert(insert_point, new_profile)
    update_profile_display()
    profile_listbox.selection_clear(0, len(profile_list))
    profile_listbox.selection_set(insert_point)
    update_profile_display()
    print(new_profile)

def profile_remove_click():
    global profile_list
    selection = profile_listbox.curselection()
    if len(selection) <= 0:
        return
    profile_list.pop(selection[0])
    update_profile_display()
    profile_listbox.selection_clear(0, len(profile_list))
    profile_listbox.selection_set(selection[0])
    update_profile_display()

def profile_dupe_click():
    global profile_list
    selection = profile_listbox.curselection()
    if len(selection) <= 0:
        return
    answer = simpledialog.askstring("Input", "New name?", parent=profiles_lf, initialvalue=profile_list[selection[0]].name)
    if answer is None:
        return
    answer = clean_input(answer, 13)
    if len(answer) <= 0 or answer in [x.name for x in profile_list]:
        return
    new_profile = copy.deepcopy(profile_list[selection[0]])
    new_profile.name = answer
    profile_list.insert(selection[0] + 1, new_profile)
    update_profile_display()
    profile_listbox.selection_clear(0, len(profile_list))
    profile_listbox.selection_set(selection[0] + 1)
    update_profile_display()

def profile_rename_click():
    global profile_list
    selection = profile_listbox.curselection()
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

def save_click():
    print('save_click')

def key_button_click_event(event):
    key_button_click(event.widget)

def key_button_click(button_widget):
    global selected_key
    global key_button_clicked_at
    if len(profile_listbox.curselection()) <= 0:
        return
    profile_index = profile_listbox.curselection()[0]
    selected_key = key_button_list.index(button_widget)
    reset_key_button_relief()
    button_widget.config(borderwidth=7, relief='sunken')
    key_name_entrybox.delete(0, 'end')
    if profile_list[profile_index].keylist[selected_key] is not None:
        key_name_entrybox.insert(0, profile_list[profile_index].keylist[selected_key].name)
        script_textbox.config(state=NORMAL)
        script_textbox.delete(1.0, 'end')
        script_textbox.insert(1.0, profile_list[profile_index].keylist[selected_key].script+'\n')

    if profile_list[profile_index].keylist[selected_key] is None:
        key_color_button.config(background='SystemButtonFace')
        key_color_rb1.config(state=DISABLED)
        key_color_rb2.config(state=DISABLED)
        clear_and_disable_script_textbox()
        return

    key_color_rb1.config(state=NORMAL)
    key_color_rb2.config(state=NORMAL)
    if profile_list[profile_index].keylist[selected_key].color is None:
        key_color_rb1.select()
        key_color_button.config(background='SystemButtonFace')
    else:
        key_color_rb2.select()
        key_color_button.config(background=rgb_to_hex(profile_list[profile_index].keylist[selected_key].color))
    key_button_clicked_at = modified_count
    check_syntax_click()

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

save_button = Button(root_folder_lf, text="Save", command=save_click, state=DISABLED, width='8')
save_button.pack()
save_button.place(x=400, y=0)

# ------------- Profiles frame -------------

profile_var = StringVar()
profiles_lf = LabelFrame(root, text="Profiles", width=int(MAIN_WINDOW_WIDTH / 3 - PADDING * 1.3), height=MAIN_WINDOW_HEIGHT - HIGHT_ROOT_FOLDER_LF - PADDING)
profiles_lf.pack()
profiles_lf.place(x=PADDING, y=HIGHT_ROOT_FOLDER_LF)
profiles_lf.pack_propagate(False)
root.update()

profile_listbox = Listbox(profiles_lf, listvariable=profile_var, height=16, exportselection=0) #, selectmode='single'?
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

profile_add_button = Button(profiles_lf, text="New", command=profile_add_click, state=DISABLED)
profile_add_button.pack()
profile_add_button.place(x=PADDING*2, y=BUTTON_Y_POS, width=BUTTON_WIDTH, height=BUTTON_HEIGHT)

profile_dupe_button = Button(profiles_lf, text="Duplicate", command=profile_dupe_click, state=DISABLED)
profile_dupe_button.pack()
profile_dupe_button.place(x=PADDING * 2.5 + BUTTON_WIDTH, y=BUTTON_Y_POS, width=BUTTON_WIDTH, height=BUTTON_HEIGHT)

profile_remove_button = Button(profiles_lf, text="Remove", command=profile_remove_click, state=DISABLED)
profile_remove_button.pack()
profile_remove_button.place(x=PADDING * 2, y=BUTTON_Y_POS + BUTTON_HEIGHT + int(PADDING/2), width=BUTTON_WIDTH, height=BUTTON_HEIGHT)

profile_rename_button = Button(profiles_lf, text="Rename", command=profile_rename_click, state=DISABLED)
profile_rename_button.pack()
profile_rename_button.place(x=PADDING * 2.5 + BUTTON_WIDTH, y=BUTTON_Y_POS + BUTTON_HEIGHT + int(PADDING/2), width=BUTTON_WIDTH, height=BUTTON_HEIGHT)

bg_color_label = Label(master=profiles_lf, text="Background color:")
bg_color_label.pack()
bg_color_label.place(x=20, y=355)

bg_color_button = Label(master=profiles_lf, borderwidth=1, relief="solid")
bg_color_button.pack()
bg_color_button.place(x=160, y=356, width=60, height=20)
bg_color_button.bind("<Button-1>", bg_color_click)

kd_color_label = Label(master=profiles_lf, text="Activation color:")
kd_color_label.pack()
kd_color_label.place(x=20, y=380)

kd_color_button = Label(master=profiles_lf, borderwidth=1, relief="solid")
kd_color_button.pack()
kd_color_button.place(x=160, y=407, width=60, height=20)
kd_color_button.bind("<Button-1>", kd_color_click)

dim_unused_keys_checkbox_var = IntVar()
dim_unused_keys_checkbox = Checkbutton(profiles_lf, text="Dim unused keys", variable=dim_unused_keys_checkbox_var, command=dim_unused_keys_click)
dim_unused_keys_checkbox.pack()
dim_unused_keys_checkbox.place(x=22, y=425)

kd_color_var = IntVar()
kd_R1 = Radiobutton(profiles_lf, text="      Auto", variable=kd_color_var, value=0, command=kd_radiobutton_auto_click)
kd_R1.pack()
kd_R1.place(x=130, y=380)
kd_R2 = Radiobutton(profiles_lf, text="", variable=kd_color_var, value=1, command=kd_radiobutton_custom_click)
kd_R2.pack()
kd_R2.place(x=130, y=405)

# ------------- Keys frame -------------
selected_key = None

keys_lf = LabelFrame(root, text="Keys", width=int(MAIN_WINDOW_WIDTH / 3 - PADDING * 1.3), height=MAIN_WINDOW_HEIGHT - HIGHT_ROOT_FOLDER_LF - PADDING)
keys_lf.pack()
keys_lf.place(x=profiles_lf.winfo_x() + profiles_lf.winfo_width() + PADDING, y=profiles_lf.winfo_y())
keys_lf.pack_propagate(False) 
root.update()

key_instruction = Label(master=keys_lf, text="click to edit, drag to rearrange")
key_instruction.pack()
root.update()
key_instruction.place(x=(keys_lf.winfo_width() - key_instruction.winfo_width())/2, y=0)

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
    if len(profile_listbox.curselection()) <= 0:
        return
    profile_index = profile_listbox.curselection()[0]
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
    if len(profile_listbox.curselection()) <= 0:
        return
    profile_index = profile_listbox.curselection()[0]
    for index, item in enumerate(profile_list[profile_index].keylist):
        if item is not None:
            item.index = index + 1

def button_drag_release(event):
    global drag_source_button_index
    global drag_destination_button_index
    if len(profile_listbox.curselection()) <= 0:
        return
    if drag_source_button_index == drag_destination_button_index:
        return
    # print('source:', drag_source_button_index)
    # print('destination:', drag_destination_button_index)
    # print('------')
    profile_index = profile_listbox.curselection()[0]
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
    this_button = Label(master=keys_lf, borderwidth=1, relief="solid", background='SystemButtonFace', font=(None, 13))
    this_button.pack()
    this_button.place(x=key_button_xy_list[x][0], y=key_button_xy_list[x][1], width=KEY_BUTTON_WIDTH, height=KEY_BUTTON_HEIGHT)
    this_button.bind("<Button-1>", key_button_click_event)
    this_button.bind("<B1-Motion>", button_drag_start)
    this_button.bind("<ButtonRelease-1>", button_drag_release)
    key_button_list.append(this_button)

key_name_text = Label(master=keys_lf, text="Key name:")
key_name_text.pack()
key_name_text.place(x=PADDING, y=305)
root.update()

key_name_entrybox = Entry(keys_lf)
key_name_entrybox.pack()
key_name_entrybox.place(x=key_name_text.winfo_width()+PADDING, y=305, width=145)

KEY_BUTTON_GAP = int((keys_lf.winfo_width() - 2 * BUTTON_WIDTH) / 3.5)

def key_rename_click():
    if is_key_selected() == False:
        return
    profile_index = profile_listbox.curselection()[0]
    result = clean_input(key_name_entrybox.get(), 7)
    if len(result) <= 0 or result in [x.name for x in profile_list[profile_index].keylist if x is not None]:
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
    profile_index = profile_listbox.curselection()[0]
    profile_list[profile_index].keylist[selected_key] = None
    update_key_button_appearances(profile_index)
    key_button_click(key_button_list[selected_key])

key_rename_button = Button(keys_lf, text="Apply", command=key_rename_click) #, state=DISABLED
key_rename_button.pack()
key_rename_button.place(x=KEY_BUTTON_GAP, y=340, width=BUTTON_WIDTH, height=BUTTON_HEIGHT)
root.update()
key_remove_button = Button(keys_lf, text="Remove", command=key_remove_click) #, state=DISABLED
key_remove_button.pack()
key_remove_button.place(x=KEY_BUTTON_GAP*2+key_rename_button.winfo_width(), y=340, width=BUTTON_WIDTH, height=BUTTON_HEIGHT)

key_name_text = Label(master=keys_lf, text="Key color:")
key_name_text.pack()
key_name_text.place(x=PADDING, y=380)
root.update()

def key_color_rb1_click():
    if is_key_selected() == False:
        return
    profile_index = profile_listbox.curselection()[0]
    if profile_list[profile_index].keylist[selected_key] is not None:
        profile_list[profile_index].keylist[selected_key].color = None
    update_key_button_appearances(profile_index)
    key_button_click(key_button_list[selected_key])

def key_color_rb2_click():
    if is_key_selected() == False:
        return
    profile_index = profile_listbox.curselection()[0]
    if profile_list[profile_index].keylist[selected_key] is not None:
        profile_list[profile_index].keylist[selected_key].color = last_rgb
    update_key_button_appearances(profile_index)
    key_button_click(key_button_list[selected_key])

def is_key_selected():
    if len(profile_listbox.curselection()) <= 0:
        return False
    if selected_key is None:
        return False
    return True

def key_color_button_click(event):
    global last_rgb
    if is_key_selected() == False:
        return
    profile_index = profile_listbox.curselection()[0]
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
key_color_rb1.pack()
key_color_rb1.place(x=85, y=380)
key_color_rb2 = Radiobutton(keys_lf, text="", variable=key_color_type_var, value=1, command=key_color_rb2_click, state=DISABLED)
key_color_rb2.pack()
key_color_rb2.place(x=85, y=405)

key_color_button = Label(master=keys_lf, borderwidth=1, relief="solid")
key_color_button.pack()
key_color_button.place(x=135, y=407, width=60, height=20)
key_color_button.bind("<Button-1>", key_color_button_click)

# ------------- Scripts frame -------------
scripts_lf = LabelFrame(root, text="Scripts", width=int(MAIN_WINDOW_WIDTH / 3 - PADDING * 1.3), height=MAIN_WINDOW_HEIGHT - HIGHT_ROOT_FOLDER_LF - PADDING)
scripts_lf.pack()
scripts_lf.pack_propagate(False)
scripts_lf.place(x=keys_lf.winfo_x() + keys_lf.winfo_width() + PADDING, y=keys_lf.winfo_y())

def open_duckypad_url():
    webbrowser.open('https://github.com/hak5darren/USB-Rubber-Ducky/wiki/Duckyscript')

def script_instruction_click(event):
    open_duckypad_url()

script_instruction = Label(master=scripts_lf, text="Read more about Duckyscript...", fg="blue", cursor="hand2")
script_instruction.pack()
script_instruction.pack_propagate(False)
root.update()
script_instruction.place(x=(scripts_lf.winfo_width() - script_instruction.winfo_width())/2, y=0)
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
    profile_index = profile_listbox.curselection()[0]
    cantthinkofaname = ""
    if modified_count - key_button_clicked_at > 2:
        if profile_list[profile_index].keylist[selected_key] is not None:
            cantthinkofaname = "Checking..."
        syntax_check_result_label.config(text=cantthinkofaname, fg="black")
    if profile_list[profile_index].keylist[selected_key] is not None:
        profile_list[profile_index].keylist[selected_key].script = script_textbox.get(1.0, END).replace('\r','').strip().strip('\n')
        modification_checked = 0
    
def script_textbox_event(event):
    script_textbox_modified()
    script_textbox.tk.call(script_textbox._w, 'edit', 'modified', 0)

script_textbox = Text(scripts_lf, relief='solid', borderwidth=1, padx=2, pady=2, spacing3=5, wrap="word", state=DISABLED)
script_textbox.pack()
script_textbox.pack_propagate(False)
script_textbox.place(x=key_button_list[0].winfo_x(), y=KEY_BUTTON_HEADROOM+PADDING-3, width=key_button_list[-1].winfo_x() + KEY_BUTTON_WIDTH - KEY_BUTTON_GAP, height=key_button_list[-1].winfo_y() - key_button_list[0].winfo_y() + KEY_BUTTON_HEIGHT + 5)
root.update()
script_textbox.bind("<<Modified>>", script_textbox_event)
script_textbox.tag_configure("error", background="#ffff00")

script_common_commands_lf = LabelFrame(scripts_lf, text="Common commands", width=script_textbox.winfo_width(), height=105)
script_common_commands_lf.pack()
script_common_commands_lf.pack_propagate(False)
script_common_commands_lf.place(x=PADDING, y=300)
root.update()

check_syntax_lf = LabelFrame(scripts_lf, text="Code check", width=script_textbox.winfo_width(), height=40)
check_syntax_lf.pack()
check_syntax_lf.pack_propagate(False)
check_syntax_lf.place(x=PADDING, y=407)
root.update()

SCRIPT_BUTTON_WIDTH = script_textbox.winfo_width()/3.4
SCRIPT_BUTTON_GAP = 5
PADDING = 2

script_button_xy_list = [(SCRIPT_BUTTON_GAP, PADDING), (SCRIPT_BUTTON_GAP*2+SCRIPT_BUTTON_WIDTH, PADDING), (SCRIPT_BUTTON_GAP*3+SCRIPT_BUTTON_WIDTH*2, PADDING), (SCRIPT_BUTTON_GAP, PADDING+BUTTON_HEIGHT+2), (SCRIPT_BUTTON_GAP*2+SCRIPT_BUTTON_WIDTH, PADDING+BUTTON_HEIGHT+2), (SCRIPT_BUTTON_GAP*3+SCRIPT_BUTTON_WIDTH*2, PADDING+BUTTON_HEIGHT+2), (SCRIPT_BUTTON_GAP, (PADDING+BUTTON_HEIGHT)*2+2), (SCRIPT_BUTTON_GAP*2+SCRIPT_BUTTON_WIDTH, (PADDING+BUTTON_HEIGHT)*2+2), (SCRIPT_BUTTON_GAP*3+SCRIPT_BUTTON_WIDTH*2, (PADDING+BUTTON_HEIGHT)*2+2)]
script_button_commands = ["STRING", "CTRL", "SHIFT", "ALT", "GUI", "ENTER", "DELAY", "REPEAT", "more..."]
script_command_button_list = []

for x in range(9):
    def this_func(x=x):
        script_textbox.insert(END, "\n" + script_button_commands[x] + " ")
        script_textbox.see(END)
    this_button = Button(script_common_commands_lf, text=script_button_commands[x], command=this_func) #, state=DISABLED
    this_button.pack()
    this_button.place(x=script_button_xy_list[x][0], y=script_button_xy_list[x][1], width=SCRIPT_BUTTON_WIDTH, height=BUTTON_HEIGHT)
    key_button_list.append(this_button)
key_button_list[-1].config(command=open_duckypad_url)

def check_syntax_click():
    if is_key_selected() == False:
        return
    has_errors = False
    profile_index = profile_listbox.curselection()[0]
    if profile_list[profile_index].keylist[selected_key] is None:
        return
    for count, line in enumerate(profile_list[profile_index].keylist[selected_key].script.split('\n')):
        if ds_syntax_check.parse_line(line) != ds_syntax_check.PARSE_OK:
            print("syntax error on line", count, ':', line)
            script_textbox.tag_add("error", str(count+1)+".0", str(count+1)+".0 lineend")
            script_textbox.mark_set("insert", str(count+1)+".0")
            script_textbox.see(str(count+1)+'.0')
            syntax_check_result_label.config(text='Error(s) found!', fg='red')
            has_errors = True
    if has_errors == False:
        syntax_check_result_label.config(text="It looks alright...", fg="green")

syntax_check_result_label = Label(master=check_syntax_lf)
syntax_check_result_label.pack()
syntax_check_result_label.pack_propagate(False)
syntax_check_result_label.place(x=65, y=-4)

# --------------------------
def repeat_func():
    global modification_checked
    if time.time() - last_textbox_edit >= 0.5 and modification_checked == 0:
        check_syntax_click()
        modification_checked = 1
    root.after(500, repeat_func)

root.after(500, repeat_func)
if os.name == 'posix':
    debug_set_root_folder()
root.mainloop()
