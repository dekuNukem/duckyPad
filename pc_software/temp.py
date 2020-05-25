"""
pack BEFORE place

"""		
def profile_shift_up():
	global profile_var
	global profile_list
	selection = profile_listbox.curselection()
	if len(selection) <= 0 or selection[0] == 0:
		print("profile_shift_up: nothing to do")
		return
	source = selection[0]
	destination = selection[0] - 1
	print("source: ", source)
	print("destination: ", destination)
	print([x.name for x in profile_list])
	profile_list[destination], profile_list[source] = profile_list[source], profile_list[destination]
	print([x.name for x in profile_list])
	profile_var.set([x.name for x in profile_list])
	profile_listbox.selection_clear(0, len(profile_list))
	profile_listbox.selection_set(destination)
my_dirs = [d for d in os.listdir(dp_root_folder_path) if os.path.isdir(os.path.join(dp_root_folder_path, d))]
	my_dirs.sort()
	my_dirs = [d for d in my_dirs if d.startswith("profile")]
	profile_var.set(my_dirs)

result = build_profile("/Users/allen/Desktop/test_profiles")
for item in result:
	print(item)

print(folder_name.split('profile')[-1].split(''))

print(self.path)
		print(self.name)
		print(self.index)
		print(self.color)
		print(self.script)
		print('...............')

				# print(self.path)
		# print(self.name)
		# print(self.keys)
		# print(self.bg_color)
		# print(self.kd_color)
		# print(self.dim_unused)
		print('-----------')

default_bg_color = (64, 126, 255)
default_keydown_color = (255, 255, 255)
from tkinter import messagebox

	messagebox.showinfo("Say Hello", "Hello World")

profile_buf = []
profile_var = StringVar(value=profile_buf)

mylist = ['one', 'two', 'three']
var = StringVar(value=mylist)

profile_listbox = Listbox(profiles_lf, listvariable=var)
profile_listbox.pack()
profile_listbox.insert(1, "Nachos") 
profile_listbox.insert(2, "Sandwich") 
profile_listbox.insert(3, "Burger") 
profile_listbox.insert(4, "Pizza") 


def check_root_folder():
	global dp_root_folder_path
	if len(dp_root_folder_path.get()) <= 0:
		print("invalid root folder: empty path")
	root.after(500, check_root_folder)


	# root.after(500, check_root_folder)

def gui_update_non_block():
    root.update()
    root.update_idletasks()

while 1:
	gui_update_non_block()
###

root = Tk()
root.title("duckyPad configurator")
root.geometry(str(MAIN_WINDOW_WIDTH) + "x" + str(MAIN_WINDOW_HEIGHT))
root.resizable(width=FALSE, height=FALSE)

lf = LabelFrame(root, text="duckyPad Folder", width=300, height=300)
lf.pack(fill="both", expand=True)
lf.place(x=100, y=100)

# t = Label(lf, text="")
# t.grid()
working_folder_select_button = Button(lf, text = "Select...")
working_folder_select_button.place(x=100, y=100)
# working_folder_select_button.grid(row=0, column=0, stick='w') 


############

import os
import sys
import time
import random
from tkinter import *

MAIN_WINDOW_WIDTH = 800
MAIN_WINDOW_HEIGHT = 533


root = Tk()
root.title("duckyPad configurator")
root.geometry(str(MAIN_WINDOW_WIDTH) + "x" + str(MAIN_WINDOW_HEIGHT))
root.resizable(width=FALSE, height=FALSE)

w = Label(root, text="Hello, world!")
w.pack()

lf = LabelFrame(root, text="Specialist:") 
lf.pack()


t = Label(lf, text="Choose your specialist:")
t.grid(columnspan=2, stick='w')

specialistchoose = IntVar()

r1 = Radiobutton(lf, text="Cardiology", variable=specialistchoose, value=1)
r1.grid(row=1, column=0, stick='w') 

r2 = Radiobutton(lf, text="Gastroenterology", variable=specialistchoose, value=2)
r2.grid(row=1, column=1, stick='w') 

r3 = Radiobutton(lf, text="Dermatology", variable=specialistchoose, value=3)
r3.grid(row=1, column=2, stick='w')

r4 = Radiobutton(lf, text="Psychiatry", variable=specialistchoose, value=4)
r4.grid(row=2, column=0, stick='w')

r5 = Radiobutton(lf, text="Dentist", variable=specialistchoose, value=5)
r5.grid(row=2, column=1, stick='w')


root.mainloop()
####################