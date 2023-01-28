import sys
import ds3_preprocessor
import ast
import myast
from keywords import *

# CPU instructions
OP_NOP = ("NOP", 0)
OP_PUSHC = ("PUSHC", 1)
OP_PUSHV = ("PUSHV", 2)
OP_POP = ("POP", 3)
OP_BRZ = ("BRZ", 4)
OP_JMP = ("JMP", 5)
OP_CALL = ("CALL", 6)
OP_RET = ("RET", 7)
OP_HALT = ("HALT", 8)

# Comparisons
OP_EQ = ("EQ", 9)
OP_NOTEQ = ("NOTEQ", 10)
# left value on bottom, right value on top
OP_LT = ("LT", 11)
OP_LTE = ("LTE", 12)
OP_GT = ("GT", 13)
OP_GTE = ("GTE", 14)

# Arithmetics
OP_ADD = ("ADD", 15)
# left value on bottom, right value on top
OP_SUB = ("SUB", 16)
OP_MULT = ("MULT", 17)
OP_DIV = ("DIV", 18)
OP_MOD = ("MOD", 19)
OP_POW = ("POW", 20)

# Bitwise operation
OP_LSHIFT = ("LSHIFT", 21)
OP_RSHIFT = ("RSHIFT", 22)
OP_BITOR = ("BITOR", 23)
OP_BITAND = ("BITAND", 24)

# Logical operation
OP_LOGIAND = ("LOGIAND", 25)
OP_LOGIOR = ("LOGIOR", 26)

OP_DELAY = ("DELAY", 27)
OP_KUPD = ("KUPD", 29)
OP_KDOWN = ("KDOWN", 30)
OP_KDOWND = ("KDOWND", 31)
OP_MSCL = ("MSCL", 33)
OP_MMOV = ("MMOV", 34)
OP_SWCB = ("SWCB", 32)
OP_SWCC = ("SWCC", 35)
OP_SWCR = ("SWCR", 36)
OP_STR = ("STR", 38)
OP_STRLN = ("STRLN", 39)
OP_EMUK = ("EMUK", 40)
OP_LOOP = ("LOOP", 41)
OP_OLC = ("OLC", 42)
OP_OLP = ("OLP", 43)
OP_OLU = ("OLU", 44)
OP_OLB = ("OLB", 45)
OP_OLR = ("OLR", 46)
OP_BCLR = ("BCLR", 47)
OP_PREVP = ("PREVP", 48)
OP_NEXTP = ("NEXTP", 49)
OP_GOTOP = ("GOTOP", 50)
OP_SLEEP = ("SLEEP", 51)

arith_lookup = {
	"Eq" : OP_EQ,
	"NotEq" : OP_NOTEQ,
	"Lt" : OP_LT,
	"LtE" : OP_LTE,
	"Gt" : OP_GT,
	"GtE" : OP_GTE,

	"Add" : OP_ADD,
	"Sub" : OP_SUB,
	"Mult" : OP_MULT,
	"Div" : OP_DIV,
	"Mod" : OP_MOD,
	"Pow" : OP_POW,

	"LShift" : OP_LSHIFT,
	"RShift" : OP_RSHIFT,
	"BitOr" : OP_BITOR,
	"BitAnd" : OP_BITAND,

	"And" : OP_LOGIAND,
	"Or" : OP_LOGIOR,
}

zero = 0
endianness = 'little'
var_boundary = 0x1f
INSTRUCTION_SIZE_BYTES = 3

if_skip_table = None
if_info_list = None
while_lookup = None
var_lookup = None
compact_program_listing = None
label_dict = None
func_lookup = None
str_lookup = None

def get_empty_instruction():
	return {
	'opcode':OP_NOP,
	'oparg':None,
	'label':None,
	'comment':None,
	'addr':None}

def print_instruction(instruction):
	if instruction['label'] is not None:
		print(f"----{instruction['label']}:")

	if instruction['addr'] is not None:
		print(str(instruction['addr']).ljust(5), end='')
	print(instruction['opcode'][0].ljust(10), end='')
	tempstr = ""
	if instruction['oparg'] is not None:
		tempstr = f"{instruction['oparg']}  "
		if isinstance(instruction['oparg'], int):
			tempstr += f"{hex(instruction['oparg'])}"
	print(tempstr.ljust(20), end='')
	tempstr = ""
	if instruction['comment'] is not None:
		tempstr = ";" + str(instruction['comment'])
	print(tempstr)

def print_asslist(lll):
	print()
	for item in lll:
		print_instruction(item)
	print()

def visit_node(node, instruction_list):
	# print(node.__dict__)
	# a node can be Name, Constant, and operations such as ADD, SUB, COMPARE, etc
	if isinstance(node, ast.Name):
		this_instruction = get_empty_instruction()
		this_instruction['opcode'] = OP_PUSHV
		this_instruction['oparg'] = str(node.id)
		instruction_list.append(this_instruction)
	elif isinstance(node, ast.Constant):
		this_instruction = get_empty_instruction()
		this_instruction['opcode'] = OP_PUSHC
		this_instruction['oparg'] = int(node.value)
		instruction_list.append(this_instruction)
	elif isinstance(node, ast.Compare):
		op_name = node.ops[0].__class__.__name__
		if op_name not in arith_lookup:
			raise ValueError("unknown AST operation")
		this_instruction = get_empty_instruction()
		this_instruction['opcode'] = arith_lookup[op_name]
		instruction_list.append(this_instruction)
	elif isinstance(node, ast.BoolOp):
		op_name = node.op.__class__.__name__
		if op_name not in arith_lookup:
			raise ValueError("unknown AST operation")
		this_instruction = get_empty_instruction()
		this_instruction['opcode'] = arith_lookup[op_name]
		# multiple boolean comparisons
		for x in range(len(node.values) - 1):
			instruction_list.append(this_instruction)
	elif isinstance(node, ast.BinOp):
		op_name = node.op.__class__.__name__
		if op_name not in arith_lookup:
			raise ValueError("unknown AST operation")
		this_instruction = get_empty_instruction()
		this_instruction['opcode'] = arith_lookup[op_name]
		instruction_list.append(this_instruction)

"""
this generates a list of instructions to calculate the result of an expression
once executed, the result should be the only item left in the stack
don't forget to pop it out to somewhere to do whatever is needed
"""
def evaluate_expr(expr):
	instruction_list = []
	root = ast.parse(expr).body[0].value
	if myast.is_walkable(root):
		myast.postorder_walk(root, visit_node, instruction_list)
	elif isinstance(root, ast.Constant):
		this_instruction = get_empty_instruction()
		this_instruction['opcode'] = OP_PUSHC
		this_instruction['oparg'] = root.value
		instruction_list.append(this_instruction)
	elif isinstance(root, ast.Name):
		this_instruction = get_empty_instruction()
		this_instruction['opcode'] = OP_PUSHV
		this_instruction['oparg'] = str(root.id)
		instruction_list.append(this_instruction)
	else:
		raise ValueError(f"unknown ast node: {root}")
	return instruction_list

def replace_operators(this_line):
	return this_line.replace("$", "").replace("||", " or ").replace("&&", " and ").replace("^", "**")

def assign_var(var_keyword, pgm_line):
	var_sides = pgm_line.split(var_keyword, 1)[-1].split('=', 1)
	lvalue = var_sides[0].split("$")[-1].rstrip()
	rvalue = var_sides[1].lstrip()
	rvalue = replace_operators(rvalue)
	ins_list = evaluate_expr(rvalue)
	this_instruction = get_empty_instruction()
	this_instruction['opcode'] = OP_POP
	this_instruction['oparg'] = lvalue
	ins_list.append(this_instruction)
	for item in ins_list:
		item['comment'] = pgm_line
	return ins_list

def write_var(var_name, var_value):
	ins_list = []
	this_instruction = get_empty_instruction()
	this_instruction['opcode'] = OP_PUSHC
	this_instruction['oparg'] = var_value
	ins_list.append(this_instruction)
	this_instruction = get_empty_instruction()
	this_instruction['opcode'] = OP_POP
	this_instruction['oparg'] = var_name
	ins_list.append(this_instruction)
	return ins_list

def get_if_label(lnum, if_info):
	for item in if_info:
		if item['else'] == lnum:
			return "ELSE"
		if item['end_if'] == lnum:
			return "ENDIF"
		if lnum in item['else_if']:
			return "ELSEIF"
	raise ValueError("get_if_label")

def parse_if(pgm_line, lnum):
	expression = pgm_line.split("IF", 1)[-1].split("THEN")[0].strip()
	expression = replace_operators(expression)
	assembly_listing = evaluate_expr(expression)
	# expression result is now the only item on arithmetic stack
	this_instruction = get_empty_instruction()
	this_instruction['opcode'] = OP_BRZ
	label_name = f"{get_if_label(if_skip_table[lnum], if_info_list)}@{if_skip_table[lnum]}"
	this_instruction['oparg'] = label_name
	label_dict[if_skip_table[lnum]] = label_name
	assembly_listing.append(this_instruction)
	for item in assembly_listing:
		item['comment'] = pgm_line
	return assembly_listing

def parse_while(pgm_line, lnum):
	expression = pgm_line.replace(cmd_WHILE, '', 1).strip()
	expression = replace_operators(expression)
	assembly_listing = evaluate_expr(expression)
	label_name = f'WHILE@{lnum}'
	assembly_listing[0]['label'] = label_name
	label_dict[lnum] = label_name
	this_instruction = get_empty_instruction()
	this_instruction['opcode'] = OP_BRZ
	label_name = f'ENDWHILE@{while_lookup[lnum]}'
	this_instruction['oparg'] = label_name
	label_dict[while_lookup[lnum]] = label_name
	assembly_listing.append(this_instruction)
	for item in assembly_listing:
		item['comment'] = pgm_line
	return assembly_listing

def find_endif(lnum, if_info):
	for item in if_info:
		if item['else'] == lnum:
			return item['end_if']
		for yyy in item['else_if']:
			if yyy == lnum:
				return item['end_if']
	raise ValueError("find_endif")

def add_jmp(lnum, if_info):
	this_instruction = get_empty_instruction()
	this_instruction['opcode'] = OP_JMP
	endif_location = find_endif(lnum, if_info)
	label_name = f"ENDIF@{endif_location}"
	this_instruction['oparg'] = label_name
	label_dict[endif_location] = label_name
	return this_instruction

def get_int_args(pgm_line):
	return [int(x) for x in pgm_line.split(" ")[1:] if len(x) > 0]

def make_delay_instruction(comment):
	this_instruction = get_empty_instruction()
	this_instruction['opcode'] = OP_DELAY
	this_instruction['comment'] = comment
	return this_instruction

def parse_expression(pgm_line):
	expression = pgm_line.split(' ', 1)[1].strip()
	expression = replace_operators(expression)
	ins_list = evaluate_expr(expression)
	for item in ins_list:
		item['comment'] = pgm_line
	return ins_list

def get_key_combined_value(keyname):
	if keyname in ds3_keyname_dict:
		key_code = ds3_keyname_dict[keyname][0]
		key_type = ds3_keyname_dict[keyname][1]
	else:
		key_code = ord(keyname[0])
		key_type = KEY_TYPE_CHAR
	return ((key_type % 0xff) << 8) | (key_code % 0xff)

def get_mmov_combined_value(pgm_line):
	split = [x for x in pgm_line.split(' ') if len(x) > 0]
	x_value = int(split[1])
	y_value = int(split[2])
	if x_value < 0:
		x_value += 256
	if y_value < 0:
		y_value += 256
	return ((x_value % 0xff) << 8) | (y_value % 0xff)

def get_mouse_wheel_value(pgm_line):
	split = [x for x in pgm_line.split(' ') if len(x) > 0]
	amount = int(split[-1])
	if amount < 0:
		amount += 256
	return amount

def get_partial_varname_addr(msg, vad):
	if len(msg) == 0:
		return None, None
	for x in range(len(msg)+1):
		partial_name = msg[:x]
		if partial_name in vad:
			return partial_name, vad[partial_name]
	return None, None

def replace_var_in_str(msg, vad):
	bytearr = bytearray()
	curr = 0
	while curr < len(msg):
		this_letter = msg[curr]
		if this_letter == "$":
			var_name, var_addr = get_partial_varname_addr(msg[curr+1:], vad)
			if var_name is not None:
				curr += len(var_name)
				bytearr += var_boundary.to_bytes(1, endianness)
				bytearr += var_addr.to_bytes(2, endianness)
				bytearr += var_boundary.to_bytes(1, endianness)
			else:
				bytearr += this_letter.encode()
		else:
			bytearr += this_letter.encode()
		curr += 1
	return bytearr

def dict_pp(dic):
	result = ''
	for key in dic:
		if dic[key] is None:
			continue
		result += f"{key}: {dic[key]}\n"
	return result

def get_swc_arg(name):
	try:
		return int(name)
	except:
		pass
	return name[1:]

def parse_color(pgm_line, cmd):
	ins_list = []
	split = [x for x in pgm_line.split(' ') if len(x) > 0]
	for item in split[1:]:
		value = get_swc_arg(item)
		this_instruction = get_empty_instruction()
		if isinstance(value, int):
			this_instruction['opcode'] = OP_PUSHC
		else:
			this_instruction['opcode'] = OP_PUSHV
		this_instruction['oparg'] = value
		this_instruction['comment'] = pgm_line
		ins_list.append(this_instruction)

	this_instruction = get_empty_instruction()
	if cmd == OP_SWCC[0]:
		this_instruction['opcode'] = OP_SWCC
	else:
		this_instruction['opcode'] = OP_SWCB
	this_instruction['comment'] = pgm_line
	ins_list.append(this_instruction)
	return ins_list

def parse_swcr(pgm_line, vt):
	ins_list = []
	split = [x for x in pgm_line.split(' ') if len(x) > 0]
	value = get_swc_arg(split[1])
	this_instruction = get_empty_instruction()
	if isinstance(value, int):
		this_instruction['opcode'] = OP_PUSHC
	else:
		this_instruction['opcode'] = OP_PUSHV
	this_instruction['oparg'] = value
	this_instruction['comment'] = pgm_line
	ins_list.append(this_instruction)

	this_instruction = get_empty_instruction()
	this_instruction['opcode'] = OP_SWCR
	this_instruction['comment'] = pgm_line
	ins_list.append(this_instruction)
	return ins_list

def parse_olc(pgm_line, vt):
	ins_list = []
	split = [x for x in pgm_line.split(' ') if len(x) > 0]
	
	for item in split[1:]:
		value = get_swc_arg(item)
		this_instruction = get_empty_instruction()
		if isinstance(value, int):
			this_instruction['opcode'] = OP_PUSHC
		else:
			this_instruction['opcode'] = OP_PUSHV
		this_instruction['oparg'] = value
		this_instruction['comment'] = pgm_line
		ins_list.append(this_instruction)
	this_instruction = get_empty_instruction()
	this_instruction['opcode'] = OP_OLC
	this_instruction['comment'] = pgm_line
	ins_list.append(this_instruction)
	return ins_list

def make_dsb(program_listing):
	global if_skip_table
	global if_info_list
	global while_lookup
	global var_lookup
	global compact_program_listing
	global label_dict
	global func_lookup
	global str_lookup
	# result_dict should at least contain is_success and comments
	result_dict = ds3_preprocessor.run_all(program_listing)
	if result_dict["is_success"] is False:
		raise ValueError(f"code contains errors:\n{dict_pp(result_dict)}")

	if_skip_table = result_dict['if_skip_table']
	if_info_list = result_dict["if_info"]
	while_lookup = result_dict['while_table_bidirectional']
	var_lookup = result_dict['var_table']
	compact_program_listing = [x[1] for x in result_dict['compact_listing']]
	label_dict = {}
	func_lookup = result_dict['func_table']
	str_lookup = {}
	break_dict = result_dict['break_dict']
	continue_dict = result_dict['continue_dict']

	assembly_listing = []

	for lnum, this_line in enumerate(compact_program_listing):
		lnum += 1
		this_instruction = get_empty_instruction()
		this_instruction['comment'] = this_line
		first_word = this_line.split()[0]

		if first_word == cmd_VAR_DECLARE:
			assembly_listing += assign_var(cmd_VAR_DECLARE, this_line)
		elif first_word[0] == "$":
			assembly_listing += assign_var(cmd_VAR_ASSIGN, this_line)
		elif first_word == cmd_IF:
			assembly_listing += parse_if(this_line, lnum)
		elif this_line.startswith(cmd_ELSE_IF):
			assembly_listing.append(add_jmp(lnum, if_info_list))
			temp_listing = parse_if(this_line, lnum)
			if lnum in label_dict:
				temp_listing[0]['label'] = label_dict[lnum]
			assembly_listing += temp_listing
		elif first_word == cmd_ELSE:
			assembly_listing.append(add_jmp(lnum, if_info_list))
			if lnum in label_dict:
				this_instruction['label'] = label_dict[lnum]
			assembly_listing.append(this_instruction)
		elif first_word == cmd_END_IF:
			if lnum in label_dict:
				this_instruction['label'] = label_dict[lnum]
			assembly_listing.append(this_instruction)
		elif first_word == cmd_WHILE:
			assembly_listing += parse_while(this_line, lnum)
		elif first_word == cmd_END_WHILE:
			this_instruction['opcode'] = OP_JMP
			this_instruction['oparg'] = label_dict[while_lookup[lnum]]
			this_instruction['comment'] = None
			assembly_listing.append(this_instruction)
			this_instruction = get_empty_instruction()
			this_instruction['comment'] = this_line
			this_instruction['label'] = label_dict[lnum]
			assembly_listing.append(this_instruction)
		elif first_word == cmd_FUNCTION:
			fun_name = this_line.split()[1].split('()')[0]
			this_instruction['opcode'] = OP_JMP
			fun_end_lnum = func_lookup[fun_name]['fun_end']
			fun_end_label = f"FEND_{fun_name}@{fun_end_lnum}"
			label_dict[fun_end_lnum] = fun_end_label
			this_instruction['oparg'] = fun_end_label
			this_instruction['comment'] = None
			assembly_listing.append(this_instruction)
			this_instruction = get_empty_instruction()
			this_instruction['comment'] = this_line
			fun_start_label = f"FUN_{fun_name}@{lnum}"
			this_instruction['label'] = fun_start_label
			label_dict[lnum] = fun_start_label
			assembly_listing.append(this_instruction)
		elif first_word == cmd_END_FUNCTION:
			# RET, then NOP
			this_instruction['opcode'] = OP_RET
			this_instruction['comment'] = None
			assembly_listing.append(this_instruction)
			this_instruction = get_empty_instruction()
			this_instruction['comment'] = this_line
			this_instruction['label'] = label_dict[lnum]
			assembly_listing.append(this_instruction)
		elif first_word.endswith('()'):
			fun_name = this_line.split('()')[0]
			this_instruction['opcode'] = OP_CALL
			this_instruction['oparg'] = label_dict[func_lookup[fun_name]['fun_start']]
			assembly_listing.append(this_instruction)
		elif this_line.startswith(cmd_STRING) or first_word == cmd_OLED_PRINT:
			str_content = this_line.split(' ', 1)[-1]
			if str_content not in str_lookup:
				str_lookup[str_content] = lnum
			if first_word == cmd_STRING:
				this_instruction['opcode'] = OP_STR
			elif first_word == cmd_STRINGLN:
				this_instruction['opcode'] = OP_STRLN
			elif first_word == cmd_OLED_PRINT:
				this_instruction['opcode'] = OP_OLP
			this_instruction['oparg'] = f"STR@{str_lookup[str_content]}"
			assembly_listing.append(this_instruction)
		elif first_word == cmd_DELAY:
			assembly_listing += parse_expression(this_line)
			assembly_listing.append(make_delay_instruction(this_line))
		elif first_word == cmd_KEYDOWN:
			this_instruction['opcode'] = OP_KDOWND
			this_instruction['oparg'] = get_key_combined_value(this_line.split(' ')[-1])
			assembly_listing.append(this_instruction)
		elif first_word == cmd_EMUK:
			this_instruction['opcode'] = OP_KDOWN
			this_instruction['oparg'] = get_key_combined_value(this_line.split(' ')[-1])
			assembly_listing.append(this_instruction)
		elif first_word == cmd_KEYUP:
			this_instruction['opcode'] = OP_KUPD
			this_instruction['oparg'] = get_key_combined_value(this_line.split(' ')[-1])
			assembly_listing.append(this_instruction)
		elif first_word == cmd_RETURN:
			this_instruction['opcode'] = OP_RET
			assembly_listing.append(this_instruction)
		elif first_word == cmd_HALT:
			this_instruction['opcode'] = OP_HALT
			assembly_listing.append(this_instruction)
		elif first_word == cmd_MOUSE_MOVE:
			this_instruction['opcode'] = OP_MMOV
			this_instruction['oparg'] = get_mmov_combined_value(this_line)
			assembly_listing.append(this_instruction)
		elif first_word == cmd_MOUSE_WHEEL:
			this_instruction['opcode'] = OP_MSCL
			this_instruction['oparg'] = get_mouse_wheel_value(this_line)
			assembly_listing.append(this_instruction)
		elif first_word == cmd_SWCC or first_word == cmd_SWCB:
			assembly_listing += parse_color(this_line, first_word)
		elif first_word == cmd_SWCR:
			assembly_listing += parse_swcr(this_line, first_word)
		elif first_word == cmd_OLED_CURSOR:
			assembly_listing += parse_olc(this_line, first_word)
		elif first_word == cmd_OLED_UPDATE:
			this_instruction['opcode'] = OP_OLU
			assembly_listing.append(this_instruction)
		elif first_word == cmd_OLED_BLANK:
			this_instruction['opcode'] = OP_OLB
			assembly_listing.append(this_instruction)
		elif first_word == cmd_OLED_RESTORE:
			this_instruction['opcode'] = OP_OLR
			assembly_listing.append(this_instruction)
		elif first_word == cmd_BCLR:
			this_instruction['opcode'] = OP_BCLR
			assembly_listing.append(this_instruction)
		elif first_word == cmd_BREAK:
			this_instruction['opcode'] = OP_JMP
			this_instruction['oparg'] = label_dict[break_dict[lnum]]
			assembly_listing.append(this_instruction)
		elif first_word == cmd_CONTINUE:
			this_instruction['opcode'] = OP_JMP
			this_instruction['oparg'] = label_dict[continue_dict[lnum]]
			assembly_listing.append(this_instruction)
		elif first_word == cmd_NEXT_PROFILE:
			this_instruction['opcode'] = OP_NEXTP
			assembly_listing.append(this_instruction)
		elif first_word == cmd_PREV_PROFILE:
			this_instruction['opcode'] = OP_PREVP
			assembly_listing.append(this_instruction)
		elif first_word == cmd_GOTO_PROFILE:
			assembly_listing += parse_expression(this_line)
			this_instruction['opcode'] = OP_GOTOP
			assembly_listing.append(this_instruction)
		elif first_word == cmd_DP_SLEEP:
			this_instruction['opcode'] = OP_SLEEP
			assembly_listing.append(this_instruction)
		elif first_word in ds3_keyname_dict: # key combos
			key_list = [x for x in this_line.split(" ") if len(x) > 0]
			# press, from first to last
			for item in key_list:
				this_instruction = get_empty_instruction()
				this_instruction['opcode'] = OP_KDOWND
				this_instruction['oparg'] = get_key_combined_value(item)
				this_instruction['comment'] = this_line
				assembly_listing.append(this_instruction)
			# release, from last to first
			for item in reversed(key_list):
				this_instruction = get_empty_instruction()
				this_instruction['opcode'] = OP_KUPD
				this_instruction['oparg'] = get_key_combined_value(item)
				this_instruction['comment'] = this_line
				assembly_listing.append(this_instruction)
		else:
			raise ValueError(f"Unknown command: {this_line}")

	this_instruction = get_empty_instruction()
	this_instruction['opcode'] = OP_HALT
	assembly_listing.append(this_instruction)

	print_asslist(assembly_listing)

	print('assembly line count:', len(assembly_listing))
	print("label_dict:", label_dict)
	print("func_lookup:", func_lookup)
	print("str_lookup:", str_lookup)
	print("var_lookup:", var_lookup)

	# ------------------ generate binary ------------------

	for index, item in enumerate(assembly_listing):
		item['addr'] = index * INSTRUCTION_SIZE_BYTES

	var_bin_start = len(assembly_listing) * INSTRUCTION_SIZE_BYTES

	VAR_SIZE_BYTES = 2
	var_addr_dict = {}
	var_count = 0
	for item in var_lookup:
		if item in reserved_variable_dict:
			var_addr_dict[item] = reserved_variable_dict[item]
		else:
			var_addr_dict[item] = var_bin_start + var_count * VAR_SIZE_BYTES
			var_count += 1

	for item in assembly_listing:
		if item['oparg'] in var_addr_dict:
			item['oparg'] = var_addr_dict[item['oparg']]

	str_list = []
	for item in str_lookup:
		this_str = {
		'content': item,
		'bytes': replace_var_in_str(item, var_addr_dict) + zero.to_bytes(1, endianness),
		'lnum': str_lookup[item],
		'addr': None}
		str_list.append(this_str)

	for item in reserved_variable_dict:
		var_lookup.pop(item, None)

	str_bin_start = var_bin_start + len(var_lookup) * VAR_SIZE_BYTES

	for index, item in enumerate(str_list):
		if index == 0:
			item['addr'] = str_bin_start
		else:
			item['addr'] = str_list[index-1]['addr'] + len(str_list[index-1]['bytes'])

	# replace lables with real memory address
	label_to_addr_dict = {}
	for item in assembly_listing:
		if item['label'] is not None:
			label_to_addr_dict[item['label']] = item['addr']

	for item in assembly_listing:
		if item['opcode'] == OP_STR or item['opcode'] == OP_STRLN or item['opcode'] == OP_OLP:
			str_lnum = int(item['oparg'].replace('STR@', ''))
			for sssss in str_list:
				if sssss['lnum'] == str_lnum:
					item['oparg'] = sssss['addr']
		if item['oparg'] is None:
			continue
		if isinstance(item['oparg'], str) and "@" in item['oparg']:
			item['oparg'] = label_to_addr_dict[item['oparg']]
		item['oparg'] = int(item['oparg'])

	print_asslist(assembly_listing)

	output_bin_array = bytearray()

	for item in assembly_listing:
		output_bin_array += item['opcode'][1].to_bytes(1, endianness)
		this_arg = 0
		if item['oparg'] is not None:
			this_arg = item['oparg']
		output_bin_array += this_arg.to_bytes(2, endianness)

	# add space for variables
	output_bin_array += bytearray(len(var_lookup) * VAR_SIZE_BYTES)

	# write zero-terminated strings
	for item in str_list:
		output_bin_array += item['bytes']

	dsb_header_size = 8
	pgm_start = dsb_header_size
	dsb_header = bytearray()
	dsb_header += pgm_start.to_bytes(2, endianness)
	dsb_header += var_bin_start.to_bytes(2, endianness)
	dsb_header += str_bin_start.to_bytes(2, endianness)
	dsb_header += bytearray(2)
	output_bin_array = dsb_header + output_bin_array

	print("label_to_addr_dict:", label_to_addr_dict)	
	print("var_addr_dict:", var_addr_dict)
	print('var_lookup:', var_lookup)
	print("var_bin_start:", var_bin_start)
	print("str_bin_start:", str_bin_start)
	print("str_list:", str_list)

	return output_bin_array

if __name__ == "__main__":

	if len(sys.argv) <= 1:
		print(__file__, "script_file")
		exit()

	text_file = open(sys.argv[1])
	program_listing = text_file.read().split('\n')
	text_file.close()

	bin_arr = make_dsb(program_listing)
	print(bin_arr)

	bin_out = open("key1.dsb", 'wb')
	bin_out.write(bin_arr)
	bin_out.close()

