# https://docs.hak5.org/hak5-usb-rubber-ducky/operators-conditions-loops-and-functions/functions

import ds_syntax_check
from keywords import *

# ---------------------- LINE PARSER ----------------------

PARSE_OK = 0
PARSE_ERROR = 1

def get_default_var_table():
	vt = {}
	for key in reserved_variable_dict:
		vt[key] = 0
	return vt

# name : value
var_table = get_default_var_table()
# name : (start line number, end line number)
func_table = {}
if_take_table = {}
if_skip_table = {}
# line_number_starting_from_1 : end_while line number
while_table = {}
func_search_stack = []
if_search_stack = []
while_search_stack = []
if_raw_info = []
define_dict = {}
break_dict = {}
continue_dict = {}

def reset():
	global var_table
	global define_dict
	var_table = get_default_var_table()
	func_table.clear()
	if_take_table.clear()
	if_skip_table.clear()
	while_table.clear()
	func_search_stack.clear()
	if_search_stack.clear()
	while_search_stack.clear()
	define_dict = {"TRUE":"1", "FALSE":"0"}
	if_raw_info.clear()
	break_dict.clear()
	continue_dict.clear()

def is_valid_var_name(varname):
	if len(varname) == 0:
		return False, 'empty name'
	if varname[0].isnumeric():
		return False, "name can't begin with a number"
	for letter in varname:
		if letter not in valid_var_chars:
			return False, 'name contains invalid characters'
	return True, ''

def contains_english_alphabet(in_str):
	for letter in in_str:
		if letter in english_alphabets:
			return True
	return False

def parse_rvalue(rvalue_str, vt):
	if len(rvalue_str) == 0:
		return False, "empty rvalue", 0
	# print("rvalue_str before replacement:", rvalue_str)
	# search longest name first
	var_list = sorted(list(vt.keys()), key=len, reverse=True)
	for key in var_list:
		if "$"+key in rvalue_str:
			rvalue_str = rvalue_str.replace("$"+key, str(vt[key]+1)).strip() # +1 is to prevent div by zero exception
	rvalue_str = rvalue_str.replace("^", "**")
	# search longest name first
	define_list = sorted(list(define_dict.keys()), key=len, reverse=True)
	for key in define_list:
		if key in rvalue_str:
			rvalue_str = rvalue_str.replace(key, str(define_dict[key])).strip()
	# print("rvalue_str after replacement:", rvalue_str)

	if contains_english_alphabet(rvalue_str):
		return False, "unknown variable or invalid character", 0
	try:
		rvalue_str = rvalue_str.replace("||", " or ").replace("&&", " and ")
		# print("rvalue_str after replacement:", rvalue_str)
		eval_result = eval(rvalue_str)
	except Exception as e:
		return False, f"expr eval fail: {e}", 0
	return True, '', int(eval_result)

def is_valid_expr(whole_line, vt):
	presult = PARSE_ERROR
	pcomment = 'invalid expression'
	try:
		whole_line = whole_line.split(' ', 1)[1]
		is_valid_rv, rv_comment, rv_int = parse_rvalue(whole_line, vt)
		if is_valid_rv:
			presult = PARSE_OK
			return presult, pcomment
	except Exception as e:
		pass
	return presult, pcomment

def assign_var(var_keyword, pgm_line, vt, check_duplicate=False):
	try:
		var_sides = pgm_line.split(var_keyword, 1)[-1].split('=')
		lvalue = var_sides[0].split("$")[-1].rstrip()
		rvalue = var_sides[1].lstrip()
	except Exception as e:
		return PARSE_ERROR, "var declaration parse failed"
	# print([lvalue, rvalue])
	if_valid_vn, vn_comment = is_valid_var_name(lvalue)
	if if_valid_vn is False:
		return PARSE_ERROR, vn_comment
	is_valid_rv, rv_comment, rv_int = parse_rvalue(rvalue, vt)
	if is_valid_rv is False:
		return PARSE_ERROR, rv_comment
	if var_keyword == cmd_VAR_ASSIGN and lvalue not in vt:
		return PARSE_ERROR, "unknown variable"
	if check_duplicate and lvalue in vt:
		return PARSE_ERROR, "variable already declared"
	# if rv_int < 0:
	# 	return PARSE_ERROR, 'var cannot be negative'
	vt[lvalue] = rv_int
	return PARSE_OK, ''

def new_func_check(pgm_line, lnum, fss, fdict):
	if len(fss) != 0:
		return PARSE_ERROR, "missing END_FUNCTION"
	if pgm_line.endswith("()") is False:
		return PARSE_ERROR, "invalid declaration"
	try:
		fun_name = pgm_line.split()[1].split('()')[0]
	except Exception:
		return PARSE_ERROR, "invalid declaration"
	if_valid_vn, vn_comment = is_valid_var_name(fun_name)
	if if_valid_vn is False:
		return PARSE_ERROR, vn_comment
	if fun_name in fdict:
		return PARSE_ERROR, "function already exists"
	fss.append(fun_name)
	fdict[fun_name] = {"fun_start":lnum, 'fun_end':None}
	return PARSE_OK, ''

def func_end_check(pgm_line, lnum, fss, fdict):
	if len(fss) == 0:
		return PARSE_ERROR, "orphan END_FUNCTION"
	fun_name = fss.pop()
	fdict[fun_name]['fun_end'] = lnum
	return PARSE_OK, ''

def if_check(pgm_line, lnum, iss, vt):
	if pgm_line.endswith(cmd_THEN) is False:
		return PARSE_ERROR, "missing THEN at end"
	if_expr = pgm_line.replace(cmd_IF, '', 1)
	if_expr = if_expr[:len(if_expr) - len(cmd_THEN)]
	success, comment, value = parse_rvalue(if_expr, vt)
	if success is False:
		return PARSE_ERROR, comment
	iss.append({lnum:{"else_if":[], "else":None, "end_if":None}})
	return PARSE_OK, ''

def end_if_check(pgm_line, lnum, iss, if_skip_table, if_take_table):
	if pgm_line != cmd_END_IF:
		return PARSE_ERROR, "missing END_IF at end"
	if len(iss) == 0:
		return PARSE_ERROR, "orphan END_IF"
	ifdict = iss.pop()
	if_root = list(ifdict.keys())[0]
	ifdict[if_root]['end_if'] = lnum
	ifdict[if_root]['else_if'] = sorted(ifdict[if_root]['else_if'])
	# print(ifdict)

	if_take_table[if_root] = ifdict[if_root]['end_if']
	# has no else_if and no else
	if len(ifdict[if_root]['else_if']) == 0 and ifdict[if_root]['else'] is None:
		# print("no else_if and no else")
		if_skip_table[if_root] = ifdict[if_root]['end_if']
	# only has else
	elif len(ifdict[if_root]['else_if']) == 0 and ifdict[if_root]['else'] is not None:
		# print("only has else")
		if_skip_table[if_root] = ifdict[if_root]['else']
	# only has else_if
	elif len(ifdict[if_root]['else_if']) > 0 and ifdict[if_root]['else'] is None:
		# print("only has else_if")
		cond_chain = [if_root] + ifdict[if_root]['else_if'] + [ifdict[if_root]['end_if']]
		# print(cond_chain)
		for index, item in enumerate(cond_chain[:-1]):
			if_skip_table[item] = cond_chain[index+1]
			if_take_table[item] = ifdict[if_root]['end_if']

	elif len(ifdict[if_root]['else_if']) > 0 and ifdict[if_root]['else'] is not None:
		# print("has both else and else_if")
		cond_chain = [if_root] + ifdict[if_root]['else_if'] + [ifdict[if_root]['else']] + [ifdict[if_root]['end_if']]
		# print(cond_chain)
		for index, item in enumerate(cond_chain[:-1]):
			if_skip_table[item] = cond_chain[index+1]
			if_take_table[item] = ifdict[if_root]['end_if']
	# print("if_skip_table", if_skip_table)
	# print("if_take_table", if_take_table)
	if_raw_info.append(ifdict)
	return PARSE_OK, ''

def elseif_check(pgm_line, lnum, iss, vt):
	if pgm_line.endswith(cmd_THEN) is False:
		return PARSE_ERROR, "missing THEN"
	if len(iss) == 0:
		return PARSE_ERROR, "orphan ELSE IF"
	ifdict = iss[-1]
	for key in ifdict:
		if ifdict[key]['else'] is not None:
			return PARSE_ERROR, "ELSE IF must be before ELSE"
		ifdict[key]['else_if'].append(lnum)
	# print(ifdict)
	elseif_expr = pgm_line.replace(cmd_ELSE_IF, '', 1)
	elseif_expr = elseif_expr[:len(elseif_expr) - len(cmd_THEN)]
	success, comment, value = parse_rvalue(elseif_expr, vt)
	if success is False:
		return PARSE_ERROR, comment
	return PARSE_OK, ''

def else_check(pgm_line, lnum, iss):
	if pgm_line != cmd_ELSE:
		return PARSE_ERROR, "extra stuff at end"
	if len(iss) == 0:
		return PARSE_ERROR, "orphan ELSE"
	ifdict = iss[-1]
	for key in ifdict:
		if ifdict[key]['else'] != None:
			return PARSE_ERROR, "unmatched ELSE"
		ifdict[key]['else'] = lnum
	# print(ifdict)
	return PARSE_OK, ''

def new_while_check(pgm_line, lnum, wss, wdict, vt):
	while_expr = pgm_line.replace(cmd_WHILE, '', 1)
	success, comment, value = parse_rvalue(while_expr, var_table)
	if success is False:
		return PARSE_ERROR, comment
	wss.append(lnum)
	wdict[lnum] = None
	return PARSE_OK, ''

def end_while_check(pgm_line, lnum, wss, wdict):
	if pgm_line != cmd_END_WHILE:
		return PARSE_ERROR, "extra stuff at end"
	if len(wss) == 0:
		return PARSE_ERROR, "orphan END_WHILE"
	while_start_line = wss.pop()
	wdict[while_start_line] = lnum;
	return PARSE_OK, '' 

def break_check(pgm_line, lnum, wss, bdict):
	split = [x for x in pgm_line.split(' ') if len(x) > 0]
	if len(split) != 1:
		return PARSE_ERROR, "extra stuff at end"
	if len(wss) == 0:
		return PARSE_ERROR, "BREAK outside WHILE"
	bdict[lnum] = wss[-1];
	return PARSE_OK, '' 

def continue_check(pgm_line, lnum, wss, cdict):
	split = [x for x in pgm_line.split(' ') if len(x) > 0]
	if len(split) != 1:
		return PARSE_ERROR, "extra stuff at end"
	if len(wss) == 0:
		return PARSE_ERROR, "CONTINUE outside WHILE"
	cdict[lnum] = wss[-1];
	return PARSE_OK, '' 

whitespace_chars = [' ', '\t']

def skip_whitespace(pgm_line):
	search_index = len(cmd_DEFINE)
	try:
		while 1:
			this_char = pgm_line[search_index]
			if this_char in whitespace_chars:
				search_index += 1
			else:
				return search_index
	except Exception:
		return -1
	return -1

def new_define(pgm_line, dd):
	define_source_start = skip_whitespace(pgm_line)
	if define_source_start == -1:
		return PARSE_ERROR, "DEFINE content not found"
	segments = pgm_line[define_source_start:].split(' ', 1)
	define_source = segments[0]
	define_destination = segments[1]
	is_valid, comment = is_valid_var_name(define_source)
	if is_valid is False:
		return PARSE_ERROR, comment
	if define_source in dd:
		return PARSE_ERROR, f"{define_source} is already defined"
	dd[define_source] = define_destination
	return PARSE_OK, ''

def replace_delay_statements(pgm_line):
	first_word = pgm_line.split()[0]
	if first_word == cmd_DEFAULTDELAY:
		pgm_line = pgm_line.replace(cmd_DEFAULTDELAY, f"$_{cmd_DEFAULTDELAY} =")
		first_word = pgm_line.split()[0]
	elif first_word == cmd_DEFAULTCHARDELAY:
		pgm_line = pgm_line.replace(cmd_DEFAULTCHARDELAY, f"$_{cmd_DEFAULTCHARDELAY} =")
		first_word = pgm_line.split()[0]
	elif first_word == cmd_CHARJITTER:
		pgm_line = pgm_line.replace(cmd_CHARJITTER, f"$_{cmd_CHARJITTER} =")
		first_word = pgm_line.split()[0]
	return first_word, pgm_line

def is_valid_swc_arg(name, vt):
	try:
		return 0 <= int(name) <= 255;
	except:
		pass
	if name[0] != '$':
		return False
	return name[1:] in vt

def check_color(pgm_line, vt):
	split = [x for x in pgm_line.split(' ') if len(x) > 0]
	if len(split) != 5:
		return PARSE_ERROR, "wrong number of arguments"
	for item in split[1:]:
		if is_valid_swc_arg(item, vt) is False:
			return PARSE_ERROR, "invalid color value"
	return PARSE_OK, ''

def check_swcf(pgm_line, vt):
	split = [x for x in pgm_line.split(' ') if len(x) > 0]
	if len(split) != 4:
		return PARSE_ERROR, "wrong number of arguments"
	for item in split[1:]:
		if is_valid_swc_arg(item, vt) is False:
			return PARSE_ERROR, "invalid color value"
	return PARSE_OK, ''

def check_swcolor(pgm_line, first_word):
	with_underscore = cmd_SWCOLOR+'_'
	if pgm_line.startswith(with_underscore):
		new_line = pgm_line.replace(with_underscore, '')
	else:
		new_line = pgm_line.replace(cmd_SWCOLOR, '')
	split = [x for x in new_line.split(' ') if len(x) > 0]
	if first_word == cmd_SWCOLOR and len(split) != 3:
		return PARSE_ERROR, "wrong number of arguments", None
	if pgm_line.startswith(with_underscore) and len(split) != 4:
		return PARSE_ERROR, "wrong number of arguments", None

	arg_list = []
	if first_word == cmd_SWCOLOR:
		arg_list.append("0")
	arg_list += split
	return PARSE_OK, '', arg_list

def check_swcr(pgm_line, vt):
	split = [x for x in pgm_line.split(' ') if len(x) > 0]
	if len(split) != 2:
		return PARSE_ERROR, "wrong number of arguments"
	if is_valid_swc_arg(split[1], vt) is False:
		return PARSE_ERROR, "invalid color value"
	return PARSE_OK, ''

def check_olc(pgm_line, vt):
	split = [x for x in pgm_line.split(' ') if len(x) > 0]
	if len(split) != 3:
		return PARSE_ERROR, "wrong number of arguments"
	for item in split[1:]:
		if is_valid_swc_arg(item, vt) is False:
			return PARSE_ERROR, "invalid cursor value"
	return PARSE_OK, ''

def ensure_zero_arg(pgm_line):
	split = [x for x in pgm_line.split(' ') if len(x) > 0]
	if len(split) != 1:
		return PARSE_ERROR, "wrong number of arguments"
	return PARSE_OK, ''

def needs_rstrip(first_word):
	return not (first_word.startswith(cmd_STRING) or first_word == cmd_OLED_PRINT)

def check_first_arg(pgm_line, vt, allow_multi_arg=False):
	split = [x for x in pgm_line.split(' ') if len(x) > 0]
	if allow_multi_arg is False and len(split) != 2:
		return PARSE_ERROR, "only one argument allowed"
	try:
		if int(split[1]) < 0:
			return PARSE_ERROR, "value can't be negative"
		return PARSE_OK, ""
	except:
		pass
	return is_valid_expr(pgm_line, vt)

def check_loop(pgm_line):
	try:
		line_split = [x for x in pgm_line.split(cmd_LOOP) if len(x) > 0]
		if ':' not in line_split[0]:
			return PARSE_ERROR, 'missing ":"', None
		number_split = [x for x in line_split[0].split(":") if len(x) > 0]
		if len(number_split) != 1:
			return PARSE_ERROR, "wrong number of arguments", None
		return PARSE_OK, "", int(number_split[0])
	except Exception as e:
		return PARSE_ERROR, str(e), None

def run_once(program_listing):
	reset()
	return_dict = {
	'is_success':False,
	'comments':"",
	'error_line_number_starting_from_1':None,
	'error_line_str':"",
	'define_dict':None,
	'var_table':None,
	'func_table':None,
	'if_take_table':None,
	'if_skip_table':None,
	'while_table_bdbc':None,
	'loop_state_save_needed':False,
	'color_state_save_needed':False,
	'oled_restore_needed':False,
	'button_buf_clear_needed':False,
	'loop_size':None,
	}

	loop_numbers = set()

	for line_number_starting_from_1, this_line in enumerate(program_listing):
		line_number_starting_from_1 += 1;
		this_line = this_line.lstrip(' \t')
		if len(this_line) == 0:
			continue

		first_word = this_line.split()[0]
		if needs_rstrip(first_word):
			this_line = this_line.rstrip(" \t")

		if "$_READKEY" in this_line:
			return_dict['button_buf_clear_needed'] = True

		presult = PARSE_ERROR
		pcomment = f"empty comment"

		if first_word != cmd_DEFINE:
			is_success, replaced_str = replace_DEFINE(this_line, define_dict)
			if is_success is False:
				return_dict['is_success'] = False
				return_dict['comments'] = "Recursive DEFINE"
				return_dict['error_line_number_starting_from_1'] = line_number_starting_from_1
				return_dict['error_line_str'] = this_line
				return return_dict
			this_line = replaced_str

		first_word, this_line = replace_delay_statements(this_line)
		if first_word == cmd_DEFINE:
			presult, pcomment = new_define(this_line, define_dict)
		elif first_word == cmd_VAR_DECLARE:
			presult, pcomment = assign_var(cmd_VAR_DECLARE, this_line, var_table, check_duplicate=True)
		elif first_word[0] == "$":
			presult, pcomment = assign_var(cmd_VAR_ASSIGN, this_line, var_table)
		elif first_word == cmd_FUNCTION:
			presult, pcomment = new_func_check(this_line, line_number_starting_from_1, func_search_stack, func_table)
		elif first_word == cmd_END_FUNCTION:
			presult, pcomment = func_end_check(this_line, line_number_starting_from_1, func_search_stack, func_table)
		elif first_word == cmd_IF:
			presult, pcomment = if_check(this_line, line_number_starting_from_1, if_search_stack, var_table)
		elif this_line.startswith(cmd_ELSE_IF):
			presult, pcomment = elseif_check(this_line, line_number_starting_from_1, if_search_stack, var_table)
		elif first_word == cmd_ELSE:
			presult, pcomment = else_check(this_line, line_number_starting_from_1, if_search_stack)
		elif first_word == cmd_END_IF:
			presult, pcomment = end_if_check(this_line, line_number_starting_from_1, if_search_stack, if_skip_table, if_take_table)
		elif first_word == cmd_WHILE:
			presult, pcomment = new_while_check(this_line, line_number_starting_from_1, while_search_stack, while_table, var_table)
		elif first_word == cmd_END_WHILE:
			presult, pcomment = end_while_check(this_line, line_number_starting_from_1, while_search_stack, while_table)
		elif first_word == cmd_BREAK:
			presult, pcomment = break_check(this_line, line_number_starting_from_1, while_search_stack, break_dict)
		elif first_word == cmd_CONTINUE:
			presult, pcomment = continue_check(this_line, line_number_starting_from_1, while_search_stack, continue_dict)
		elif this_line == cmd_HALT:
			presult = PARSE_OK
			pcomment = ''
		elif this_line == cmd_RETURN:
			if len(func_search_stack) == 0:
				presult = PARSE_ERROR
				pcomment = f"RETURN outside function"
			else:
				presult = PARSE_OK
				pcomment = ''
		elif this_line.endswith("()"):
			fun_name = this_line[0:len(this_line)-2]
			if fun_name in func_table:
				presult = PARSE_OK
			else:
				pcomment = f"Unknown function: {fun_name}"
		elif first_word == cmd_DELAY:
			presult, pcomment = check_first_arg(this_line, var_table)
		elif first_word == cmd_GOTO_PROFILE:
			presult, pcomment = check_first_arg(this_line, var_table, allow_multi_arg=True)
		elif first_word == cmd_SWCC:
			return_dict['color_state_save_needed'] = True
			presult, pcomment = check_color(this_line, var_table)
		elif first_word == cmd_SWCF:
			presult, pcomment = check_swcf(this_line, var_table)
		elif first_word == cmd_SWCR:
			presult, pcomment = check_swcr(this_line, var_table)
		elif first_word == cmd_OLED_CURSOR:
			presult, pcomment = check_olc(this_line, var_table)
		elif first_word == cmd_OLED_UPDATE:
			return_dict['oled_restore_needed'] = True
			presult, pcomment = ensure_zero_arg(this_line)
		elif first_word == cmd_OLED_BLANK:
			presult, pcomment = ensure_zero_arg(this_line)
		elif first_word == cmd_OLED_RESTORE:
			presult, pcomment = ensure_zero_arg(this_line)
		elif first_word == cmd_BCLR:
			presult, pcomment = ensure_zero_arg(this_line)
		elif first_word == cmd_NEXT_PROFILE:
			presult, pcomment = ensure_zero_arg(this_line)
		elif first_word == cmd_PREV_PROFILE:
			presult, pcomment = ensure_zero_arg(this_line)
		elif first_word == cmd_DP_SLEEP:
			presult, pcomment = ensure_zero_arg(this_line)
		elif this_line.startswith(cmd_SWCOLOR):
			presult, pcomment, arg_list = check_swcolor(this_line, first_word)
			return_dict['color_state_save_needed'] = True
		elif this_line.startswith(cmd_LOOP):
			presult, pcomment, value = check_loop(this_line)
			if value is not None:
				return_dict['loop_state_save_needed'] = True
				loop_numbers.add(value)
		else:
			presult, pcomment = ds_syntax_check.parse_line(this_line)
		
		if presult == PARSE_ERROR:
			# error_message = f"PARSE ERROR at Line {line_number_starting_from_1}: {this_line}\n{pcomment}"
			return_dict['is_success'] = False
			return_dict['comments'] = pcomment
			return_dict['error_line_number_starting_from_1'] = line_number_starting_from_1
			return_dict['error_line_str'] = this_line
			return return_dict

	# print("\n----------- EOF REACHED -----------\n")

	if len(if_search_stack) != 0:
		return_dict['is_success'] = False
		# return_dict['comments'] = f"END_IF missing for IF at line {list(if_search_stack[0].keys())[0]}"
		return_dict['comments'] = "Missing END_IF"
		return_dict['error_line_number_starting_from_1'] = list(if_search_stack[0].keys())[0]
		return return_dict

	if len(func_search_stack) != 0:
		return_dict['is_success'] = False
		# return_dict['comments'] = f"END_FUNCTION missing for FUNCTION {func_search_stack[0]}() at line {func_table[func_search_stack[0]]['fun_start']}"
		return_dict['comments'] = "Missing END_FUNCTION"
		return_dict['error_line_number_starting_from_1'] = func_table[func_search_stack[0]]['fun_start']
		return return_dict

	if len(while_search_stack) != 0:
		return_dict['is_success'] = False
		# return_dict['comments'] = f"END_WHILE missing for WHILE at line {while_search_stack[-1]}"
		return_dict['comments'] = "Missing END_WHILE"
		return_dict['error_line_number_starting_from_1'] = while_search_stack[-1]
		return return_dict

	while_table_bidirectional = {**while_table, **dict((v,k) for k,v in while_table.items())}

	for key in break_dict:
		while_start = break_dict[key]
		end_while = while_table_bidirectional[while_start]
		break_dict[key] = end_while

	return_dict['is_success'] = True
	return_dict['comments'] = ""
	return_dict['error_line_number_starting_from_1'] = None
	return_dict['error_line_str'] = ""
	return_dict['define_dict'] = define_dict
	return_dict['var_table'] = var_table
	return_dict['func_table'] = func_table
	return_dict['if_take_table'] = if_take_table
	return_dict['if_skip_table'] = if_skip_table
	return_dict['while_table_bidirectional'] = while_table_bidirectional
	return_dict['break_dict'] = break_dict
	return_dict['continue_dict'] = continue_dict
	if len(loop_numbers) > 0:
		return_dict['loop_size'] = max(loop_numbers)
	return return_dict

def run_all(program_listing):
	rdict = run_once(program_listing)
	if rdict['is_success'] is False:
		return rdict

	print("\n---------First pass OK!---------\n")

	# ----------- making condensed version ----------

	def_dict = rdict['define_dict']
	second_pass_program_listing = []
	needs_end_if = False

	epilogue = 0
	if rdict['loop_state_save_needed']:
		epilogue |= 0x1
	if rdict['color_state_save_needed']:
		epilogue |= 0x2
	if rdict['oled_restore_needed']:
		epilogue |= 0x4

	if epilogue != 0:
		second_pass_program_listing.append((1, f"$_NEEDS_EPILOGUE = {epilogue}"))
	if rdict['button_buf_clear_needed']:
		second_pass_program_listing.append((1, cmd_BCLR))
	if rdict['loop_size'] is not None:
		second_pass_program_listing.append((1, f"$_LOOP_SIZE = {rdict['loop_size']+1}"))

	for line_number_starting_from_1, this_line in enumerate(program_listing):
		line_number_starting_from_1 += 1;
		this_line = this_line.lstrip(' \t')
		if len(this_line) == 0:
			continue
		first_word = this_line.split(" ")[0]
		first_word, this_line = replace_delay_statements(this_line)
		if needs_rstrip(first_word):
			this_line = this_line.rstrip(" \t")
		if first_word == cmd_REM or first_word == cmd_INJECT_MOD or this_line.startswith(cmd_C_COMMENT):
			continue
		if first_word != cmd_DEFINE:
			is_success, replaced_str = replace_DEFINE(this_line, def_dict)
			if is_success is False:
				rdict['is_success'] = False
				rdict['comments'] = "Recursive DEFINE"
				rdict['error_line_number_starting_from_1'] = line_number_starting_from_1
				rdict['error_line_str'] = this_line
				return rdict
			else:
				this_line = replaced_str
		else:
			continue

		if first_word == cmd_REPEAT:
			if len(second_pass_program_listing) == 0:
				rdict['is_success'] = False
				rdict['comments'] = "Nothing before REPEAT"
				rdict['error_line_number_starting_from_1'] = line_number_starting_from_1
				rdict['error_line_str'] = this_line
				return rdict
			last_line = second_pass_program_listing[-1]
			for x in range(int(this_line[len(cmd_REPEAT):].strip())):
				second_pass_program_listing.append(last_line)
		elif this_line.startswith(cmd_SWCOLOR):
			presult, pcomment, arg_list = check_swcolor(this_line, first_word)
			this_str = f"{cmd_SWCC} "
			for item in arg_list:
				this_str += f"{item} "
			second_pass_program_listing.append((line_number_starting_from_1, this_str))
		elif this_line.startswith(cmd_LOOP):
			presult, pcomment, value = check_loop(this_line)
			if needs_end_if:
				second_pass_program_listing.append((line_number_starting_from_1, cmd_END_IF))
			loop_str = f'{cmd_IF} $_KEYPRESS_COUNT % $_LOOP_SIZE == {value} {cmd_THEN}'
			second_pass_program_listing.append((line_number_starting_from_1, loop_str))
			needs_end_if = True
		else:
			second_pass_program_listing.append((line_number_starting_from_1, this_line))

	if needs_end_if:
		second_pass_program_listing.append((line_number_starting_from_1, cmd_END_IF))

	if rdict['button_buf_clear_needed']:
		second_pass_program_listing.append((line_number_starting_from_1, cmd_BCLR))

	print("---------Second pass OK!---------\n")

	# for item in second_pass_program_listing:
	# 	print(item)
	# exit()

	final_dict = run_once([x[1] for x in second_pass_program_listing])
	final_dict["compact_listing"] = second_pass_program_listing

	if_info_list = []
	for item in if_raw_info:	
		if_root = list(item.keys())[0]
		if_info = {
		'root':if_root,
		"else_if":item[if_root]['else_if'],
		"else":item[if_root]['else'],
		"end_if":item[if_root]['end_if'],
		}
		if_info_list.append(if_info)
	final_dict["if_info"] = if_info_list
	print("---------Preprocessing Done!---------\n")
	return final_dict