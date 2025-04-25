import os
import sys
import json
import socket
import urllib.request

MAX_PROFILE_COUNT = 64

class dp_key(object):
	def load_script(self, path):
		try:
			with open(path, encoding='utf8') as keyfile:
				return keyfile.read()
		except Exception as e:
			print('load_script exception:', e)
			return ""

	def read_color(self, config_path):
		with open(config_path) as configfile:
			for line in configfile:
				line = line.replace('\n', '').replace('\r', '')
				hotword = "SWCOLOR_" + str(self.index) + ' '
				while('  ' in line):
					line = line.replace('  ', ' ')
				if hotword in line:
					temp_split = line.split(' ')
					self.color = (int(temp_split[1]), int(temp_split[2]), int(temp_split[3]))

	def __str__(self):
		ret = ""
		ret += str('...............') + '\n'
		ret += "path:\t" + str(self.path) + '\n'
		ret += "name:\t" + str(self.name) + '\n'
		ret += "index:\t" + str(self.index) + '\n'
		ret += "color:\t" + str(self.color) + '\n'
		ret += "script:\t" + str(len(self.script)) + " characters\n"
		ret += str('...............') + '\n'
		return ret

	def get_keyname(self, path, index):
		ret = 'ERR' + str(index)
		try:
			with open(os.path.join(os.path.dirname(path), "config.txt")) as ffffff:
				for line in ffffff:
					this_split = line.replace('\n','').replace('\r','').split(' ', 1)
					if this_split[0].startswith('z') and int(this_split[0][1:]) == index:
						ret = this_split[1]
						break
		except Exception as e:
			print('get_keyname exception:', e)
			pass
		return ret

	def __init__(self, path=None):
		super(dp_key, self).__init__()
		self.path = path
		self.name = None
		self.index = None
		self.color = None
		self.has_loop = False
		self.max_loop = 0
		self.script = ''
		self.binary_array = None
		if path is None:
			return
		self.index = int(os.path.basename(os.path.normpath(path)).split("_")[0].split(".txt")[0].strip('key'))
		if '_' in os.path.basename(os.path.normpath(path)):
			self.name = os.path.basename(os.path.normpath(path)).rsplit('.', 1)[0].split('_', 1)[-1]
		else:
			self.name = self.get_keyname(path, self.index)
		self.color = None
		self.script = self.load_script(path).replace('\r', '')
		try:
			config_path = os.path.join(os.path.dirname(path), "config.txt")
			self.read_color(config_path)
		except Exception as e:
			print(">>> read_color:", config_path, e)
			pass

# -----------------------------------------------------------

class dp_profile(object):
	def read_keys(self, path):
		key_file_list = [x for x in os.listdir(path) if x.endswith('.txt') and x.startswith('key') and x[3].isnumeric()]
		key_file_list.sort(key=lambda s: int(s[3:].split("_")[0].split(".txt")[0])) # sort by number not by letter
		# print(key_file_list)
		for item in key_file_list:
			this_key = dp_key(os.path.join(path, item))
			self.keylist[this_key.index - 1] = this_key

	def read_config(self, path):
		try:
			with open(os.path.join(path, "config.txt")) as configfile:
				for line in configfile:
					line = line.replace('\n', '').replace('\r', '')
					while('  ' in line):
						line = line.replace('  ', ' ')
					if line.startswith('BG_COLOR '):
						temp_split = line.split(' ')
						self.bg_color = (int(temp_split[1]), int(temp_split[2]), int(temp_split[3]))
					if line.startswith('KEYDOWN_COLOR '):
						temp_split = line.split(' ')
						self.kd_color = (int(temp_split[1]), int(temp_split[2]), int(temp_split[3]))
					if line.startswith("DIM_UNUSED_KEYS 0"):
						self.dim_unused = False
		except Exception as e:
			print('>>>>> read_config:', path, e)
			pass

	def load_from_path(self, path):
		folder_name = os.path.basename(os.path.normpath(path))
		if not (folder_name.startswith('profile') and folder_name[7].isnumeric() and '_' in folder_name):
			print("invalid profile folder:", folder_name)
			return
		self.path = path
		self.name = folder_name.split('_', 1)[-1]
		self.read_config(path)
		self.read_keys(path)

	def __str__(self):
		ret = ""
		ret += str('-----Profile info-----') + '\n'
		ret += "path:\t" + str(self.path) + '\n'
		ret += "name:\t" + str(self.name) + '\n'
		ret += "bg_color:\t" + str(self.bg_color) + '\n'
		ret += "kd_color:\t" + str(self.kd_color) + '\n'
		ret += "dim_unused:\t" + str(self.dim_unused) + '\n'
		ret += "key count:\t" + str(len([x for x in self.keylist if x is not None])) + '\n'
		ret += "keys:\n"
		for item in [x for x in self.keylist]:
			ret += str(item) + '\n'
		ret += str('----------------------') + '\n'
		return ret

	def __init__(self):
		super(dp_profile, self).__init__()
		self.key_count = 15
		self.path = None
		self.name = None
		self.keylist = [None] * self.key_count
		self.bg_color = (84,22,180)
		self.kd_color = None
		self.dim_unused = True

class dp_global_settings(object):
	def load_from_path(self, path):
		try:
			with open(os.path.join(path, 'dp_settings.txt')) as settings_file:
				file_lines = settings_file.readlines()
				self.list_of_lines = file_lines
				for line in file_lines:
					line = line.replace('\n', '').replace('\r', '')
					if 'sleep_after_min' in line:
						self.sleep_after_minutes = int(line.split(' ')[-1])
		except Exception as e:
			print('dp_global_settings load_from_path:', e)
	def __init__(self):
		super(dp_global_settings, self).__init__()
		self.sleep_after_minutes = 30
		self.list_of_lines = []

def build_profile(root_dir_path):
	my_dirs = [d for d in os.listdir(root_dir_path) if os.path.isdir(os.path.join(root_dir_path, d))]
	my_dirs = [x for x in my_dirs if x.startswith('profile') and x[7].isnumeric() and '_' in x]
	my_dirs.sort(key=lambda s: int(s[7:].split("_")[0]))
	my_dirs = [os.path.join(root_dir_path, d) for d in my_dirs if d.startswith("profile")]
	my_dirs = my_dirs[:MAX_PROFILE_COUNT]
	profile_list = []
	for item in my_dirs:
		this_profile = dp_profile()
		this_profile.load_from_path(item)
		profile_list.append(this_profile)
	return profile_list

def import_profile_single(root_dir_path):
	this_profile = dp_profile()
	this_profile.load_from_path(root_dir_path)
	return this_profile

def import_profile(root_dir_path):
	try:
		key_file_list = [x for x in os.listdir(root_dir_path) if x.endswith('.txt') and x.startswith('key') and x[3].isnumeric()]
		if len(key_file_list) != 0:
			return True, [import_profile_single(root_dir_path)]
	except Exception as e:
		return False, str(e)
	try:
		return True, build_profile(root_dir_path)
	except Exception as e:
		return False, str(e)
	return False, "unknown error"

# fff = import_profile("sample_profiles/profile1_windows")
# print(fff)