import os
import sys

if not (sys.version_info.major >= 3 and sys.version_info.minor >= 4):
	print('update_firmware_ref: PYTHON VERISON TOO LOW, 3.4+ REQUIRED')
	exit()

try:
	dfu_folder = os.path.join('.', 'firmware')
	dfu_file_list = [d for d in os.listdir(dfu_folder) if d.startswith('duckypad_v') and d.endswith('.dfu')]
	dfu_file_list.sort(key=lambda s: list(map(int, s.replace('duckypad_v', '').replace('.dfu', '').split('.'))))
	latest_dfu_file_name = dfu_file_list[-1]
	print('latest dfu file is:', latest_dfu_file_name)
except Exception as e:
	print("update_firmware_ref: get latest dfu exception:", e)
	exit()

readme_file = open('getting_started.md')
readme_lines = readme_file.readlines()
readme_file.close()

changes_made = False
for index, line in enumerate(readme_lines):
	if 'click me' in line.lower() and 'to download the latest firmware' in line.lower():
		readme_lines[index] = "* [Click me](https://github.com/dekuNukem/duckyPad/raw/master/firmware/" + str(latest_dfu_file_name) + ") to download the latest firmware\n"
		changes_made = True

if changes_made:
	readme_file = open('getting_started.md', 'w')
	readme_file.writelines(readme_lines)
	readme_file.close()
	print('update_firmware_ref: Success')
else:
	print('update_firmware_ref: TARGET STRING NOT FOUND, CHECK MD FILE!!!!!!!!!!!')