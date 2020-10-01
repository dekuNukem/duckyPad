import os

autogui_map = {"ESCAPE":"escape",
"ESC":"esc",
"ENTER":"enter",
"UP":"up",
"DOWN":"down",
"LEFT":"left",
"RIGHT":"right",
"UPARROW":"up",
"DOWNARROW":"down",
"LEFTARROW":"left",
"RIGHTARROW":"right",
"F1":"f1",
"F2":"f2",
"F3":"f3",
"F4":"f4",
"F5":"f5",
"F6":"f6",
"F7":"f7",
"F8":"f8",
"F9":"f9",
"F10":"f10",
"F11":"f11",
"F12":"f12",
"BACKSPACE":"backspace",
"TAB":"tab",
"CAPSLOCK":"capslock",
"PRINTSCREEN":"printscreen",
"SCROLLLOCK":"scrolllock",
"PAUSE":"pause",
"BREAK":"pause",
"INSERT":"insert",
"HOME":"home",
"PAGEUP":"pageup",
"PAGEDOWN":"pagedown",
"DELETE":"delete",
"END":"end",
"SPACE":"space",
"SHIFT":"shift",
"ALT":"alt",
"GUI":"win",
"WINDOWS":"win",
"CONTROL":"ctrl",
"CTRL":"ctrl",
"MK_VOLUP":"volumeup",
"MK_VOLDOWN":"volumedown",
"MK_MUTE":"volumemute",
"MK_PREV":"prevtrack",
"MK_NEXT":"nexttrack",
"MK_PP":"playpause",
"MK_STOP":"stop",
"MENU":"apps",
"APP":"apps"
}

valid_chars = ['!', '"', '#', '$', '%', '&', "'", '(',
')', '*', '+', ',', '-', '.', '/', '0', '1', '2', '3', '4', '5', '6', '7',
'8', '9', ':', ';', '<', '=', '>', '?', '@', '[', '\\', ']', '^', '_', '`',
'a', 'b', 'c', 'd', 'e','f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', '~']

cmd_REPEAT = "REPEAT "
cmd_REM = "REM "
cmd_DEFAULTDELAY = "DEFAULTDELAY "
cmd_DEFAULT_DELAY = "DEFAULT_DELAY "
cmd_DEFAULTCHARDELAY = "DEFAULTCHARDELAY "
cmd_DELAY = "DELAY "
cmd_STRING = "STRING "
cmd_UARTPRINT = "UARTPRINT "

PARSE_OK = 0
PARSE_ERROR = 1
PARSE_EMPTY_LINE = 3

def parse_combo(combo_line):
	combo_keys = combo_line.split(' ')
	if len(combo_keys) > 3:
		return 1
	for item in [x.lower() for x in combo_keys if x not in autogui_map.keys()]:
		if item not in valid_chars:
			return 1
	autogui_args = []
	for item in combo_keys:
		if item in autogui_map:
			autogui_args.append(autogui_map[item])
		else:
			autogui_args.append(item.lower())
	return 0

def parse_line(ducky_line):
	parse_result = PARSE_OK
	ducky_line = ducky_line.replace('\n', '').replace('\r', '')
	if not (ducky_line.startswith(cmd_STRING) or ducky_line.startswith(cmd_REM)):
		ducky_line = ducky_line.strip()
	if len(ducky_line) <= 0:
		return PARSE_OK
	elif len(ducky_line) >= 254:
		return PARSE_ERROR
	elif ducky_line.startswith(cmd_REM) or ducky_line.startswith(cmd_UARTPRINT):
		return PARSE_OK
	elif ducky_line.startswith(cmd_STRING):
		return PARSE_OK
	elif ducky_line.startswith(cmd_REPEAT):
		for x in range(int(ducky_line[len(cmd_REPEAT):].strip())):
			parse_line(prev_line)
	elif ducky_line.startswith(cmd_DELAY):
		to_sleep = int(ducky_line[len(cmd_DELAY):].strip())/1000
	elif ducky_line.startswith(cmd_DEFAULTDELAY):
		default_cmd_delay_ms = int(ducky_line[len(cmd_DEFAULTDELAY):].strip())
	elif ducky_line.startswith(cmd_DEFAULT_DELAY):
		default_cmd_delay_ms = int(ducky_line[len(cmd_DEFAULT_DELAY):].strip())
	elif ducky_line.startswith(cmd_DEFAULTCHARDELAY):
		default_char_delay_ms = int(ducky_line[len(cmd_DEFAULTCHARDELAY):].strip())
	elif ducky_line.split(' ')[0] in autogui_map.keys():
		parse_result = parse_combo(ducky_line)
	else:
		parse_result = PARSE_ERROR
	return parse_result

# content = ''
# with open('test_script.txt') as script_file:
# 	content = script_file.readlines()

# for index, line in enumerate(content):
	# if parse_line(line) != PARSE_OK:
	# 	print("error on line", index, ':', line)
	# print(parse_line(line))
