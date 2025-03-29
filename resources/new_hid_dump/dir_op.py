import os
import hid
import time

def millis():
    return time.time_ns() // 1000000;

PC_TO_DUCKYPAD_HID_BUF_SIZE = 64
DUCKYPAD_TO_PC_HID_BUF_SIZE = 64

duckypad_pid = 0xd11c
duckypad_pro_pid = 0xd11d
valid_pid_list = [duckypad_pro_pid, duckypad_pid]

def get_duckypad_path_uncached():
    path_dict = {}
    for device_dict in hid.enumerate():
        if device_dict['vendor_id'] == 0x0483 and device_dict['product_id'] in valid_pid_list:
            path_dict[device_dict['usage']] = device_dict['path']
    if len(path_dict) == 0:
        return None
    if 58 in path_dict:
        return path_dict[58]
    return list(path_dict.values())[0]

last_dp_path = None
def get_duckypad_path(start_fresh=False):
    global last_dp_path
    if start_fresh:
        last_dp_path = None
    if last_dp_path is None:
        last_dp_path = get_duckypad_path_uncached()
    return last_dp_path

h = hid.device()

HID_COMMAND_OPEN_FILE_FOR_READING = 33
HID_COMMAND_READ_FILE = 11
HID_COMMAND_OPEN_FILE_FOR_WRITING = 14
HID_COMMAND_WRITE_FILE = 15
HID_COMMAND_CLOSE_FILE = 16
HID_COMMAND_CREATE_DIR = 18
HID_COMMAND_DELETE_DIR = 19

pc_to_duckypad_buf = [0] * PC_TO_DUCKYPAD_HID_BUF_SIZE
pc_to_duckypad_buf[0] = 5   # HID Usage ID, always 5
pc_to_duckypad_buf[1] = 0   # unused
pc_to_duckypad_buf[2] = HID_COMMAND_DELETE_DIR # Command type

HID_READ_FILE_PATH_SIZE_MAX = 55

pathname = "/profile_Welcome"

if len(pathname) > HID_READ_FILE_PATH_SIZE_MAX:
    raise OSError("file path too long")

for index, value in enumerate(pathname):
    pc_to_duckypad_buf[3+index] = ord(value)

duckypad_path = get_duckypad_path()
if duckypad_path is None:
    raise OSError('duckyPad Not Found!')
h.open_path(duckypad_path)

print("\n\nSending to duckyPad:\n", pc_to_duckypad_buf)
h.write(pc_to_duckypad_buf)
duckypad_to_pc_buf = h.read(DUCKYPAD_TO_PC_HID_BUF_SIZE)
print("\nduckyPad response:\n", duckypad_to_pc_buf)

h.close()
