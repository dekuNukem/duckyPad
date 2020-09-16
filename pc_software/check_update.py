import json
import socket
import urllib.request

pc_app_release_url = "https://api.github.com/repos/dekuNukem/duckyPad/releases/latest"
firmware_url = 'https://api.github.com/repos/dekuNukem/duckyPad/contents/firmware?ref=master'

def is_internet_available():
    try:
        socket.create_connection(("www.google.com", 80))
        return True
    except OSError:
        pass
    return False

def versiontuple(v):
    return tuple(map(int, (v.strip('v').split("."))))

"""
0 no update
1 has update
2 unknown
"""
def get_pc_app_update_status(this_version):
	if is_internet_available() is False:
		return 2
	try:
		result_dict = json.loads(urllib.request.urlopen(pc_app_release_url).read())
		this_version = versiontuple(this_version)
		remote_version = versiontuple(result_dict['tag_name'])
		return int(remote_version > this_version)
	except Exception as e:
		print('has_update:', e)
		return 2

file_list = json.loads(urllib.request.urlopen(firmware_url).read())

dfu_list = [x for x in file_list if x['type'] == 'file']
print(dfu_list)

# print(has_update('0.0.9'))