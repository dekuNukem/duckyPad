import os

for x in range(1,16):
	this_file = open("key" + str(x) + '_.txt', 'w')
	this_file.write("GUI r\nDELAY 300\nSTRING notepad.exe\nENTER\nDELAY 300\nSTRING hello world!\n")


