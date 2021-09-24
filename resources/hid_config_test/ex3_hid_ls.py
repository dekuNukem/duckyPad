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

h = hid.device()

duckypad_path = get_duckypad_path()
if duckypad_path is None:
	raise OSError('duckyPad Not Found!')
h.open_path(duckypad_path)

# wait up to 0.5 seconds for response
# def hid_read():
# 	read_start = time.time()
# 	while time.time() - read_start <= 0.5:
# 		result = h.read(DUCKYPAD_TO_PC_HID_BUF_SIZE)
# 		if len(result) > 0:
# 			return result
# 		time.sleep(0.01)
# 	return []

HID_RESPONSE_OK = 0
HID_RESPONSE_ERROR = 1
HID_RESPONSE_BUSY = 2
HID_RESPONSE_EOF = 3

HID_COMMAND_OP_RESUME = 12
HID_COMMAND_OP_ABORT = 13

def duckypad_list_files(root_dir = None):
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
		print(result)
		print("".join([chr(x) for x in result]))
		if len(result) == 0 or result[2] == HID_RESPONSE_EOF:
			break
		duckypad_hid_resume()
	h.close()

def duckypad_hid_resume():
	pc_to_duckypad_buf = [0] * PC_TO_DUCKYPAD_HID_BUF_SIZE
	pc_to_duckypad_buf[0] = 5	# HID Usage ID, always 5
	pc_to_duckypad_buf[1] = 0	# Sequence Number
	pc_to_duckypad_buf[2] = HID_COMMAND_OP_RESUME	# Command type
	h.write(pc_to_duckypad_buf)

def duckypad_read_file(file_dir):
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
		print("".join([chr(x) for x in result]))
		if len(result) == 0 or result[2] == HID_RESPONSE_EOF:
			break
		duckypad_hid_resume()
		

start = time.time()
duckypad_list_files('')
print('took', time.time() - start)


# duckypad_read_file("profile1_windows/key7.txt")

h.close()
# duckypad_read_file("keymaps/dpkm_Finnish.txt")

"""

put in file read mode? then keep file open and do call and response? should be much faster

"""