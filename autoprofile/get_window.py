import wmi
import time
import win32process
import pygetwindow as gw

c = wmi.WMI()

def get_app_name(hwnd):
    """Get application name given hwnd."""
    try:
        _, pid = win32process.GetWindowThreadProcessId(hwnd)
        for p in c.query('SELECT Name FROM Win32_Process WHERE ProcessId = %s' % str(pid)):
            exe = p.Name
            break
    except:
        return None
    else:
        return exe

# returns a list of (app_name, window_title) tuples
def get_list_of_all_windows():
	ret = set()
	for item in gw.getAllWindows():
		ret.add((get_app_name(item._hWnd).rsplit('.', 1)[0], item.title))
	ret = sorted(list(ret), key=lambda x: x[0])
	return ret

# returns a (app_name, window_title) tuple
def get_active_window():
	active_window = gw.getActiveWindow()
	return (get_app_name(active_window._hWnd), active_window.title)

# you can test it out here:
# if __name__ == "__main__":
# 	while 1:
# 		print(get_active_window())
# 		time.sleep(0.5)

# def get_app_path(hwnd):
#     """Get application path given hwnd."""
#     try:
#         _, pid = win32process.GetWindowThreadProcessId(hwnd)
#         for p in c.query('SELECT ExecutablePath FROM Win32_Process WHERE ProcessId = %s' % str(pid)):
#             exe = p.ExecutablePath
#             break
#     except:
#         return None
#     else:
#         return exe