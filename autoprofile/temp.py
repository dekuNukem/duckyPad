import hid_rw
buffff = [0] * 64
buffff[0] = 5
buffff[2] = 3
hid_rw.duckypad_hid_write(buffff)
        # print(item)
        # print(app_name_condition, window_title_condition, result)
    print('----------------')
def hid_read():
	wait_start = time.time()
	while time.time() - wait_start <= 1.5:
		result = h.read(DUCKYPAD_TO_PC_HID_BUF_SIZE)
	print(result)
	return result

print(timeit.timeit(duckypad_hid_open, number=1))


    if check_profile_number(switch_to_entrybox.get()) is None:
        # messagebox.showerror("Error", "Invalid profile number!\n\nMust be an integer between 1 and 31")
        window.destroy()
        return

class dpas_rule(object):
	def __init__(self, path=None):
		super(dpas_rule, self).__init__()
		self.app_name_contains = None
		self.window_title_contains = None
		self.switch_to = None
		self.enabled = False
		self.list_str = 'unknown'




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