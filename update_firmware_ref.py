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

readme_file = open('firmware_updates_and_version_history.md', encoding='utf8')
readme_lines = readme_file.readlines()
readme_file.close()

changes_made = False
for index, line in enumerate(readme_lines):
	if 'click me' in line.lower() and 'to download the latest firmware' in line.lower():
		readme_lines[index] = "* [Click me](https://github.com/dekuNukem/duckyPad/raw/master/firmware/" + str(latest_dfu_file_name) + ") to download the latest firmware (v" + latest_dfu_file_name.replace('duckypad_v', '').replace('.dfu', '') + ")\n"
		changes_made = True

if changes_made:
	readme_file = open('firmware_updates_and_version_history.md', 'w', encoding='utf8')
	readme_file.writelines(readme_lines)
	readme_file.close()
	print('update_firmware_ref: Success')
else:
	print('update_firmware_ref: TARGET STRING NOT FOUND, CHECK MD FILE!!!!!!!!!!!')

try:
	discord_path = os.path.join('.', 'resources')
	discord_path = os.path.join(discord_path, 'discord_link.txt')
	discord_link = None
	with open(discord_path) as discord_file:
		for line in discord_file:
			line = line.replace('\n', '').replace('\r', '').strip()
			if line.startswith('https://discord.gg/'):
				discord_link = line
except Exception as e:
	print("!!!!!!!!!!discord link exception:", e)
	exit()

if discord_link is None:
	print("DISCORD LINK NOT FOUND!!!!!!!!!!")
	exit()

md_file_list = [d for d in os.listdir('.') if d.lower().endswith('.md')]

for filename in md_file_list:
	this_md_file = open(filename, encoding='utf-8')
	readme_lines = this_md_file.readlines()
	this_md_file.close()
	changes_made = False

	for index, line in enumerate(readme_lines):
		if '[official duckyPad discord](' in line:
			ssss = line.split('[official duckyPad discord](')
			front = ssss[0]
			back = ssss[1].split(')', 1)[-1]
			readme_lines[index] = front + "[official duckyPad discord](" + discord_link + ")" + back
			print(readme_lines[index])
			changes_made = True
		elif '[Official Discord](' in line:
			ssss = line.split('[Official Discord](')
			front = ssss[0]
			back = ssss[1].split(')', 1)[-1]
			readme_lines[index] = front + "[Official Discord](" + discord_link + ")" + back
			print(readme_lines[index])
			changes_made = True

	if changes_made:
		this_md_file = open(filename, 'w', encoding='utf-8')
		this_md_file.writelines(readme_lines)
		this_md_file.close()
		print('discord link updated on', filename)
