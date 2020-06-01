import json
import socket
import urllib.request

remote_url = "https://raw.githubusercontent.com/dekuNukem/duckyPad/master/pc_software/release_info.json"

def is_internet_available():
    try:
        socket.create_connection(("www.google.com", 80))
        return True
    except OSError:
        pass
    return False

def versiontuple(v):
    return tuple(map(int, (v.split("."))))

def has_update(this_version):
	if is_internet_available() is False:
		return False
	try:
		result_dict = json.loads(urllib.request.urlopen(remote_url).read())
		this_version = versiontuple(this_version)
		remote_version = versiontuple(result_dict['latest_version'])
		return remote_version > this_version
	except Exception as e:
		print('has_update:', e)
		return False

print(has_update('0.1.0'))