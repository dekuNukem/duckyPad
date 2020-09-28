import os
import sys
import string
import shutil
import random

profile_number = 100

root_name = 'duck_test'

if os.path.isdir(root_name):
	shutil.rmtree(root_name)
os.mkdir(root_name)

color_pool = [8, 16, 32, 64]

def get_random_name():
	return ''.join(random.choices(string.ascii_lowercase + string.digits, k=5))

def get_random_colors():
	ret = ''
	for item in random.choices(color_pool, k=3):
		ret += str(item) + ' '
	return ret

root_dir = os.path.join(".", root_name)
for x in range(1, profile_number + 1):
	profile_dir_name = os.path.join(root_dir, 'profile' + str(x) + "_" + get_random_name())
	os.mkdir(profile_dir_name)
	config_name = os.path.join(profile_dir_name, 'config.txt')
	config_content = ''
	for y in range(1, 16):
		config_content += 'z' + str(y) + ' ' + get_random_name() + "\n"
	for y in range(1, 16):
		config_content += 'SWCOLOR_' + str(y) + ' ' + get_random_colors() + "\n"
	print(config_content)

	with open(config_name, 'w') as config_file:
		config_file.write(config_content)

	for y in range(1, 16):
		script_name = os.path.join(profile_dir_name, 'key' + str(y) + '.txt')
		script_content = 'STRING ' + get_random_name() + '\nENTER\n'
		with open(script_name, 'w') as script_file:
			script_file.write(script_content)
