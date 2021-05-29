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
HID_COMMAND_OPEN_FILE_FOR_WRITING = 14
HID_COMMAND_WRITE_FILE = 15
HID_COMMAND_CLOSE_FILE = 16
HID_COMMAND_DELETE_FILE = 17
HID_COMMAND_CREATE_DIR = 18
HID_COMMAND_DELETE_DIR = 19

h = hid.device()

def duckypad_hid_init():
	duckypad_path = get_duckypad_path()
	if duckypad_path is None:
		raise OSError('duckyPad Not Found!')
	h.open_path(duckypad_path)

def duckypad_hid_close():
	h.close()

def duckypad_open_file_for_writing(file_dir):
	pc_to_duckypad_buf = [0] * PC_TO_DUCKYPAD_HID_BUF_SIZE
	pc_to_duckypad_buf[0] = 5	# HID Usage ID, always 5
	pc_to_duckypad_buf[1] = 0	# Sequence Number
	pc_to_duckypad_buf[2] = HID_COMMAND_OPEN_FILE_FOR_WRITING	# Command type

	for x in range(0, len(file_dir)):
		pc_to_duckypad_buf[3+x] = ord(file_dir[x])

	h.write(pc_to_duckypad_buf)

	result = h.read(DUCKYPAD_TO_PC_HID_BUF_SIZE)
	print(result)

def duckypad_close_file():
	pc_to_duckypad_buf = [0] * PC_TO_DUCKYPAD_HID_BUF_SIZE
	pc_to_duckypad_buf[0] = 5	# HID Usage ID, always 5
	pc_to_duckypad_buf[1] = 0	# Sequence Number
	pc_to_duckypad_buf[2] = HID_COMMAND_CLOSE_FILE	# Command type

	h.write(pc_to_duckypad_buf)

	result = h.read(DUCKYPAD_TO_PC_HID_BUF_SIZE)
	print(result)

def duckypad_write_file_one_line(content):
	pc_to_duckypad_buf = [0] * PC_TO_DUCKYPAD_HID_BUF_SIZE
	pc_to_duckypad_buf[0] = 5	# HID Usage ID, always 5
	pc_to_duckypad_buf[1] = 0	# Sequence Number
	pc_to_duckypad_buf[2] = HID_COMMAND_WRITE_FILE	# Command type

	if len(content) > 60:
		raise ValueError("content too long")

	for x in range(0, len(content)):
		pc_to_duckypad_buf[3+x] = ord(content[x])

	h.write(pc_to_duckypad_buf)

	result = h.read(DUCKYPAD_TO_PC_HID_BUF_SIZE)
	print(result)

def duckypad_write_file(content):
	n=60
	line_list = [content[i:i+n] for i in range(0, len(content), n)]
	print(line_list)
	for line in line_list:
		duckypad_write_file_one_line(line)

def duckypad_delete_file(file_name):
	pc_to_duckypad_buf = [0] * PC_TO_DUCKYPAD_HID_BUF_SIZE
	pc_to_duckypad_buf[0] = 5	# HID Usage ID, always 5
	pc_to_duckypad_buf[1] = 0	# Sequence Number
	pc_to_duckypad_buf[2] = HID_COMMAND_DELETE_FILE	# Command type

	for x in range(0, len(file_name)):
		pc_to_duckypad_buf[3+x] = ord(file_name[x])

	h.write(pc_to_duckypad_buf)

	result = h.read(DUCKYPAD_TO_PC_HID_BUF_SIZE)
	print(result)

def duckypad_create_dir(dir_name):
	pc_to_duckypad_buf = [0] * PC_TO_DUCKYPAD_HID_BUF_SIZE
	pc_to_duckypad_buf[0] = 5	# HID Usage ID, always 5
	pc_to_duckypad_buf[1] = 0	# Sequence Number
	pc_to_duckypad_buf[2] = HID_COMMAND_CREATE_DIR	# Command type

	for x in range(0, len(dir_name)):
		pc_to_duckypad_buf[3+x] = ord(dir_name[x])

	h.write(pc_to_duckypad_buf)

	result = h.read(DUCKYPAD_TO_PC_HID_BUF_SIZE)
	print(result)

def duckypad_delete_dir(dir_name):
	pc_to_duckypad_buf = [0] * PC_TO_DUCKYPAD_HID_BUF_SIZE
	pc_to_duckypad_buf[0] = 5	# HID Usage ID, always 5
	pc_to_duckypad_buf[1] = 0	# Sequence Number
	pc_to_duckypad_buf[2] = HID_COMMAND_DELETE_DIR	# Command type
	for x in range(0, len(dir_name)):
		pc_to_duckypad_buf[3+x] = ord(dir_name[x])
	h.write(pc_to_duckypad_buf)
	result = h.read(DUCKYPAD_TO_PC_HID_BUF_SIZE)
	print(result)


# sssss = "Extreme E's boat full of race cars has docked in Senegal after kicking off its season in Saudi Arabia (now home of the Dakar Rally), where it's racing this weekend at Lac Rose, the original site of the Dakar. The whole idea of the series is to go off-roading in some of the environments most vulnerable to pollution and climate change caused by humans, with the Ocean X Prix aimed to highlight the problems of over-fishing, sea plastics, destruction of habitats like coral reefs and the million other really bad things we keep putting in the sea."


# start = time.time()
# # duckypad_open_file_for_writing('test.txt')
# # duckypad_write_file(sssss)
# # duckypad_close_file()
# duckypad_delete_dir("/")
# print('took', time.time() - start)

