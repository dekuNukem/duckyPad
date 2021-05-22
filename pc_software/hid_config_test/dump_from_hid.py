"""
duckyPad HID example: HID read AND write

For more details, see:

https://github.com/dekuNukem/duckyPad-profile-autoswitcher/blob/master/HID_details.md

"""

import hid
import time

def get_duckypad_path():
	for device_dict in hid.enumerate():
	    if device_dict['vendor_id'] == 0x0483 and \
	    device_dict['product_id'] == 0xd11c and \
	    device_dict['usage'] == 58:
	    	return device_dict['path']
	return None

PC_TO_DUCKYPAD_HID_BUF_SIZE = 64
DUCKYPAD_TO_PC_HID_BUF_SIZE = 64
HID_RESPONSE_OK = 0
HID_RESPONSE_ERROR = 1
HID_RESPONSE_BUSY = 2
HID_RESPONSE_EOF = 3
HID_COMMAND_OP_RESUME = 12
HID_COMMAND_OP_ABORT = 13

h = hid.device()

duckypad_path = get_duckypad_path()
if duckypad_path is None:
	raise OSError('duckyPad Not Found!')
h.open_path(duckypad_path)

def duckypad_list_files(root_dir = None):
	ret = []
	pc_to_duckypad_buf = [0] * PC_TO_DUCKYPAD_HID_BUF_SIZE
	pc_to_duckypad_buf[0] = 5	# HID Usage ID, always 5
	pc_to_duckypad_buf[1] = 0	# Sequence Number
	pc_to_duckypad_buf[2] = 10	# Command type

	if root_dir is not None:
		for x in range(0, len(root_dir)):
			pc_to_duckypad_buf[3+x] = ord(root_dir[x])

	h.write(pc_to_duckypad_buf)
	while 1:
		result = h.read(DUCKYPAD_TO_PC_HID_BUF_SIZE)
		if len(result) == 0 or result[2] == HID_RESPONSE_EOF:
			break
		# print(result)
		this_filename = ("".join([chr(x) for x in result[4:]]).strip('\0'), result[3])
		print(this_filename)
		ret.append(this_filename)
		duckypad_hid_resume()
	return ret

def duckypad_hid_resume():
	pc_to_duckypad_buf = [0] * PC_TO_DUCKYPAD_HID_BUF_SIZE
	pc_to_duckypad_buf[0] = 5	# HID Usage ID, always 5
	pc_to_duckypad_buf[1] = 0	# Sequence Number
	pc_to_duckypad_buf[2] = HID_COMMAND_OP_RESUME	# Command type
	h.write(pc_to_duckypad_buf)

def duckypad_read_file(file_dir):
	ret = ''
	pc_to_duckypad_buf = [0] * PC_TO_DUCKYPAD_HID_BUF_SIZE
	pc_to_duckypad_buf[0] = 5	# HID Usage ID, always 5
	pc_to_duckypad_buf[1] = 0	# Sequence Number
	pc_to_duckypad_buf[2] = 11	# Command type

	for x in range(0, len(file_dir)):
		pc_to_duckypad_buf[3+x] = ord(file_dir[x])

	h.write(pc_to_duckypad_buf)

	while 1:
		result = h.read(DUCKYPAD_TO_PC_HID_BUF_SIZE)
		print(result)
		# print("".join([chr(x) for x in result]))
		ret += "".join([chr(x) for x in result[3:]]).strip('\0')
		if len(result) == 0 or result[2] == HID_RESPONSE_EOF:
			break
		duckypad_hid_resume()
	return ret


class my_file_obj(object):
	def __init__(self, name, ftype, content):
		super(my_file_obj, self).__init__()
		self.name = name
		self.content = content
		self.type = ftype
	def __str__(self):
		ret = ""
		ret += str('...............') + '\n'
		ret += "name:\t" + str(self.name) + '\n'
		ret += "type:\t" + str(self.type) + '\n'
		ret += "content:\t" + str(self.content) + '\n'
		ret += str('...............') + '\n'
		return ret

def dump_from_hid():
	file_struct_list = []

	# top level
	for item in duckypad_list_files():
		file_struct_list.append(my_file_obj(item[0], item[1], None))

	for item in file_struct_list:
		time.sleep(0.02)
		if item.type == 0:
			item.content = duckypad_read_file(item.name)
			print(item)
		if item.type == 1:
			if 'keymap' in item.name:
				continue
			files_in_this_dir = duckypad_list_files(item.name)
			for fff in files_in_this_dir:
				time.sleep(0.02)
				duckypad_read_file(item.name + "/" + fff[0])
			# input()

	# for key in file_struct_list:
	# 	print(key)
	# 	time.sleep(0.05)
	# 	if file_struct_list[key]['type'] == 0: # file
	# 		file_struct_list[key]['content'] = duckypad_read_file(key)
	# 	if file_struct_list[key]['type'] == 1: # directory
	# 		files_in_this_dir = duckypad_list_files(key)
	# 		print(files_in_this_dir)
	# 		print(file_struct_list)
	# 		# for item in files_in_this_dir:
	# 		# 	file_struct_list[key]['content'] = {'type':item[1], 'content':None}

	# 		# file_struct_list[key]['content'] = 
	# 		input()

	# [print(x) for x in file_struct_list]

start = time.time()
dump_from_hid()
print('took', time.time() - start)


# duckypad_read_file("dp_settings.txt")

h.close()
# duckypad_read_file("keymaps/dpkm_Finnish.txt")

