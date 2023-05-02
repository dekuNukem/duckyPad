import os
import autogui

valid_chars = ['!', '"', '#', '$', '%', '&', "'", '(',
')', '*', '+', ',', '-', '.', '/', '0', '1', '2', '3', '4', '5', '6', '7',
'8', '9', ':', ';', '<', '=', '>', '?', '@', '[', '\\', ']', '^', '_', '`',
'a', 'b', 'c', 'd', 'e','f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', '~']

cmd_REPEAT = "REPEAT "
cmd_REM = "REM "
cmd_DEFAULTDELAY = "DEFAULTDELAY "
cmd_DEFAULTCHARDELAY = "DEFAULTCHARDELAY "
cmd_DELAY = "DELAY "
cmd_STRING = "STRING "
cmd_STRINGLN = "STRINGLN "

cmd_EMUK = "EMUK "
cmd_LOOP = "LOOP"
cmd_SWCOLOR = "SWCOLOR_";
cmd_SW_SELF_COLOR = "SWCOLOR ";

cmd_LMOUSE = "LMOUSE"
cmd_RMOUSE = "RMOUSE"
cmd_MMOUSE = "MMOUSE"
cmd_MOUSE_MOVE = "MOUSE_MOVE"
cmd_MOUSE_WHEEL = "MOUSE_WHEEL"
cmd_KEYDOWN = "KEYDOWN ";
cmd_KEYUP = "KEYUP ";

DEFAULTDELAYFUZZ = "DEFAULTDELAYFUZZ ";
DEFAULTCHARDELAYFUZZ = "DEFAULTCHARDELAYFUZZ ";

mouse_commands = [cmd_LMOUSE, cmd_RMOUSE, cmd_MMOUSE, cmd_MOUSE_MOVE, cmd_MOUSE_WHEEL]

ignored_but_valid_commands = ["UARTPRINT ", "LCR", DEFAULTDELAYFUZZ, DEFAULTCHARDELAYFUZZ, cmd_REM, "SWCOLOR_", "SWCOLOR ", 'DP_SLEEP', 'PREV_PROFILE', 'NEXT_PROFILE', 'GOTO_PROFILE ', 'INJECT_MOD', "KATAKANAHIRAGANA", "HENKAN", "MUHENKAN", "KATAKANA", "HIRAGANA", "ZENKAKUHANKAKU"]
def is_ignored_but_valid_command(ducky_line):
	for item in ignored_but_valid_commands:
		if ducky_line.startswith(item):
			return True
	return False

PARSE_OK = 0
PARSE_ERROR = 1

def parse_combo(combo_line):
	combo_keys = combo_line.split(' ')
	if len(combo_keys) > 6:
		return PARSE_ERROR, 'No more than 6 combos'
	for item in [x.lower() for x in combo_keys if x not in autogui.autogui_map.keys()]:
		if item not in valid_chars:
			return PARSE_ERROR, 'Invalid combo character'
	return PARSE_OK, 'Success'

def parse_mouse(ducky_line):
	mouse_command_list = [x for x in mouse_commands if x in ducky_line]
	if len(mouse_command_list) != 1:
		return PARSE_ERROR, 'Mouse command too many arguments'
	this_mouse_command = mouse_command_list[0]
	if this_mouse_command == cmd_LMOUSE:
		return PARSE_OK, "Success"
	elif this_mouse_command == cmd_RMOUSE:
		return PARSE_OK, "Success"
	elif this_mouse_command == cmd_MMOUSE:
		return PARSE_OK, "Success"
	elif this_mouse_command == cmd_MOUSE_MOVE:
		try:
			x_amount = int(ducky_line.split(' ')[1])
			y_amount = int(ducky_line.split(' ')[2])
			if x_amount > 127 or x_amount < -127:
				raise ValueError
			if y_amount > 127 or y_amount < -127:
				raise ValueError
		except:
			return PARSE_ERROR, 'MOUSE_MOVE value error: should be -127 to 127'
		return PARSE_OK, "Success"
	elif this_mouse_command == cmd_MOUSE_WHEEL:
		try:
			amount = int(ducky_line.split(' ')[1])
			if amount > 127 or amount < -127:
				raise ValueError
		except:
			return PARSE_ERROR, 'MOUSE_WHEEL value error: should be -127 to 127'
		return PARSE_OK, "Success"
	return PARSE_ERROR, "Invalid mouse command"

def parse_line(ducky_line, is_fw_before_emuk=False):
	global cmd_EMUK
	if is_fw_before_emuk:
		cmd_EMUK = "HOLD "
	else:
		cmd_EMUK = "EMUK "
	parse_result = PARSE_OK
	parse_message = 'Unknown'
	ducky_line = ducky_line.replace('\n', '').replace('\r', '')
	if not (ducky_line.startswith(cmd_STRING) or ducky_line.startswith(cmd_STRINGLN) or ducky_line.startswith(cmd_REM)):
		ducky_line = ducky_line.strip()
	if len(ducky_line) <= 0:
		return PARSE_OK, "Success"
	elif len(ducky_line) >= 250:
		return PARSE_ERROR, "Line too long, max 250 characters per line."
	elif is_ignored_but_valid_command(ducky_line):
		return PARSE_OK, "Success"
	elif ducky_line.startswith(cmd_KEYDOWN):
		parse_result, parse_message = parse_combo(ducky_line[len(cmd_KEYDOWN):])
	elif ducky_line.startswith(cmd_KEYUP):
		parse_result, parse_message = parse_combo(ducky_line[len(cmd_KEYUP):])
	elif ducky_line.startswith(cmd_LOOP) and ducky_line.endswith(':') and len(ducky_line) == 6 and ducky_line[4].isnumeric():
		return PARSE_OK, "Success"
	elif ducky_line.startswith(cmd_EMUK):
		sssss = ducky_line[len(cmd_EMUK):].strip().split(' ')
		if len(sssss) > 2:
			return PARSE_ERROR, "EMUK max 2 keys"
		if sssss[0] in autogui.autogui_map.keys() or sssss[0] in mouse_commands[:3]:
			return PARSE_OK, "Success"
		elif len(sssss[0]) == 1:
			return PARSE_OK, "Success"
		return PARSE_ERROR, "EMUK invalid key"
	elif ducky_line.startswith(cmd_STRING) or ducky_line.startswith(cmd_STRINGLN):
		return PARSE_OK, "Success"
	elif ducky_line.startswith(cmd_REPEAT):
		try:
			int(ducky_line[len(cmd_REPEAT):].strip())
		except Exception:
			return PARSE_ERROR, "REPEAT missing argument"
		return PARSE_OK, "Success"
	elif ducky_line.startswith(cmd_DELAY):
		try:
			if int(ducky_line[len(cmd_DELAY):].strip())/1000 <= 0:
				return PARSE_ERROR, "DELAY can't be zero or negative"
			else:
				return PARSE_OK, "Success"
		except Exception:
			return PARSE_ERROR, "DELAY invalid argument"
	elif ducky_line.startswith(cmd_DEFAULTDELAY):
		try:
			if int(ducky_line[len(cmd_DEFAULTDELAY):].strip()) <= 0:
				return PARSE_ERROR, "DEFAULTDELAY can't be zero or negative"
			else:
				return PARSE_OK, "Success"
		except Exception:
			return PARSE_ERROR, "DEFAULTDELAY invalid argument"
	elif ducky_line.startswith(cmd_DEFAULTCHARDELAY):
		try:
			if int(ducky_line[len(cmd_DEFAULTCHARDELAY):].strip()) <= 0:
				return PARSE_ERROR, "DEFAULTCHARDELAY can't be zero or negative"
			else:
				return PARSE_OK, "Success"
		except Exception:
			return PARSE_ERROR, "DEFAULTCHARDELAY invalid argument"
	elif ducky_line.split(' ')[0] in autogui.autogui_map.keys():
		parse_result, parse_message = parse_combo(ducky_line)
	elif ducky_line.split(' ')[0] in mouse_commands:
		parse_result, parse_message = parse_mouse(ducky_line)
	else:
		parse_result = PARSE_ERROR
		parse_message = "Invalid Command"
	return parse_result, parse_message

# content = ''
# with open('test_script.txt') as script_file:
# 	content = script_file.readlines()

# for index, line in enumerate(content):
	# if parse_line(line) != PARSE_OK:
	# 	print("error on line", index, ':', line)
	# print(parse_line(line))
