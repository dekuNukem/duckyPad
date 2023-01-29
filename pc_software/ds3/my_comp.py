import os
import sys
import hashlib

duckypad_file_whitelist = [
	"profile",
	"dp_settings",
	"dpkm_",
	"config",
	"key",
]

duckypad_file_blacklist = [
	"keymaps",
	"dsb",
]

def is_duckypad_file(name):
	for item in duckypad_file_blacklist:
		if item in name:
			return False
	for item in duckypad_file_whitelist:
		if item in name:
			return True
	return False

def is_file_different(file1, file2):
	hash1 = hashlib.md5(open(file1,'rb').read()).hexdigest()
	hash2 = hashlib.md5(open(file2,'rb').read()).hexdigest()
	if hash1 != hash2:
		print(f"{file1} and {file2} are different!")
		# print(hash1, hash2)
		print(open(file1,'rb').read())
		print(open(file2,'rb').read())
	return hash1 != hash2

def compare(old_path, new_path):
	old_files = set([x for x in os.listdir(old_path) if is_duckypad_file(x)])
	new_files = set([x for x in os.listdir(new_path) if is_duckypad_file(x)])

	# files in new but not old
	files_to_add = new_files - old_files
	# files in old but not new
	files_to_delete = old_files - new_files

	files_in_both = new_files.intersection(old_files)
	files_with_difference = set()

	for item in files_in_both:
		new_full_path = os.path.join(new_path, item)
		old_full_path = os.path.join(old_path, item)
		if os.path.isdir(new_full_path):
			continue
		if is_file_different(old_full_path, new_full_path):
			files_with_difference.add(item)

	common_dirs = [x for x in files_in_both if os.path.isdir(os.path.join(new_path, x))]

	return files_to_add, files_to_delete, files_with_difference, set(common_dirs)
	# print("files_to_add:", files_to_add)
	# print("files_to_delete:", files_to_delete)
	# print("files_with_difference:", files_with_difference)
	
"""

Files in new but not old: add to duckypad

Files in old but not new: delete from duckypad

Files in both but different content: delete from duckypad then write new version

"""


# if len(sys.argv) != 3:
# 	print(__file__, "old_folder new_folder")
# 	exit()

# old_dir = sys.argv[1]
# new_dir = sys.argv[2]

# print(compare(old_dir, new_dir))