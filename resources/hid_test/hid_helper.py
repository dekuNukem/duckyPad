import hid
import time
import timeit

PC_TO_DUCKYPAD_HID_BUF_SIZE = 64
DUCKYPAD_TO_PC_HID_BUF_SIZE = 32

h = hid.device()

def get_duckypad_path():
	for device_dict in hid.enumerate():
	    if 'dekuNukem' in device_dict['manufacturer_string'] and \
	    'duckyPad(2020)' in device_dict['product_string'] and \
	    int(device_dict['usage']) == 58:
	    	return device_dict['path']
	return None

def hid_read():
	read_start = time.time()
	while time.time() - read_start <= 0.5:
		result = h.read(DUCKYPAD_TO_PC_HID_BUF_SIZE)
		if len(result) > 0:
			return result
		time.sleep(0.05)
	return []

def duckypad_hid_write(hid_buf_64b):
	if len(hid_buf_64b) != PC_TO_DUCKYPAD_HID_BUF_SIZE:
		raise ValueError('PC to duckyPad buffer wrong size, should be exactly 64 Bytes')
	duckypad_path = get_duckypad_path()
	if duckypad_path is None:
		raise OSError('duckyPad Not Found!')
	h.open_path(duckypad_path)
	h.set_nonblocking(1)
	h.write(hid_buf_64b)
	result = hid_read()
	h.close()
	return result

# write some data to the device

# print("Writing data...")
buffff = [0] * 64
buffff[0] = 5
buffff[1] = 255
buffff[2] = 3

print(timeit.timeit(lambda: duckypad_hid_write(buffff), number=1))

# test it out by running this script
# if __name__ == "__main__":
# 	for device_dict in hid.enumerate():
# 	    keys = list(device_dict.keys())
# 	    keys.sort()
# 	    for key in keys:
# 	        print("%s : %s" % (key, device_dict[key]))
# 	    print()