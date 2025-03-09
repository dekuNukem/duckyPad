
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

HID_COMMAND_ENTER_EXCLUSIVE_MODE = 31
HID_COMMAND_DUMP_SD = 32

pc_to_duckypad_buf = [0] * PC_TO_DUCKYPAD_HID_BUF_SIZE
pc_to_duckypad_buf[0] = 5   # HID Usage ID, always 5
pc_to_duckypad_buf[1] = 0   # unused
pc_to_duckypad_buf[2] = HID_COMMAND_DUMP_SD # Command type

bbbb = millis()

duckypad_path = get_duckypad_path()
if duckypad_path is None:
    raise OSError('duckyPad Not Found!')
h.open_path(duckypad_path)

SD_WALK_OP_TYPE_INDEX = 1

SD_WALK_OP_ACK = 0
SD_WALK_OP_NEW_DIR = 1
SD_WALK_OP_FILE_CONTENT = 2
SD_WALK_OP_FILE_MD5 = 3
SD_WALK_OP_EOT = 4

current_dir = None

def dump_file(file_path, file_name, file_content):
    file_path = file_path.lstrip("\\/")
    script_dir = os.path.dirname(os.path.abspath(__file__))
    full_dir_path = os.path.join(script_dir, "hid_dump", file_path)
    full_file_path = os.path.join(full_dir_path, file_name)
    print(full_dir_path)
    print(full_file_path)
    print('----------')
    os.makedirs(full_dir_path, exist_ok=True)
    with open(full_file_path, 'wb') as file:
        file.write(file_content)

while 1:
    # now = millis()
    # print("\n\nSending to duckyPad:\n", pc_to_duckypad_buf)
    h.write(pc_to_duckypad_buf)
    duckypad_to_pc_buf = h.read(DUCKYPAD_TO_PC_HID_BUF_SIZE)
    # print("\nduckyPad response:\n", duckypad_to_pc_buf)
    # print("took", millis() - now, "ms")
    if duckypad_to_pc_buf[SD_WALK_OP_TYPE_INDEX] == SD_WALK_OP_ACK:
        continue

    elif duckypad_to_pc_buf[SD_WALK_OP_TYPE_INDEX] == SD_WALK_OP_EOT:
        break

    elif duckypad_to_pc_buf[SD_WALK_OP_TYPE_INDEX] == SD_WALK_OP_NEW_DIR:
        rawchar = duckypad_to_pc_buf[2:]
        current_dir = ''.join(chr(c) for c in rawchar[:rawchar.index(0)])
        print(current_dir)

    elif duckypad_to_pc_buf[SD_WALK_OP_TYPE_INDEX] == SD_WALK_OP_FILE_MD5:
        rawchar = duckypad_to_pc_buf[18:]
        this_file_name = ''.join(chr(c) for c in rawchar[:rawchar.index(0)])
        md5_list = duckypad_to_pc_buf[2:18]
        md5_string = ''.join(f'{x:02x}' for x in md5_list)
        print(this_file_name, md5_string)

    elif duckypad_to_pc_buf[SD_WALK_OP_TYPE_INDEX] == SD_WALK_OP_FILE_CONTENT:
        file_name_end = duckypad_to_pc_buf[2] + 1
        file_content_end = duckypad_to_pc_buf[3] + 1
        raw_filename_list = duckypad_to_pc_buf[4:file_name_end]
        this_file_name = ''.join(chr(c) for c in raw_filename_list[:raw_filename_list.index(0)])
        raw_file_content_bytes = bytes(duckypad_to_pc_buf[file_name_end:file_content_end])
        dump_file(current_dir, this_file_name, raw_file_content_bytes)
        print(this_file_name, raw_file_content_bytes)

h.close()
print("total time:", millis() - bbbb, "ms")


