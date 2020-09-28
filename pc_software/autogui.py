import time
import pyautogui

pyautogui.FAILSAFE = False

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
"VOLUP":"volumeup",
"VOLDOWN":"volumedown",
"MENU":"apps",
"APP":"apps",
"MUTE":"volumemute"}

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

default_cmd_delay_ms = 17
default_char_delay_ms = 17
prev_line = ""

PARSE_OK = 0
PARSE_ERROR = 1
PARSE_EMPTY_LINE = 3

def parse_combo(combo_line):
	combo_keys = combo_line.split(' ')
	if len(combo_keys) > 3:
		return 1, "too many combos, up to 3 supported"
	for item in [x.lower() for x in combo_keys if x not in autogui_map.keys()]:
		if item not in valid_chars:
			return 1, "invalid key combos"
	autogui_args = []
	for item in combo_keys:
		if item in autogui_map:
			autogui_args.append(autogui_map[item])
		else:
			autogui_args.append(item.lower())
	pyautogui.hotkey(*autogui_args, interval=default_char_delay_ms/1000)
	return 0, ""

def parse_line(ducky_line):
	global default_cmd_delay_ms
	global default_char_delay_ms
	parse_result = PARSE_OK
	parse_note = ""
	ducky_line = ducky_line.replace('\n', '').replace('\r', '')
	if not (ducky_line.startswith(cmd_STRING) or ducky_line.startswith(cmd_REM)):
		ducky_line = ducky_line.strip()
	if len(ducky_line) <= 0:
		return PARSE_EMPTY_LINE, parse_note
	elif ducky_line.startswith(cmd_REM) or ducky_line.startswith(cmd_UARTPRINT):
		return PARSE_OK, parse_note
	elif ducky_line.startswith(cmd_STRING):
		pyautogui.write(ducky_line[len(cmd_STRING):], interval=default_char_delay_ms/1000)
	elif ducky_line.startswith(cmd_REPEAT):
		for x in range(int(ducky_line[len(cmd_REPEAT):].strip())):
			parse_line(prev_line)
	elif ducky_line.startswith(cmd_DELAY):
		time.sleep(int(ducky_line[len(cmd_DELAY):].strip())/1000)
	elif ducky_line.startswith(cmd_DEFAULTDELAY):
		default_cmd_delay_ms = int(ducky_line[len(cmd_DEFAULTDELAY):].strip())
	elif ducky_line.startswith(cmd_DEFAULT_DELAY):
		default_cmd_delay_ms = int(ducky_line[len(cmd_DEFAULT_DELAY):].strip())
	elif ducky_line.startswith(cmd_DEFAULTCHARDELAY):
		default_char_delay_ms = int(ducky_line[len(cmd_DEFAULTCHARDELAY):].strip())
	elif ducky_line.split(' ')[0] in autogui_map.keys():
		parse_result, parse_note = parse_combo(ducky_line)
	else:
		parse_result = PARSE_ERROR
		parse_note = 'invalid command'
	return parse_result, parse_note

def execute_file(str_list):
	global prev_line
	for index, line in enumerate(str_list):
		try:
			parse_result, parse_note = parse_line(line)
		except Exception as e:
			return PARSE_ERROR, "parse error on line " + str(index + 1) + ':\n\n' + line + '\n\n' + str(e)
		if parse_result == PARSE_OK and not line.startswith(cmd_REPEAT):
			prev_line = line
		if parse_result == PARSE_ERROR:
			return PARSE_ERROR, "parse error on line " + str(index + 1) + ':\n\n' + line + '\n\n' + parse_note
		time.sleep(default_cmd_delay_ms/1000)
	return 0, ''

# content = None
# with open('key7.txt') as fff:
# 	content = fff.readlines()

# print(execute_file(content)[-1])
