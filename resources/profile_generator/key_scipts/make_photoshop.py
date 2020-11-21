import os

cmd_list = ["Esc", "Move", "Wand", "Select", "Lasso", "Pen", "Crop", "Eraser", "Text", "Brush", "History", "Grid", "Undo", "Redo", "Save"]

for index, item in enumerate(cmd_list):
	this_file = open("key" + str(index+1) + '_' + item.lower() +'.txt', 'w')
	this_file.write("GUI r\nDELAY 300\nSTRING notepad.exe\nENTER\nDELAY 300\nSTRING hello world!\n")


