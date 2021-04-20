import time
import hid_rw

def duckypad_get_info():
	dpinfo = {}
	dpinfo['model'], dpinfo['serial'] = hid_rw.get_duckypad_hid_info()
	buffff = [0] * 64
	buffff[0] = 5
	result = hid_rw.duckypad_hid_write(buffff)
	dpinfo['fw_ver'] = f'{result[3]}.{result[4]}.{result[5]}'
	return dpinfo