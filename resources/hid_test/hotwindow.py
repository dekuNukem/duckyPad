import wmi
import time
import win32process
import pygetwindow as gw

def get_app_name(hwnd):
    """Get applicatin path given hwnd."""
    try:
        _, pid = win32process.GetWindowThreadProcessId(hwnd)
        for p in c.query('SELECT Name FROM Win32_Process WHERE ProcessId = %s' % str(pid)):
            exe = p.Name
            break
    except:
        return None
    else:
        return exe

c = wmi.WMI()

while 1:
	active_window = gw.getActiveWindow()
	print(get_app_name(active_window._hWnd), active_window.title)
	time.sleep(0.5)



# def get_app_path(hwnd):
#     """Get applicatin path given hwnd."""
#     try:
#         _, pid = win32process.GetWindowThreadProcessId(hwnd)
#         for p in c.query('SELECT ExecutablePath FROM Win32_Process WHERE ProcessId = %s' % str(pid)):
#             exe = p.ExecutablePath
#             break
#     except:
#         return None
#     else:
#         return exe