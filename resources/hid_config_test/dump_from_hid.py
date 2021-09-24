"""
duckyPad HID example: HID read AND write

For more details, see:

https://github.com/dekuNukem/duckyPad-profile-autoswitcher/blob/master/HID_details.md

"""

import os
import hid
import time
import shutil

def ensure_dir(dir_path):
    if not os.path.exists(dir_path):
        os.makedirs(dir_path)

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
# HID_WAIT_TIME = 0.005

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
		# time.sleep(HID_WAIT_TIME)
		result = h.read(DUCKYPAD_TO_PC_HID_BUF_SIZE)
		if len(result) == 0 or result[2] == HID_RESPONSE_EOF:
			break
		if result[2] == HID_RESPONSE_BUSY or result[2] == HID_RESPONSE_ERROR:
			raise OSError("HID read error or busy")
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
		# time.sleep(HID_WAIT_TIME)
		result = h.read(DUCKYPAD_TO_PC_HID_BUF_SIZE)
		if result[2] == HID_RESPONSE_BUSY or result[2] == HID_RESPONSE_ERROR:
			raise OSError("HID read error or busy")
		# print(result)
		print("".join([chr(x) for x in result]))
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

file_struct_list = []

def dump_from_hid():
	global file_struct_list

	file_struct_list = []

	# top level
	for item in duckypad_list_files():
		file_struct_list.append(my_file_obj(item[0], item[1], None))

	for item in file_struct_list:
		if item.type == 0:
			item.content = duckypad_read_file(item.name)
		if item.type == 1:
			if 'keymap' in item.name:
				continue
			files_in_this_dir = duckypad_list_files(item.name)
			lv2_list = []
			for fff in files_in_this_dir:
				if fff[1] != 0:
					continue
				lv2_list.append(my_file_obj(fff[0], fff[1], duckypad_read_file(item.name + "/" + fff[0])))
			item.content = lv2_list

	# pickle.dump(file_struct_list, open("save.p", "wb" ))

start = time.time()
dump_from_hid()
print('took', time.time() - start)
h.close()
input()

out_folder_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "hid_dump")

try:
    shutil.rmtree(out_folder_path)
    time.sleep(0.05)
except FileNotFoundError:
    pass
ensure_dir(out_folder_path)


for item in file_struct_list:
	if item.type == 0 and item.content is not None:
		with open(os.path.join(out_folder_path, item.name), 'w') as this_file:
			this_file.write(item.content)

	if item.type == 1 and item.content is not None:
		this_folder_path = os.path.join(out_folder_path, item.name)
		ensure_dir(this_folder_path)
		for subfile in item.content:
			if subfile.type == 0 and subfile.content is not None:
				with open(os.path.join(this_folder_path, subfile.name), 'w') as this_file:
					this_file.write(subfile.content)