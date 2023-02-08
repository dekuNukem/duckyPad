import os

THIS_VERSION = None
try:
	mainfile = open('duckypad_config.py')
	for line in mainfile:
		if "THIS_VERSION_NUMBER =" in line:
			THIS_VERSION = line.replace('\n', '').replace('\r', '').split("'")[-2]
	mainfile.close()
except Exception as e:
	print('build_windows exception:', e)
	exit()

if THIS_VERSION is None:
	print('could not find version number!')
	exit()

zip_file_name = "duckypad_config_" + THIS_VERSION + "_source.zip"
zip_file_name = "duckypad_config_latest_source.zip"
os.system("rm -fv ./*.zip")
os.system(f"7z.exe a {zip_file_name} ./ -x!_*")
