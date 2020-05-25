import os
import sys

class dp_key(object):
	def read_file(self, path):
		with open(path) as keyfile:
			self.script = keyfile.read()

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

	def __init__(self, path):
		super(dp_key, self).__init__()
		temp_split = os.path.basename(os.path.normpath(path)).rstrip('.txt').lstrip('key').split('_', 1)
		self.path = path
		self.name = temp_split[1]
		self.index = int(temp_split[0])
		self.color = None
		self.script = None
		self.read_file(path)
		try:
			config_path = os.path.join(os.path.dirname(path), "config.txt")
			self.read_color(config_path)
		except Exception as e:
			print(">>> read_color:", config_path, e)

# -----------------------------------------------------------

class dp_profile(object):
	def read_keys(self, path):
		key_file_list = [x for x in os.listdir(path) if x.endswith('.txt') and x.startswith('key') and '_' in x and x[3].isnumeric()]
		key_file_list.sort(key=lambda s: int(s[3:].split("_")[0])) # sort by number not by letter
		for item in key_file_list:
			self.keylist.append(dp_key(os.path.join(path, item)))

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

	def __str__(self):
		ret = ""
		ret += str('-----Profile info-----') + '\n'
		ret += "path:\t" + str(self.path) + '\n'
		ret += "name:\t" + str(self.name) + '\n'
		ret += "bg_color:\t" + str(self.bg_color) + '\n'
		ret += "kd_color:\t" + str(self.kd_color) + '\n'
		ret += "dim_unused:\t" + str(self.dim_unused) + '\n'
		ret += "key count:\t" + str(len(self.keylist)) + '\n'
		ret += "keys:\n"
		for item in self.keylist:
			ret += str(item)
		ret += str('----------------------') + '\n'
		return ret

	def __init__(self, path):
		super(dp_profile, self).__init__()
		folder_name = os.path.basename(os.path.normpath(path))
		if not (folder_name.startswith('profile') and folder_name[7].isnumeric() and '_' in folder_name):
			print("invalid profile folder:", folder_name)
			raise ValueError
		self.path = path
		self.name = folder_name.split('_', 1)[-1]
		self.keylist = []
		self.bg_color = None
		self.kd_color = None
		self.dim_unused = True
		self.read_config(path)
		self.read_keys(path)
		
def build_profile(root_dir_path):
	my_dirs = [d for d in os.listdir(root_dir_path) if os.path.isdir(os.path.join(root_dir_path, d))]
	my_dirs = [x for x in my_dirs if x.startswith('profile') and x[7].isnumeric() and '_' in x]
	my_dirs.sort(key=lambda s: int(s[7:].split("_")[0]))
	my_dirs = [os.path.join(root_dir_path, d) for d in my_dirs if d.startswith("profile")]
	profile_list = []
	for item in my_dirs:
		profile_list.append(dp_profile(item))
	return profile_list



