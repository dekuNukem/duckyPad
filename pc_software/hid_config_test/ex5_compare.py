import time
import os
import sys
import filecmp
import hid_op

if len(sys.argv) < 3:
    print(__file__, "duckypad_dir local_dir")
    exit()

def get_file_content(file_path):
    this_file = open(file_path, encoding='utf-8')
    content = this_file.read()
    this_file.close()
    return content

duckypad_dir_name = sys.argv[1]
local_dir_name = sys.argv[2]

# print(duckypad_dir_name)
# print(local_dir_name)

# print(filecmp.dircmp(duckypad_dir_name, local_dir_name).report_partial_closure())
hid_op.duckypad_hid_init()

compare_result = filecmp.dircmp(duckypad_dir_name, local_dir_name, ignore=['keymaps','SYSTEM~1'])
top_level_to_copy = compare_result.right_only
print("top_level_to_copy", top_level_to_copy)
top_level_to_remove = compare_result.left_only
print("top_level_to_remove", top_level_to_remove)

# for item in top_level_to_remove:
#     item_path = os.path.join(duckypad_dir_name, item)
#     print("removing...", item_path)
#     if os.path.isfile(item_path):
#         print("this is a file!")
#         hid_op.duckypad_delete_file(item)
#     elif os.path.isdir(item_path):
#         print("this is a dir")
#         hid_op.duckypad_delete_dir(item)

# for item in top_level_to_copy:
#     item_path = os.path.join(local_dir_name, item)
#     print("copying...", item_path)
#     if os.path.isfile(item_path):
#         print("this is a file!")
#         hid_op.duckypad_open_file_for_writing(item)
#         hid_op.duckypad_write_file(get_file_content(item_path))
#         hid_op.duckypad_close_file()
#         continue
#     # this is a dir, create a new dir first
#     print("creating dir:", item)
#     hid_op.duckypad_create_dir(item)
#     sub_dir_path = [(os.path.join(item_path, x), os.path.join(item, x)) for x in os.listdir(item_path)]
#     my_files = [d for d in sub_dir_path if os.path.isfile(d[0])]
#     print(my_files)
#     for xxx in my_files:
#         file_path = xxx[0]
#         fatfs_path = xxx[1]
#         print("\tcopying...", fatfs_path)
#         hid_op.duckypad_open_file_for_writing(fatfs_path)
#         hid_op.duckypad_write_file(get_file_content(file_path))
#         hid_op.duckypad_close_file()

for common_dir_name in compare_result.common_dirs:
    subdir_compare_result = filecmp.dircmp(os.path.join(duckypad_dir_name, common_dir_name), os.path.join(local_dir_name, common_dir_name))
    
    subdir_file_to_copy = subdir_compare_result.right_only + subdir_compare_result.diff_files
    print("subdir_file_to_copy", subdir_file_to_copy)

    for item in subdir_file_to_copy:
        fatfs_path = os.path.join(common_dir_name, item)
        file_path = os.path.join(local_dir_name, fatfs_path)
        print("\tcopying...", fatfs_path)
        hid_op.duckypad_open_file_for_writing(fatfs_path)
        hid_op.duckypad_write_file(get_file_content(file_path))
        hid_op.duckypad_close_file()


    subdir_file_to_remove = subdir_compare_result.left_only
    print("subdir_file_to_remove", subdir_file_to_remove)

    for item in subdir_file_to_remove:
        fatfs_path = os.path.join(common_dir_name, item)
        print("\tdeleting...", fatfs_path)
        hid_op.duckypad_delete_file(fatfs_path)

"""
i want left folder to be like right

find files and folders in right thats not in left, add them

find files and folders in left thats not in right, delete them

go into subdirs and do the same
"""