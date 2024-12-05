import os
import time
import sys
import psutil

removable_drives = [x for x in psutil.disk_partitions() if ('removable' in x.opts.lower() and 'fat' in x.fstype.lower())]
if len(removable_drives) == 0:
    print("no removable drives found")
    exit()

sample_profile_path = "C:\\Users\\allen\\Desktop\\repos\\duckyPad\\sample_profiles\\"

for item in removable_drives:
    print("removable drive:", item.mountpoint)
    os.system(f"del /s /q {item.mountpoint}*.* && rmdir /s /q {item.mountpoint}")
    os.system(f"xcopy {sample_profile_path} {item.mountpoint} /e /i /y")