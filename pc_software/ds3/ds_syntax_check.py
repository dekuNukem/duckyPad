import os
from keywords import *

valid_chars = ['!', '"', '#', '$', '%', '&', "'", '(',
')', '*', '+', ',', '-', '.', '/', '0', '1', '2', '3', '4', '5', '6', '7',
'8', '9', ':', ';', '<', '=', '>', '?', '@', '[', '\\', ']', '^', '_', '`',
'a', 'b', 'c', 'd', 'e','f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', '~']

mouse_commands = [cmd_LMOUSE, cmd_RMOUSE, cmd_MMOUSE, cmd_MOUSE_MOVE, cmd_MOUSE_WHEEL]
ignored_but_valid_commands = ["UARTPRINT", "LCR", cmd_REM, 'DP_SLEEP', 'PREV_PROFILE', 'NEXT_PROFILE', 'GOTO_PROFILE ', 'INJECT_MOD']

def is_ignored_but_valid_command(ducky_line):
	for item in ignored_but_valid_commands:
		if ducky_line.startswith(item):
			return True
	return False

PARSE_OK = 0
PARSE_ERROR = 1

def parse_combo(combo_line):
	combo_keys = [x for x in combo_line.split(' ') if len(x) > 0]
	if len(combo_keys) > 6:
		return PARSE_ERROR, 'No more than 6 combos'
	for item in [x.lower() for x in combo_keys if x not in ds3_keyname_dict.keys()]:
		if item not in valid_chars:
			return PARSE_ERROR, 'Invalid combo character'
	return PARSE_OK, 'Success'

def parse_mouse(ducky_line):
	mouse_command_list = [x for x in mouse_commands if x in ducky_line]
	if len(mouse_command_list) != 1:
		return PARSE_ERROR, 'too many arguments'
	this_mouse_command = mouse_command_list[0]
	if this_mouse_command == cmd_LMOUSE:
		return PARSE_OK, "Success"
	elif this_mouse_command == cmd_RMOUSE:
		return PARSE_OK, "Success"
	elif this_mouse_command == cmd_MMOUSE:
		return PARSE_OK, "Success"
	elif this_mouse_command == cmd_MOUSE_MOVE:
		try:
			x_amount = int([x for x in ducky_line.split(' ') if len(x) > 0][1])
			y_amount = int([x for x in ducky_line.split(' ') if len(x) > 0][2])
			if x_amount > 127 or x_amount < -127:
				raise ValueError
			if y_amount > 127 or y_amount < -127:
				raise ValueError
		except:
			return PARSE_ERROR, 'should be between -127 to 127'
		return PARSE_OK, "Success"
	elif this_mouse_command == cmd_MOUSE_WHEEL:
		try:
			amount = int([x for x in ducky_line.split(' ') if len(x) > 0][1])
			if amount > 127 or amount < -127:
				raise ValueError
		except:
			return PARSE_ERROR, 'should be between -127 to 127'
		return PARSE_OK, "Success"
	return PARSE_ERROR, "Invalid mouse command"

def check_color(pgm_line):
	split = [x for x in pgm_line.split(' ') if len(x) > 0]

	if len(split) != 4:
		return PARSE_ERROR, "wrong number of arguments"

	cmd = split[0].strip()
	sw = 0
	try:
		if cmd.startswith("SWCOLOR_"):
			sw = int(cmd.split('_')[1])
			if not 1 <= sw <= 15:
				return PARSE_ERROR, "switch index must be between 1 - 15"
	except Exception as e:
		return PARSE_ERROR, "switch index must be between 1 - 15"

	try:
		for x in range(1, 4):
			if not 0 <= int(split[x]) <= 255:
				return PARSE_ERROR, "value must be between 0 - 255"
	except Exception as e:
		return PARSE_ERROR, "value must be between 0 - 255"

	return PARSE_OK, ""

def parse_line(ducky_line):
	parse_result = PARSE_OK
	parse_message = 'Unknown'
	ducky_line = ducky_line.replace('\n', '').replace('\r', '')
	if not (ducky_line.startswith(cmd_STRING) or ducky_line.startswith(cmd_STRINGLN) or ducky_line.startswith(cmd_REM)):
		ducky_line = ducky_line.strip()
	if len(ducky_line) == 0:
		return PARSE_OK, "Empty line"
	elif len(ducky_line) >= 250:
		return PARSE_ERROR, "Line too long, max 250 char"
	elif is_ignored_but_valid_command(ducky_line):
		return PARSE_OK, "Success"
	elif ducky_line.startswith(cmd_KEYDOWN + " "):
		parse_result, parse_message = parse_combo(ducky_line[len(cmd_KEYDOWN + " "):])
	elif ducky_line.startswith(cmd_KEYUP + " "):
		parse_result, parse_message = parse_combo(ducky_line[len(cmd_KEYUP + " "):])
	elif ducky_line.startswith(cmd_STRING + " ") or ducky_line.startswith(cmd_STRINGLN + " "):
		return PARSE_OK, "Success"
	elif ducky_line.startswith(cmd_REPEAT + " "):
		try:
			int(ducky_line[len(cmd_REPEAT + " "):].strip())
		except Exception:
			return PARSE_ERROR, "REPEAT invalid argument"
		return PARSE_OK, "Success"
	elif ducky_line.startswith(cmd_DELAY + " "):
		try:
			if int(ducky_line[len(cmd_DELAY + " "):].strip())/1000 <= 0:
				return PARSE_ERROR, "can't be zero or negative"
			else:
				return PARSE_OK, "Success"
		except Exception:
			return PARSE_ERROR, "DELAY invalid argument"
	elif [x for x in ducky_line.split(' ') if len(x) > 0][0] in ds3_keyname_dict.keys():
		parse_result, parse_message = parse_combo(ducky_line)
	elif [x for x in ducky_line.split(' ') if len(x) > 0][0] in mouse_commands:
		parse_result, parse_message = parse_mouse(ducky_line)
	elif ducky_line.startswith(cmd_SWCOLOR):
		return check_color(ducky_line)
	else:
		parse_result = PARSE_ERROR
		parse_message = "Invalid command"
	return parse_result, parse_message


"""
	elif ducky_line.startswith(cmd_LOOP) and ducky_line.endswith(':') and len(ducky_line) == 6 and ducky_line[4].isnumeric():
		return PARSE_OK, "Success"
	elif ducky_line.startswith(cmd_EMUK):
		sssss = ducky_line[len(cmd_EMUK):].strip().split(' ')
		if len(sssss) > 2:
			return PARSE_ERROR, "EMUK max 2 keys"
		if sssss[0] in ds3_keyname_dict.keys() or sssss[0] in mouse_commands[:3]:
			return PARSE_OK, "Success"
		elif len(sssss[0]) == 1:
			return PARSE_OK, "Success"
		return PARSE_ERROR, "EMUK invalid key"
"""


