import os
import sys
import shutil

os.system('rm -rfv ./__pycache__')
os.system('rm -rfv ./build')
os.system('rm -rfv ./dist')
os.system('rm -rfv ./*.zip')

THIS_VERSION = None
try:
	mainfile = open('duckypad_config.py')
	for line in mainfile:
		if "THIS_VERSION_NUMBER =" in line:
			THIS_VERSION = line.replace('\n', '').replace('\r', '').split("'")[-2]
	mainfile.close()
except Exception as e:
	print('build_linux exception:', e)
	exit()

if THIS_VERSION is None:
	print('could not find version number!')
	exit()

print(THIS_VERSION)
py_file_list = [x for x in os.listdir('.') if x.endswith('.py')]
py_file_list = [x for x in py_file_list if 'build_' not in x.lower() and 'setup.py' not in x.lower()]
py_file_list.append('duckypad_config_install_linux_only.sh')
print(py_file_list)

output_dir_name = "duckypad_config_" + THIS_VERSION + "_source"
if os.path.isdir(output_dir_name):
	shutil.rmtree(output_dir_name)
os.mkdir(output_dir_name)
output_dir_path = os.path.join('.', output_dir_name)
print(output_dir_path)

for item in py_file_list:
	shutil.copy(item, output_dir_path)

zip_file_name = "duckypad_config_" + THIS_VERSION + "_source.zip"
os.system('7z.exe a ' + zip_file_name + ' -r ' + output_dir_path)
if os.path.isdir(output_dir_path):
	shutil.rmtree(output_dir_path)