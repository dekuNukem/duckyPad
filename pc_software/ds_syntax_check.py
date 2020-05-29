import os


# quick and dirty translation from parse.c

KEY_LEFT_CTRL = 0x80
KEY_LEFT_SHIFT = 0x81
KEY_LEFT_ALT = 0x82
KEY_LEFT_GUI = 0x83
KEY_RIGHT_CTRL = 0x84
KEY_RIGHT_SHIFT = 0x85
KEY_RIGHT_ALT = 0x86
KEY_RIGHT_GUI = 0x87
KEY_RETURN = 0x28+0x88
KEY_ESC = 0x29+0x88
KEY_BACKSPACE = 0x2A+0x88
KEY_TAB = 0x2B+0x88
KEY_CAPS_LOCK = 0x39+0x88
KEY_F1 = 0x3A+0x88
KEY_F2 = 0x3B+0x88
KEY_F3 = 0x3C+0x88
KEY_F4 = 0x3D+0x88
KEY_F5 = 0x3E+0x88
KEY_F6 = 0x3F+0x88
KEY_F7 = 0x40+0x88
KEY_F8 = 0x41+0x88
KEY_F9 = 0x42+0x88
KEY_F10 = 0x43+0x88
KEY_F11 = 0x44+0x88
KEY_F12 = 0x45+0x88
KEY_PRINT_SCREEN = 0x46+0x88
KEY_SCROLL_LOCK = 0x47+0x88
KEY_PAUSE = 0x48+0x88
KEY_INSERT = 0x49+0x88
KEY_HOME = 0x4A+0x88
KEY_PAGE_UP = 0x4B+0x88
KEY_DELETE = 0x4C+0x88
KEY_END = 0x4D+0x88
KEY_PAGE_DOWN = 0x4E+0x88
KEY_RIGHT_ARROW = 0x4F+0x88
KEY_LEFT_ARROW = 0x50+0x88
KEY_DOWN_ARROW = 0x51+0x88
KEY_UP_ARROW = 0x52+0x88
KEY_VOL_UP = 0xff
KEY_VOL_DOWN = 0xfe
KEY_VOL_MUTE = 0xfd

cmd_NAME = "NAME "
cmd_REPEAT = "REPEAT "
cmd_REM = "REM "
cmd_C_COMMENT = "// "
cmd_DEFAULTDELAY = "DEFAULTDELAY "
cmd_DEFAULTCHARDELAY = "DEFAULTCHARDELAY "
cmd_DELAY = "DELAY "
cmd_STRING = "STRING "
cmd_ESCAPE = "ESCAPE"
cmd_ESC = "ESC"
cmd_ENTER = "ENTER"
cmd_UP = "UP"
cmd_DOWN = "DOWN"
cmd_LEFT = "LEFT"
cmd_RIGHT = "RIGHT"
cmd_UPARROW = "UPARROW"
cmd_DOWNARROW = "DOWNARROW"
cmd_LEFTARROW = "LEFTARROW"
cmd_RIGHTARROW = "RIGHTARROW"
cmd_F1 = "F1"
cmd_F2 = "F2"
cmd_F3 = "F3"
cmd_F4 = "F4"
cmd_F5 = "F5"
cmd_F6 = "F6"
cmd_F7 = "F7"
cmd_F8 = "F8"
cmd_F9 = "F9"
cmd_F10 = "F10"
cmd_F11 = "F11"
cmd_F12 = "F12"
cmd_BACKSPACE = "BACKSPACE"
cmd_TAB = "TAB"
cmd_CAPSLOCK = "CAPSLOCK"
cmd_PRINTSCREEN = "PRINTSCREEN"
cmd_SCROLLLOCK = "SCROLLLOCK"
cmd_PAUSE = "PAUSE"
cmd_BREAK = "BREAK"
cmd_INSERT = "INSERT"
cmd_HOME = "HOME"
cmd_PAGEUP = "PAGEUP"
cmd_PAGEDOWN = "PAGEDOWN"
cmd_DELETE = "DELETE"
cmd_END = "END"
cmd_SPACE = "SPACE"
cmd_SHIFT = "SHIFT"
cmd_ALT = "ALT"
cmd_GUI = "GUI"
cmd_WINDOWS = "WINDOWS"
cmd_CONTROL = "CONTROL"
cmd_CTRL = "CTRL"
cmd_BG_COLOR = "BG_COLOR "
cmd_KD_COLOR = "KEYDOWN_COLOR "
cmd_SWCOLOR = "SWCOLOR_"
cmd_DIM_UNUSED_KEYS = "DIM_UNUSED_KEYS "
cmd_VOLUP = "VOLUP"
cmd_VOLDOWN = "VOLDOWN"
cmd_VOLMUTE = "MUTE"

PARSE_OK = 0
PARSE_ERROR = 1

def parse_special_key(msg):
	if(len(os.path.commonprefix([cmd_F10, msg])) == len(cmd_F10)):
		return KEY_F10
	elif(len(os.path.commonprefix([cmd_F11, msg])) == len(cmd_F11)):
		return KEY_F11
	elif(len(os.path.commonprefix([cmd_F12, msg])) == len(cmd_F12)):
		return KEY_F12
	elif(len(os.path.commonprefix([cmd_F1, msg])) == len(cmd_F1)):
		return KEY_F1
	elif(len(os.path.commonprefix([cmd_F2, msg])) == len(cmd_F2)):
		return KEY_F2
	elif(len(os.path.commonprefix([cmd_F3, msg])) == len(cmd_F3)):
		return KEY_F3
	elif(len(os.path.commonprefix([cmd_F4, msg])) == len(cmd_F4)):
		return KEY_F4
	elif(len(os.path.commonprefix([cmd_F5, msg])) == len(cmd_F5)):
		return KEY_F5
	elif(len(os.path.commonprefix([cmd_F6, msg])) == len(cmd_F6)):
		return KEY_F6
	elif(len(os.path.commonprefix([cmd_F7, msg])) == len(cmd_F7)):
		return KEY_F7
	elif(len(os.path.commonprefix([cmd_F8, msg])) == len(cmd_F8)):
		return KEY_F8
	elif(len(os.path.commonprefix([cmd_F9, msg])) == len(cmd_F9)):
		return KEY_F9
	elif(len(os.path.commonprefix([cmd_UP, msg])) == len(cmd_UP)):
		return KEY_UP_ARROW
	elif(len(os.path.commonprefix([cmd_DOWN, msg])) == len(cmd_DOWN)):
		return KEY_DOWN_ARROW
	elif(len(os.path.commonprefix([cmd_LEFT, msg])) == len(cmd_LEFT)):
		return KEY_LEFT_ARROW
	elif(len(os.path.commonprefix([cmd_RIGHT, msg])) == len(cmd_RIGHT)):
		return KEY_RIGHT_ARROW
	elif(len(os.path.commonprefix([cmd_UPARROW, msg])) == len(cmd_UPARROW)):
		return KEY_UP_ARROW
	elif(len(os.path.commonprefix([cmd_DOWNARROW, msg])) == len(cmd_DOWNARROW)):
		return KEY_DOWN_ARROW
	elif(len(os.path.commonprefix([cmd_LEFTARROW, msg])) == len(cmd_LEFTARROW)):
		return KEY_LEFT_ARROW
	elif(len(os.path.commonprefix([cmd_RIGHTARROW, msg])) == len(cmd_RIGHTARROW)):
		return KEY_RIGHT_ARROW
	elif(len(os.path.commonprefix([cmd_ESCAPE, msg])) == len(cmd_ESCAPE)):
		return KEY_ESC
	elif(len(os.path.commonprefix([cmd_ESC, msg])) == len(cmd_ESC)):
		return KEY_ESC
	elif(len(os.path.commonprefix([cmd_ENTER, msg])) == len(cmd_ENTER)):
		return KEY_RETURN
	elif(len(os.path.commonprefix([cmd_BACKSPACE, msg])) == len(cmd_BACKSPACE)):
		return KEY_BACKSPACE
	elif(len(os.path.commonprefix([cmd_TAB, msg])) == len(cmd_TAB)):
		return KEY_TAB
	elif(len(os.path.commonprefix([cmd_CAPSLOCK, msg])) == len(cmd_CAPSLOCK)):
		return KEY_CAPS_LOCK
	elif(len(os.path.commonprefix([cmd_PRINTSCREEN, msg])) == len(cmd_PRINTSCREEN)):
		return KEY_PRINT_SCREEN
	elif(len(os.path.commonprefix([cmd_SCROLLLOCK, msg])) == len(cmd_SCROLLLOCK)):
		return KEY_SCROLL_LOCK
	elif(len(os.path.commonprefix([cmd_PAUSE, msg])) == len(cmd_PAUSE)):
		return KEY_PAUSE
	elif(len(os.path.commonprefix([cmd_BREAK, msg])) == len(cmd_BREAK)):
		return KEY_PAUSE
	elif(len(os.path.commonprefix([cmd_INSERT, msg])) == len(cmd_INSERT)):
		return KEY_INSERT
	elif(len(os.path.commonprefix([cmd_HOME, msg])) == len(cmd_HOME)):
		return KEY_HOME
	elif(len(os.path.commonprefix([cmd_PAGEUP, msg])) == len(cmd_PAGEUP)):
		return KEY_PAGE_UP
	elif(len(os.path.commonprefix([cmd_PAGEDOWN, msg])) == len(cmd_PAGEDOWN)):
		return KEY_PAGE_DOWN
	elif(len(os.path.commonprefix([cmd_DELETE, msg])) == len(cmd_DELETE)):
		return KEY_DELETE
	elif(len(os.path.commonprefix([cmd_END, msg])) == len(cmd_END)):
		return KEY_END
	elif(len(os.path.commonprefix([cmd_SHIFT, msg])) == len(cmd_SHIFT)):
		return KEY_LEFT_SHIFT
	elif(len(os.path.commonprefix([cmd_ALT, msg])) == len(cmd_ALT)):
		return KEY_LEFT_ALT
	elif(len(os.path.commonprefix([cmd_GUI, msg])) == len(cmd_GUI)):
		return KEY_LEFT_GUI
	elif(len(os.path.commonprefix([cmd_WINDOWS, msg])) == len(cmd_WINDOWS)):
		return KEY_LEFT_GUI
	elif(len(os.path.commonprefix([cmd_CONTROL, msg])) == len(cmd_CONTROL)):
		return KEY_LEFT_CTRL
	elif(len(os.path.commonprefix([cmd_CTRL, msg])) == len(cmd_CTRL)):
		return KEY_LEFT_CTRL
	elif(len(os.path.commonprefix([cmd_SPACE, msg])) == len(cmd_SPACE)):
		return ' '
	elif(len(os.path.commonprefix([cmd_VOLUP, msg])) == len(cmd_VOLUP)):
		return KEY_VOL_UP
	elif(len(os.path.commonprefix([cmd_VOLDOWN, msg])) == len(cmd_VOLDOWN)):
		return KEY_VOL_DOWN
	elif(len(os.path.commonprefix([cmd_VOLMUTE, msg])) == len(cmd_VOLMUTE)):
		return KEY_VOL_MUTE
	return 0

def is_empty_line(line):
	empty_char = [' ', '\r', '\n', '\t']
	return len(line) == len([x for x in line if x in empty_char])

def parse_line(line):
	result = PARSE_OK
	if is_empty_line(line):
		pass
	elif parse_special_key(line) != 0:
		pass
	elif(len(os.path.commonprefix([cmd_NAME, line])) == len(cmd_NAME)):
		pass
	elif(len(os.path.commonprefix([cmd_REM, line])) == len(cmd_REM)):
		pass
	elif(len(os.path.commonprefix([cmd_C_COMMENT, line])) == len(cmd_C_COMMENT)):
		pass
	elif(len(os.path.commonprefix([cmd_STRING, line])) == len(cmd_STRING)):
		pass
	elif(len(os.path.commonprefix([cmd_DELAY, line])) == len(cmd_DELAY)):
		try:
			int(line.split(cmd_DELAY)[-1].strip('\n').strip('\r'))
		except Exception as e:
			result = PARSE_ERROR
	elif(len(os.path.commonprefix([cmd_DEFAULTDELAY, line])) == len(cmd_DEFAULTDELAY)):
		try:
			int(line.split(cmd_DEFAULTDELAY)[-1].strip('\n').strip('\r'))
		except Exception as e:
			result = PARSE_ERROR
	elif(len(os.path.commonprefix([cmd_DEFAULTCHARDELAY, line])) == len(cmd_DEFAULTCHARDELAY)):
		try:
			int(line.split(cmd_DEFAULTCHARDELAY)[-1].strip('\n').strip('\r'))
		except Exception as e:
			result = PARSE_ERROR
	else:
		result = PARSE_ERROR
	return result

content = ''
with open('test_script.txt') as script_file:
	content = script_file.readlines()

for index, line in enumerate(content):
	if parse_line(line) != PARSE_OK:
		print("error on line", index, ':', line)
