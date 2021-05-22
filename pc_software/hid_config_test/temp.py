# pyinstaller.exe .\dump_from_hid.py

def dump_from_hid():
	file_struct_dict = {}

	# top level
	for item in duckypad_list_files():
		file_struct_dict[item[0]] = {'type':item[1], 'content':None}

	for key in file_struct_dict:
		print(key)
		time.sleep(0.05)
		if file_struct_dict[key]['type'] == 0: # file
			file_struct_dict[key]['content'] = duckypad_read_file(key)
		if file_struct_dict[key]['type'] == 1: # directory
			files_in_this_dir = duckypad_list_files(key)
			print(files_in_this_dir)
			print(file_struct_dict)
			# for item in files_in_this_dir:
			# 	file_struct_dict[key]['content'] = {'type':item[1], 'content':None}

			# file_struct_dict[key]['content'] = 
			input()

	print(file_struct_dict)
