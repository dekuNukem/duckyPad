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


class dpas_rule(object):
	def __init__(self, path=None):
		super(dpas_rule, self).__init__()
		self.app_name_contains = None
		self.window_title_contains = None
		self.compare_logic = None
		self.window_title_enabled = False
		self.app_name_enabled = False
		self.rule_enabled = False
		self.list_str = 'unknown'
