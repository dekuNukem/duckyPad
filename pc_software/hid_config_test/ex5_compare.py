import time
import os
import sys
import filecmp

if len(sys.argv) < 3:
	print(__file__, "old_dir new_dir")

old_dir_name = sys.argv[1]
new_dir_name = sys.argv[2]

# print(old_dir_name)
# print(new_dir_name)

# print(filecmp.dircmp(old_dir_name, new_dir_name).report_partial_closure())

print(filecmp.dircmp(old_dir_name, new_dir_name).diff_files)