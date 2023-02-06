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
	return []

def duckypad_hid_write(hid_buf_64b):
	if len(hid_buf_64b) != PC_TO_DUCKYPAD_HID_BUF_SIZE:
		raise ValueError('PC-to-duckyPad buffer wrong size, should be exactly 64 Bytes')
	duckypad_path = get_duckypad_path()
	if duckypad_path is None:
		raise OSError('duckyPad Not Found!')
	h.open_path(duckypad_path)
	h.set_nonblocking(1)
	h.write(hid_buf_64b)
	result = hid_read()
	h.close()
	return result

pc_to_duckypad_buf = [0] * PC_TO_DUCKYPAD_HID_BUF_SIZE
pc_to_duckypad_buf[0] = 5	# HID Usage ID, always 5
pc_to_duckypad_buf[1] = 0	# Sequence Number
pc_to_duckypad_buf[2] = 0xa	# Command type
print("\n\nSending to duckyPad:\n", pc_to_duckypad_buf)
now = time.time()
duckypad_to_pc_buf = duckypad_hid_write(pc_to_duckypad_buf)
print(time.time() - now)
print("\nduckyPad response:\n", bytearray(duckypad_to_pc_buf))

pc_to_duckypad_buf = [0] * PC_TO_DUCKYPAD_HID_BUF_SIZE
pc_to_duckypad_buf[0] = 5	# HID Usage ID, always 5
pc_to_duckypad_buf[1] = 0	# Sequence Number
pc_to_duckypad_buf[2] = 0xc	# Command type
duckypad_to_pc_buf = duckypad_hid_write(pc_to_duckypad_buf)
print(time.time() - now)
print("\nduckyPad response:\n", bytearray(duckypad_to_pc_buf))

"""
duckypad HID write uses strlen, which terminates at 0
need to change the firmware side for explicit length, maybe use sequence number
"""