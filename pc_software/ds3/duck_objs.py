import os
import sys
import check_update
import json
import socket
import urllib.request

MAX_PROFILE_COUNT = 32

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
		self.script = self.load_script(path)
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

class dp_keymap(object):
	def __init__(self):
		super(dp_keymap, self).__init__()
		self.file_name = ''
		self.display_name = ''
		self.is_valid = 0
		self.content = ''
		self.url = None

	def __str__(self):
		ret = ""
		ret += str(self.display_name) + '\n'
		ret += str(self.file_name) + '\n'
		ret += str(self.url) + '\n'
		ret += str(self.is_valid) + '\n'
		ret += str(self.content[:10]) + '\n'
		ret += '------------'
		return ret

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

def load_keymap_from_file(file_path):
	this_keymap = dp_keymap()
	this_keymap.file_name = os.path.basename(file_path)
	this_keymap.display_name = this_keymap.file_name.replace('dpkm_', '').replace('.txt', '')
	with open(file_path, encoding='utf8') as keymap_file:
		this_keymap.content = keymap_file.readlines()
	this_keymap.is_valid = 1
	return this_keymap

def load_keymap(root_dir_path):
	keymap_folder_path = os.path.join(root_dir_path, 'keymaps')
	if os.path.isdir(keymap_folder_path) is False:
		return []
	keymap_file_list = [d for d in os.listdir(keymap_folder_path) if d.startswith("dpkm_") and d.endswith(".txt")]
	return_list = []
	for item in keymap_file_list:
		this_keymap = load_keymap_from_file(os.path.join(keymap_folder_path, item))
		if this_keymap.is_valid:
			return_list.append(this_keymap)
	return return_list

keymap_url = 'https://api.github.com/repos/dekuNukem/duckyPad/contents/sample_profiles/keymaps?ref=master'

def load_online_keymap():
	if check_update.is_internet_available() is False:
		return []
	file_list = json.loads(urllib.request.urlopen(keymap_url).read())
	return_list = []
	for item in file_list:
		if not ('name' in item and 'type' in item and item['type'] == 'file'):
			continue
		if not (item['name'].startswith("dpkm_") and item['name'].endswith(".txt")):
			continue
		this_keymap = dp_keymap()
		this_keymap.file_name = item['name']
		this_keymap.display_name = this_keymap.file_name.replace('dpkm_', '').replace('.txt', '')
		# this_keymap.content = str(urllib.request.urlopen(item['download_url']).read().decode('utf-8')).split('\n')
		this_keymap.url = item['download_url'] # download the content later, when saving
		this_keymap.is_valid = 1
		return_list.append(this_keymap)
	return return_list
