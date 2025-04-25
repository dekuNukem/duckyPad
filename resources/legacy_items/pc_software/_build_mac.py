import os
import sys
import platform

"""
https://py2app.readthedocs.io/en/latest/tutorial.html

OPTIONS = {'iconfile':'icon.icns'}

rm -rf build dist

python3.8 setup.py py2app
"""

if 'darwin' not in sys.platform:
    print("this script is for macOS only!")
    exit()

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
	print('build_mac exception:', e)
	exit()

if THIS_VERSION is None:
	print('could not find version number!')
	exit()

print(THIS_VERSION)
os.system("python3.12 setup.py py2app --packages=PIL")

input("PRESS ANY KEY TO CONTINUE...")

output_folder_path = os.path.join('.', "dist")
original_name = os.path.join(output_folder_path, "duckypad_config.app")
new_name = os.path.join(output_folder_path, "duckypad_config_" + THIS_VERSION + "_macOS_" + str(platform.mac_ver()[0]) + ".app")

print(original_name)
print(new_name)
print()

f = open(os.path.join(output_folder_path, "run.sh"), "w")
f.write("sudo " + str(os.path.join(new_name, 'Contents', 'MacOS', 'duckypad_config')).replace("dist/", ''))
f.close()

os.rename(original_name, new_name)
zip_file_name = "duckypad_config_" + THIS_VERSION + "_macOS_" + str(platform.mac_ver()[0]) +".zip"
os.system('cd dist; zip -rv ' + zip_file_name + ' ' + new_name.split('/')[-1] + " run.sh; mv " + zip_file_name + " ../")
