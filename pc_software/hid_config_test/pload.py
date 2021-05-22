import os
import time
import shutil
import pickle

class my_file_obj(object):
	def __init__(self, name, ftype, content):
		super(my_file_obj, self).__init__()
		self.name = name
		self.content = content
		self.type = ftype
	def __str__(self):
		ret = ""
		ret += str('...............') + '\n'
		ret += "name:\t" + str(self.name) + '\n'
		ret += "type:\t" + str(self.type) + '\n'
		ret += "content:\n" + str(self.content) + '\n'
		ret += str('...............') + '\n'
		return ret

def ensure_dir(dir_path):
    if not os.path.exists(dir_path):
        os.makedirs(dir_path)


file_struct_list = pickle.load(open("save.p", "rb"))

# for item in file_struct_list:
# 	if item.type == 0:
# 		print(item)
# 		continue
# 	if item.content is None:
# 		continue
# 	for fff in item.content:
# 		print(fff)

out_folder_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "hid_dump")

try:
    shutil.rmtree(out_folder_path)
    time.sleep(0.05)
except FileNotFoundError:
    pass
ensure_dir(out_folder_path)


for item in file_struct_list:
	if item.type == 0 and item.content is not None:
		with open(os.path.join(out_folder_path, item.name), 'w') as this_file:
			this_file.write(item.content)

	if item.type == 1 and item.content is not None:
		this_folder_path = os.path.join(out_folder_path, item.name)
		ensure_dir(this_folder_path)
		for subfile in item.content:
			if subfile.type == 0 and subfile.content is not None:
				with open(os.path.join(this_folder_path, subfile.name), 'w') as this_file:
					this_file.write(subfile.content)