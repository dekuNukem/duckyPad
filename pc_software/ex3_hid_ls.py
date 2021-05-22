"""
duckyPad HID example: HID read AND write

For more details, see:

https://github.com/dekuNukem/duckyPad-profile-autoswitcher/blob/master/HID_details.md

"""

import hid
import time

PC_TO_DUCKYPAD_HID_BUF_SIZE = 64
DUCKYPAD_TO_PC_HID_BUF_SIZE = 64

h = hid.device()

def get_duckypad_path():
	for device_dict in hid.enumerate():
	    if device_dict['vendor_id'] == 0x0483 and \
	    device_dict['product_id'] == 0xd11c and \
	    device_dict['usage'] == 58:
	    	return device_dict['path']
	return None

# wait up to 0.5 seconds for response
def hid_read():
	read_start = time.time()
	while time.time() - read_start <= 0.5:
		result = h.read(DUCKYPAD_TO_PC_HID_BUF_SIZE)
		if len(result) > 0:
			return result
		time.sleep(0.01)
	return []

HID_RESPONSE_OK = 0
HID_RESPONSE_ERROR = 1
HID_RESPONSE_BUSY = 2
HID_RESPONSE_EOF = 3

def duckypad_list_files(root_dir = None):
	pc_to_duckypad_buf = [0] * PC_TO_DUCKYPAD_HID_BUF_SIZE
	pc_to_duckypad_buf[0] = 5	# HID Usage ID, always 5
	pc_to_duckypad_buf[1] = 0	# Sequence Number
	pc_to_duckypad_buf[2] = 10	# Command type

	duckypad_path = get_duckypad_path()
	if duckypad_path is None:
		raise OSError('duckyPad Not Found!')
	h.open_path(duckypad_path)
	h.set_nonblocking(1)
	h.write(pc_to_duckypad_buf)
	while 1:
		result = hid_read()
		print(result)
		print("".join([chr(x) for x in result]))
		if len(result) == 0 or result[2] == HID_RESPONSE_EOF:
			break
	h.close()

# dir_name = "keymaps"
# for x in range(0, len(dir_name)):
# 	pc_to_duckypad_buf[3+x] = ord(dir_name[x])

duckypad_list_files()



