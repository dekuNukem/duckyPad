

def hid_read():
	wait_start = time.time()
	while time.time() - wait_start <= 1.5:
		result = h.read(DUCKYPAD_TO_PC_HID_BUF_SIZE)
	print(result)
	return result

print(timeit.timeit(duckypad_hid_open, number=1))







import hid
import time

def get_duckypad_path():
	for device_dict in hid.enumerate():
	    if 'dekuNukem' in device_dict['manufacturer_string'] and \
	    'duckyPad(2020)' in device_dict['product_string'] and \
	    int(device_dict['usage']) == 58:
	    	return device_dict['path']
	return None

duckypad_path = get_duckypad_path()
if duckypad_path is None:
	print("didn't find any duckypads")
	exit()
print("Opening", duckypad_path)

h = hid.device()
h.open_path(duckypad_path)

print("Manufacturer: %s" % h.get_manufacturer_string())
print("Product: %s" % h.get_product_string())
print("Serial No: %s" % h.get_serial_number_string())
# print(h.get_input_report(1,1))
# # enable non-blocking mode
# h.set_nonblocking(1)

# write some data to the device

print("Writing data...")
buffff = [0] * 64
buffff[0] = 5
buffff[1] = 255
buffff[2] = 3
print(h.write(buffff))

# wait
# time.sleep(0.1)

# read back the answer
print("Read the data")
result = h.read(32)
print(result)
print(len(result))

print("Closing the device")
h.close()

# test it out by running this script
if __name__ == "__main__":
	for device_dict in hid.enumerate():
	    keys = list(device_dict.keys())
	    keys.sort()
	    for key in keys:
	        print("%s : %s" % (key, device_dict[key]))
	    print()