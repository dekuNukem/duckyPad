import time
import os
import sys
import filecmp

if len(sys.argv) < 3:
	print(__file__, "duckypad_dir local_dir")

duckypad_dir_name = sys.argv[1]
local_dir_name = sys.argv[2]

# print(duckypad_dir_name)
# print(local_dir_name)

# print(filecmp.dircmp(duckypad_dir_name, local_dir_name).report_partial_closure())

compare_result = filecmp.dircmp(duckypad_dir_name, local_dir_name, ignore=['keymaps','SYSTEM~1'])
top_level_to_copy = compare_result.right_only
print("top_level_to_copy", top_level_to_copy)
top_level_to_remove = compare_result.left_only
print("top_level_to_remove", top_level_to_remove)

for item in top_level_to_remove:
	print("removing...", item)

for item in top_level_to_copy:
	item_path = os.path.join(local_dir_name, item)
	print("copying...", item_path)
	if os.path.isfile(item_path):
		print("this is a file!")
		continue
	sub_dir_path = [os.path.join(item_path, x) for x in os.listdir(item_path)]
	my_files = [d for d in sub_dir_path if os.path.isfile(d)]
	for xxx in my_files:
		print("\tcopying...", xxx)

for common_dir_name in compare_result.common_dirs:
	subdir_compare_result = filecmp.dircmp(os.path.join(duckypad_dir_name, common_dir_name), os.path.join(local_dir_name, common_dir_name))
	subdir_file_to_copy = subdir_compare_result.right_only + subdir_compare_result.diff_files
	print("subdir_file_to_copy", subdir_file_to_copy)
	subdir_file_to_remove = subdir_compare_result.left_only
	print("subdir_file_to_remove", subdir_file_to_remove)

"""
i want left folder to be like right

find files and folders in right thats not in left, add them

find files and folders in left thats not in right, delete them

go into subdirs and do the same
"""